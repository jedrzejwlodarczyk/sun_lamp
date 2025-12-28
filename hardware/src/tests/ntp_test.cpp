#include "Arduino.h"
#include "time.h"
#include "test/ntp_test.h"

void testNTP() {
    Serial.println("=== NTP Test ===");
    Serial.println("Testing NTP...");

    struct tm timeinfo;
    int retries = 10;
    while (!getLocalTime(&timeinfo) && retries > 0) {
        Serial.println("⏳ Waiting for NTP...");
        delay(1000);
        retries--;
    }

    if (retries == 0) {
        Serial.println("❌ Failed to obtain time");
        return;
    }

    Serial.println("🕒 Time synchronized:");
    Serial.println(&timeinfo, "%H:%M:%S");

    Serial.println("=================");
}

