#ifndef ALARM_SERVICE_H
#define ALARM_SERVICE_H

#include <ArduinoJson.h>

void saveAlarmsToFlash();
void loadAlarmsFromFlash();
void loadScheduleFromDelta(JsonArray arr);
void printSchedule();
bool ifAlarmEnable();


#endif