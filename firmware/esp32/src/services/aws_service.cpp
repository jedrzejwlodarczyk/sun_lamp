/**
 * @file aws_service.cpp
 * @brief AWS IoT Core Device Shadow service implementation
 * 
 * Implements proper Device Shadow protocol with delta handling
 * CRITICAL: Uses AWS IoT Device Shadow topics, not custom topics
 */

#include "aws_service.h"
#include "secrets.h"
#include "lamp_service.h"
#include "alarm_service.h"
#include "hardware_service.h"
#include "hardware_config.h"
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>

// ==================== Device Shadow Topics ====================
// CRITICAL: Must use AWS IoT Device Shadow topics, not custom topics
String SHADOW_UPDATE_TOPIC;
String SHADOW_UPDATE_DELTA_TOPIC;
String SHADOW_UPDATE_ACCEPTED_TOPIC;
String SHADOW_UPDATE_REJECTED_TOPIC;
String SHADOW_GET_TOPIC;

// ==================== MQTT Client ====================
static WiFiClientSecure net;
static MQTTClient mqttClient(512);  // Increased buffer for shadow messages

// ==================== Forward Declarations ====================
static void messageHandler(String &topic, String &payload);
static void handleShadowDelta(JsonDocument& doc);
static void handleShadowAccepted(JsonDocument& doc);
static void handleShadowRejected(JsonDocument& doc);

// ==================== Initialization ====================

void initializeAWS() {
    // Build Device Shadow topic strings using THINGNAME
    SHADOW_UPDATE_TOPIC = "$aws/things/" + String(THINGNAME) + "/shadow/update";
    SHADOW_UPDATE_DELTA_TOPIC = "$aws/things/" + String(THINGNAME) + "/shadow/update/delta";
    SHADOW_UPDATE_ACCEPTED_TOPIC = "$aws/things/" + String(THINGNAME) + "/shadow/update/accepted";
    SHADOW_UPDATE_REJECTED_TOPIC = "$aws/things/" + String(THINGNAME) + "/shadow/update/rejected";
    SHADOW_GET_TOPIC = "$aws/things/" + String(THINGNAME) + "/shadow/get";
    
    Serial.println("🔐 Configuring AWS IoT certificates...");
    
    // Configure X.509 certificates for mutual TLS
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);
    
    // Connect to AWS IoT MQTT broker
    mqttClient.begin(AWS_IOT_ENDPOINT, 8883, net);
    mqttClient.onMessage(messageHandler);
    
    Serial.println("📡 Connecting to AWS IoT Core...");
    
    int retries = 0;
    while (!mqttClient.connect(THINGNAME) && retries < 5) {
        Serial.print(".");
        delay(1000);
        retries++;
    }
    
    if (!mqttClient.connected()) {
        Serial.println("\n❌ AWS IoT connection failed!");
        setConnectionLED(COLOR_RED);
        updateStatusLEDs();
        return;
    }
    
    Serial.println("\n✅ Connected to AWS IoT Core");
    
    // Subscribe to Device Shadow topics
    Serial.println("📥 Subscribing to Device Shadow topics...");
    mqttClient.subscribe(SHADOW_UPDATE_DELTA_TOPIC);
    mqttClient.subscribe(SHADOW_UPDATE_ACCEPTED_TOPIC);
    mqttClient.subscribe(SHADOW_UPDATE_REJECTED_TOPIC);
    
    Serial.printf("  ✓ %s\n", SHADOW_UPDATE_DELTA_TOPIC.c_str());
    Serial.printf("  ✓ %s\n", SHADOW_UPDATE_ACCEPTED_TOPIC.c_str());
    Serial.printf("  ✓ %s\n", SHADOW_UPDATE_REJECTED_TOPIC.c_str());
    
    // Set connection LED to green
    setConnectionLED(COLOR_GREEN);
    updateStatusLEDs();
    
    // Request current Device Shadow state
    requestDeviceShadow();
}

// ==================== Connection Status ====================

bool isAWSConnected() {
    return mqttClient.connected();
}

// ==================== Message Routing ====================

