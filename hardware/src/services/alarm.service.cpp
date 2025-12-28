#include <Arduino.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include "alarm_service.h"

#define MAX_ALARMS 10

Preferences prefs;

String alarmTimes[MAX_ALARMS];
bool alarmEnabled[MAX_ALARMS];
int alarmDays[MAX_ALARMS][7];
int alarmDaysCount[MAX_ALARMS];

int alarmCount = 0;


void saveAlarmsToFlash() {
    prefs.begin("alarms", false);
    prefs.putInt("count", alarmCount);

    for (int i = 0; i < alarmCount; i++) {
        prefs.putString(("time" + String(i)).c_str(), alarmTimes[i]);
        prefs.putBool(("ena" + String(i)).c_str(), alarmEnabled[i]);
        prefs.putInt(("daysCnt" + String(i)).c_str(), alarmDaysCount[i]);

        for (int d = 0; d < alarmDaysCount[i]; d++) {
            prefs.putInt(
                ("day" + String(i) + "_" + String(d)).c_str(),
                alarmDays[i][d]
            );
        }
    }

    prefs.end();
    Serial.println("💾 Saved alarms to flash");
}


void loadAlarmsFromFlash() {
    prefs.begin("alarms", true);
    alarmCount = prefs.getInt("count", 0);

    if (alarmCount == 0) {
        Serial.println("⚠️ No alarms in flash");
        prefs.end();
        return;
    }

    for (int i = 0; i < alarmCount; i++) {
        alarmTimes[i]     = prefs.getString(("time" + String(i)).c_str(), String("00:00"));
        alarmEnabled[i]   = prefs.getBool(("ena" + String(i)).c_str(), false);
        alarmDaysCount[i] = prefs.getInt(("daysCnt" + String(i)).c_str(), 0);

        for (int d = 0; d < alarmDaysCount[i]; d++) {
            alarmDays[i][d] = prefs.getInt(
                ("day" + String(i) + "_" + String(d)).c_str(), 
                0
            );
        }
    }

    prefs.end();
    Serial.println("📥 Loaded alarms from flash");
}

void loadScheduleFromDelta(JsonArray arr) {
    alarmCount = 0;

    for (JsonObject item : arr) {
        if (alarmCount >= MAX_ALARMS) break;

        alarmTimes[alarmCount] = item["time"].as<String>();
        alarmEnabled[alarmCount] = item["enabled"].as<bool>();

        JsonArray days = item["days"];
        alarmDaysCount[alarmCount] = days.size();

        for (int i = 0; i < days.size() && i < 7; i++) {
            alarmDays[alarmCount][i] = days[i].as<int>();
        }

        alarmCount++;
    }
    saveAlarmsToFlash();
}

void printSchedule() {
    Serial.println("===== CURRENT SCHEDULE =====");

    if (alarmCount == 0) {
        Serial.println("No alarms defined.");
        return;
    }

    for (int i = 0; i < alarmCount; i++) {
        Serial.print("Alarm #");
        Serial.print(i);
        Serial.println(":");

        Serial.print("  Time: ");
        Serial.println(alarmTimes[i]);

        Serial.print("  Enabled: ");
        Serial.println(alarmEnabled[i] ? "true" : "false");

        Serial.print("  Days: ");
        for (int d = 0; d < alarmDaysCount[i]; d++) {
            Serial.print(alarmDays[i][d]);
            if (d < alarmDaysCount[i] - 1) Serial.print(", ");
        }
        Serial.println();
        Serial.println();
    }

    Serial.println("============================");
}


bool ifAlarmEnable(){
    for(int i=0; i<alarmCount; i++){
        if(alarmEnabled[i] == true){
            return true;
        }
    }
    return false;
} 



