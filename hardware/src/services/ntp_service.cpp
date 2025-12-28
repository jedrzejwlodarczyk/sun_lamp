#include <WiFi.h>
#include "time.h"

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;       
const int daylightOffset_sec = 0;   

void setupNTP() {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void maintainNTP() {
    static unsigned long lastAttempt = 0;
    const unsigned long retryEvery = 60 * 1000;

    if (WiFi.status() != WL_CONNECTED) return;

    if (millis() - lastAttempt > retryEvery) {
        lastAttempt = millis();
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            Serial.println("🔄 Re-syncing NTP...");
            setupNTP();
        }
    }
}