static void messageHandler(String &topic, String &payload) {
    Serial.printf("📩 Received on topic: %s\n", topic.c_str());
    Serial.printf("Payload: %s\n", payload.c_str());
    
    // Parse JSON payload
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    
    if (error) {
        Serial.printf("❌ JSON parse error: %s\n", error.c_str());
        return;
    }
    
    // Route to appropriate handler based on topic
    if (topic == SHADOW_UPDATE_DELTA_TOPIC) {
        handleShadowDelta(doc);
    } else if (topic == SHADOW_UPDATE_ACCEPTED_TOPIC) {
        handleShadowAccepted(doc);
    } else if (topic == SHADOW_UPDATE_REJECTED_TOPIC) {
        handleShadowRejected(doc);
    } else {
        Serial.printf("⚠️ Unknown topic: %s\n", topic.c_str());
    }
}

// ==================== Shadow Delta Handler ====================

static void handleShadowDelta(JsonDocument& doc) {
    Serial.println("🔄 Processing shadow delta...");
    
    JsonObject state = doc["state"];
    
    // Handle lamp state changes
    if (state.containsKey("lamp")) {
        JsonObject lamp = state["lamp"];
        
        bool on = lamp["on"] | isLampOn();
        uint8_t ww = lamp["ww"] | getWarmWhite();
        uint8_t cw = lamp["cw"] | getCoolWhite();
        bool boost = lamp["boost"] | isBoostEnabled();
        
        setLampState(on, ww, cw, boost);
        Serial.printf("💡 Lamp updated: on=%d, ww=%d, cw=%d, boost=%d\n", on, ww, cw, boost);
    }
    
    // Handle alarm schedule changes
    if (state.containsKey("alarm")) {
        JsonObject alarm = state["alarm"];
        
        if (alarm.containsKey("schedule")) {
            JsonArray schedule = alarm["schedule"];
            updateAlarmSchedule(schedule);
            Serial.println("⏰ Alarm schedule updated");
        }
        
        if (alarm.containsKey("enabled")) {
            bool enabled = alarm["enabled"];
            if (enabled != isAnyAlarmEnabled()) {
                toggleAlarmEnabled();
            }
        }
    }
    
    // After applying delta, report new state
    publishReportedState();
}

// ==================== Shadow Update Handlers ====================

static void handleShadowAccepted(JsonDocument& doc) {
    Serial.println("✅ Shadow update accepted");
    // Optional: Could update local state from accepted response
}

static void handleShadowRejected(JsonDocument& doc) {
    Serial.println("❌ Shadow update rejected");
    if (doc.containsKey("message")) {
        Serial.printf("Reason: %s\n", doc["message"].as<const char*>());
    }
}

// ==================== Publish Reported State ====================

void publishReportedState() {
    if (!mqttClient.connected()) {
        Serial.println("⚠️ Cannot publish: not connected to AWS");
        return;
    }
    
    JsonDocument doc;
    JsonObject state = doc["state"].to<JsonObject>();
    JsonObject reported = state["reported"].to<JsonObject>();
    
    // Lamp state
    JsonObject lamp = reported["lamp"].to<JsonObject>();
    lamp["on"] = isLampOn();
    lamp["ww"] = getWarmWhite();
    lamp["cw"] = getCoolWhite();
    lamp["boost"] = isBoostEnabled();
    
    // Alarm state
    JsonObject alarm = reported["alarm"].to<JsonObject>();
    alarm["enabled"] = isAnyAlarmEnabled();
    
    // Health metrics
    JsonObject health = reported["health"].to<JsonObject>();
    health["rssi"] = WiFi.RSSI();
    health["uptime"] = millis() / 1000;
    health["freeHeap"] = ESP.getFreeHeap();
    
    // Serialize and publish
    String payload;
    serializeJson(doc, payload);
    
    Serial.println("📤 Publishing reported state...");
    Serial.println(payload);
    
    if (mqttClient.publish(SHADOW_UPDATE_TOPIC, payload)) {
        Serial.println("✅ Reported state published");
    } else {
        Serial.println("❌ Failed to publish reported state");
    }
}

// ==================== Request Device Shadow ====================

void requestDeviceShadow() {
    if (!mqttClient.connected()) {
        Serial.println("⚠️ Cannot request shadow: not connected");
        return;
    }
    
    Serial.println("📥 Requesting current Device Shadow...");
    mqttClient.publish(SHADOW_GET_TOPIC, "{}");
}

// ==================== Connection Maintenance ====================

void maintainAWSConnection() {
    if (!mqttClient.connected()) {
        Serial.println("🔌 AWS IoT disconnected, reconnecting...");
        setConnectionLED(COLOR_BLUE);
        updateStatusLEDs();
        initializeAWS();
    } else {
        mqttClient.loop();
    }
}
