#include <WiFi.h>
#include "secrets.h"

void connectToWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 15000; // 15 sekund

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
        delay(500);
    }
}

bool isConnectedtoWifi(){
    return WiFi.status() == WL_CONNECTED;
}

