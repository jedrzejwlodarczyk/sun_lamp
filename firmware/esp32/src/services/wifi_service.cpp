/**
 * @file wifi_service.cpp
 * @brief WiFi connection service implementation
 */

#include "wifi_service.h"
#include "secrets.h"
#include "hardware_service.h"
#include "hardware_config.h"
#include <WiFi.h>
#include <Arduino.h>

void connectToWiFi() {
    Serial.println("📡 Connecting to WiFi...");
    Serial.printf("SSID: %s\n", WIFI_SSID);
    
    // Set connection LED to blue (connecting)
    setConnectionLED(COLOR_BLUE);
    updateStatusLEDs();
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    unsigned long startAttempt = millis();
    const unsigned long timeout = 20000;  // 20 seconds
    
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < timeout) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("✅ WiFi connected");
        Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
        
        // Set connection LED to green (connected, but AWS not yet)
        setConnectionLED(COLOR_GREEN);
        updateStatusLEDs();
    } else {
        Serial.println("❌ WiFi connection failed");
        setConnectionLED(COLOR_RED);
        updateStatusLEDs();
    }
}

bool isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

int getWiFiRSSI() {
    return WiFi.RSSI();
}
