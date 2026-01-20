/**
 * @file ntp_service.cpp
 * @brief NTP time synchronization service implementation
 */

#include "ntp_service.h"
#include <WiFi.h>
#include <time.h>
#include <Arduino.h>

// NTP Server and timezone configuration
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 3600;        // UTC+1 for Poland (adjust as needed)
const int DAYLIGHT_OFFSET_SEC = 0;       // Set to 3600 for daylight saving time

void initializeNTP() {
    Serial.println("🕒 Initializing NTP...");
    
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    
    // Wait for time synchronization
    struct tm timeinfo;
    int retries = 10;
    while (!getLocalTime(&timeinfo) && retries > 0) {
        Serial.print(".");
        delay(1000);
        retries--;
    }
    Serial.println();
    
    if (retries > 0) {
        Serial.println("✅ NTP synchronized");
        printCurrentTime();
    } else {
        Serial.println("❌ Failed to sync NTP");
    }
}

void maintainNTPSync() {
    static unsigned long lastSync = 0;
    const unsigned long SYNC_INTERVAL = 3600000;  // Re-sync every hour
    
    if (WiFi.status() != WL_CONNECTED) return;
    
    unsigned long now = millis();
    if (now - lastSync >= SYNC_INTERVAL) {
        lastSync = now;
        
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            Serial.println("🔄 Re-syncing NTP...");
            initializeNTP();
        }
    }
}

void printCurrentTime() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        Serial.printf("🕒 Current time: %04d-%02d-%02d %02d:%02d:%02d\n",
                     timeinfo.tm_year + 1900,
                     timeinfo.tm_mon + 1,
                     timeinfo.tm_mday,
                     timeinfo.tm_hour,
                     timeinfo.tm_min,
                     timeinfo.tm_sec);
    } else {
        Serial.println("⚠️ Time not available");
    }
}
