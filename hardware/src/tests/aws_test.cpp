#include <Arduino.h>
#include "aws_service.h"

void testAWSConnection() {
    Serial.println("Testing AWS IoT connection...");

    if (isAWSConnected()) {
        Serial.println("✅ AWS IoT is connected!");
    } else {
        Serial.println("❌ AWS IoT is NOT connected!");
    }
    Serial.println("=================");
}
