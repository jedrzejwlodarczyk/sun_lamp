#include <Arduino.h>
#include "wifi_service.h"
#include "test/wifi_test.h"
#include "ntp_service.h"
#include "test/ntp_test.h"
#include "time.h"
#include "aws_service.h"
#include "test/aws_test.h"
#include "alarm_service.h"


void setup() {
    Serial.begin(115200);

    connectToWiFi();
    testConnectToWiFi();

    setupNTP();
    testNTP();

    connectToAWS();
    testAWSConnection();

    loadAlarmsFromFlash();
    printSchedule();
}

void loop() {
  maintainNTP();
  maintainAWS();
}
