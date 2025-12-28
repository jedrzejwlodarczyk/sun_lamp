#include "secrets.h"
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "lamp_service.h"
#include "alarm_service.h"
#include <string>
#include <vector>

#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure net;
MQTTClient client = MQTTClient(256);

void messageHandler(String &topic, String &payload) {
    Serial.println("📩 Received from AWS:");
    
    StaticJsonDocument<512> doc;
    DeserializationError err = deserializeJson(doc, payload);
    if (err) {
        Serial.println("❌ Failed to parse delta");
        return;
    }
    
    bool desiredLampState = doc["state"]["lamp"]["on"];
    int desiredWW = doc["state"]["lamp"]["ww"];
    int desiredCW = doc["state"]["lamp"]["cw"];
    bool desiredBoost = doc["state"]["lamp"]["boost"];
    

    setLampState(desiredLampState);
    setWW(desiredWW);
    setCW(desiredCW);
    setBoost(desiredBoost);

    if (doc["state"]["alarm"].containsKey("schedule")) {
        JsonArray scheduleArr = doc["state"]["alarm"]["schedule"].as<JsonArray>();
        loadScheduleFromDelta(scheduleArr);
    }
    
    //publishReportedState();
}

void connectToAWS(){
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);

    // Connect to the MQTT broker on the AWS endpoint we defined earlier
    client.begin(AWS_IOT_ENDPOINT, 8883, net);

    // Create a message handler
    client.onMessage(messageHandler);

    if(!client.connected()){
        return;
    }

    // Subscribe to a topic
    client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

    client.publish("$aws/things/device/shadow/get", "{}");
}

bool isAWSConnected(){
    return client.connected();
}

void publishReportedState() {
    StaticJsonDocument<512> doc;

    doc["state"]["reported"]["lamp"]["on"] = getLampState();
    doc["state"]["reported"]["lamp"]["ww"] = getWW();
    doc["state"]["reported"]["lamp"]["cw"] = getCW();
    doc["state"]["reported"]["lamp"]["boost"] = getBoost();
    doc["state"]["reported"]["lamp"]["status"] = "ok";

    doc["state"]["reported"]["alarm"]["enabled"] = ifAlarmEnable();

    doc["state"]["reported"]["health"]["rssi"] = WiFi.RSSI();
    doc["state"]["reported"]["health"]["uptime"] = millis() / 1000;
    doc["state"]["reported"]["health"]["free_heap"] = ESP.getFreeHeap();

    
    Serial.println("📤 [LOCAL] Reported state:");
    serializeJsonPretty(doc, Serial);
    Serial.println();                  


    // char buffer[512]; 
    // serializeJson(doc, buffer); 
    // client.publish(AWS_IOT_PUBLISH_TOPIC, buffer);
}

void maintainAWS() {
    if (!client.connected()) {
        connectToAWS();
    } else {
        client.loop();
    }
}
