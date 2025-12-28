#include "test/wifi_test.h"
#include <WiFi.h>


void testConnectToWiFi() {
    Serial.println("=== Wi-Fi Test ===");
    Serial.println("Connecting to Wi-Fi...");
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("✅ Test Passed");

        Serial.print("IP: ");
        Serial.println(WiFi.localIP());

        Serial.print("RSSI: ");
        Serial.print(WiFi.RSSI());
        if (WiFi.RSSI() > -50){
            Serial.println("🟢🟢🟢🟢");
        }else if (WiFi.RSSI() <= -50 && WiFi.RSSI() > -70){
            Serial.println("🟢🟢🟢⚫");
        }else if (WiFi.RSSI() <= -70 && WiFi.RSSI() > -90){
            Serial.println("🟢🟢⚫⚫");
        }else{
            Serial.println("🟢⚫⚫⚫");
        }

    } else {
        Serial.println("❌ Test Failed");
    }

    Serial.println("=================");
}

