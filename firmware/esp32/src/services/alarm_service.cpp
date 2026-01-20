/**
 * @file alarm_service.cpp
 * @brief Alarm domain service implementation
 * 
 * Manages alarm schedule with persistent storage and time-based triggers
 */

#include "alarm_service.h"
#include "lamp_service.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <time.h>

#define MAX_ALARMS 10

// ==================== Alarm Schedule Storage ====================
struct Alarm {
    String time;              // Format: "HH:MM"
    bool enabled;
    int days[7];              // Array of weekday numbers (1=Monday, 7=Sunday)
    int daysCount;
};

static Alarm alarms[MAX_ALARMS];
static int alarmCount = 0;
static bool globalAlarmEnabled = false;
static Preferences prefs;

// Track last trigger to prevent re-triggering within same minute
static int lastTriggerHour = -1;
static int lastTriggerMinute = -1;
static int lastTriggerDay = -1;

// ==================== Initialization ====================

void initializeAlarms() {
    loadAlarmsFromFlash();
    Serial.println("⏰ Alarm service initialized");
}

// ==================== Enable/Disable ====================

void toggleAlarmEnabled() {
    globalAlarmEnabled = !globalAlarmEnabled;
    Serial.printf("⏰ Global alarm %s\n", globalAlarmEnabled ? "ENABLED" : "DISABLED");
}

bool isAnyAlarmEnabled() {
    if (!globalAlarmEnabled) return false;
    
    for (int i = 0; i < alarmCount; i++) {
        if (alarms[i].enabled) {
            return true;
        }
    }
    return false;
}

// ==================== Schedule Management ====================

void updateAlarmSchedule(JsonArray scheduleArray) {
    alarmCount = 0;
    
    for (JsonObject item : scheduleArray) {
        if (alarmCount >= MAX_ALARMS) {
            Serial.println("⚠️ Maximum alarms reached");
            break;
        }
        
        alarms[alarmCount].time = item["time"].as<String>();
        alarms[alarmCount].enabled = item["enabled"].as<bool>();
        
        JsonArray daysArray = item["days"];
        alarms[alarmCount].daysCount = daysArray.size();
        
        for (int i = 0; i < daysArray.size() && i < 7; i++) {
            alarms[alarmCount].days[i] = daysArray[i].as<int>();
        }
        
        alarmCount++;
    }
    
    saveAlarmsToFlash();
    printAlarmSchedule();
}

// ==================== Time Checking & Triggering ====================

bool checkAlarmsAndTrigger() {
    if (!globalAlarmEnabled || alarmCount == 0) {
        return false;
    }
    
    // Get current time
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return false;  // Time not synchronized
    }
    
    int currentHour = timeinfo.tm_hour;
    int currentMinute = timeinfo.tm_min;
    int currentWeekday = timeinfo.tm_wday;  // 0=Sunday, 1=Monday, ... 6=Saturday
    int currentDay = timeinfo.tm_mday;
    
    // Convert Sunday from 0 to 7 for consistency with backend (1=Mon, 7=Sun)
    int dayNumber = (currentWeekday == 0) ? 7 : currentWeekday;
    
    // Prevent re-triggering in the same minute
    if (currentHour == lastTriggerHour && 
        currentMinute == lastTriggerMinute && 
        currentDay == lastTriggerDay) {
        return false;
    }
    
    // Check each alarm
    for (int i = 0; i < alarmCount; i++) {
        if (!alarms[i].enabled) continue;
        
        // Parse alarm time (format: "HH:MM")
        int alarmHour, alarmMinute;
        if (sscanf(alarms[i].time.c_str(), "%d:%d", &alarmHour, &alarmMinute) != 2) {
            Serial.printf("⚠️ Invalid alarm time format: %s\n", alarms[i].time.c_str());
            continue;
        }
        
        // Check if current time matches alarm time
        if (currentHour != alarmHour || currentMinute != alarmMinute) {
            continue;
        }
        
        // Check if current day matches alarm schedule
        bool dayMatches = false;
        for (int d = 0; d < alarms[i].daysCount; d++) {
            if (alarms[i].days[d] == dayNumber) {
                dayMatches = true;
                break;
            }
        }
        
        if (dayMatches) {
            // Alarm triggered!
            Serial.printf("🔔 ALARM TRIGGERED! Alarm #%d at %s\n", i, alarms[i].time.c_str());
            
            // Remember this trigger
            lastTriggerHour = currentHour;
            lastTriggerMinute = currentMinute;
            lastTriggerDay = currentDay;
            
            // Start wake-up sequence
            startWakeUpSequence();
            
            return true;
        }
    }
    
    return false;
}

// ==================== Persistent Storage ====================

void saveAlarmsToFlash() {
    prefs.begin("alarms", false);
    prefs.putInt("count", alarmCount);
    prefs.putBool("enabled", globalAlarmEnabled);
    
    for (int i = 0; i < alarmCount; i++) {
        String prefix = String(i);
        prefs.putString(("time" + prefix).c_str(), alarms[i].time);
        prefs.putBool(("ena" + prefix).c_str(), alarms[i].enabled);
        prefs.putInt(("dcnt" + prefix).c_str(), alarms[i].daysCount);
        
        for (int d = 0; d < alarms[i].daysCount; d++) {
            prefs.putInt(("day" + prefix + "_" + String(d)).c_str(), alarms[i].days[d]);
        }
    }
    
    prefs.end();
    Serial.println("💾 Alarms saved to flash");
}

void loadAlarmsFromFlash() {
    prefs.begin("alarms", true);
    alarmCount = prefs.getInt("count", 0);
    globalAlarmEnabled = prefs.getBool("enabled", false);
    
    if (alarmCount == 0) {
        Serial.println("⚠️ No alarms in flash");
        prefs.end();
        return;
    }
    
    for (int i = 0; i < alarmCount; i++) {
        String prefix = String(i);
        alarms[i].time = prefs.getString(("time" + prefix).c_str(), "00:00");
        alarms[i].enabled = prefs.getBool(("ena" + prefix).c_str(), false);
        alarms[i].daysCount = prefs.getInt(("dcnt" + prefix).c_str(), 0);
        
        for (int d = 0; d < alarms[i].daysCount; d++) {
            alarms[i].days[d] = prefs.getInt(("day" + prefix + "_" + String(d)).c_str(), 0);
        }
    }
    
    prefs.end();
    Serial.println("📥 Alarms loaded from flash");
}

// ==================== Debugging ====================

void printAlarmSchedule() {
    Serial.println("========== ALARM SCHEDULE ==========");
    Serial.printf("Global Enabled: %s\n", globalAlarmEnabled ? "YES" : "NO");
    
    if (alarmCount == 0) {
        Serial.println("No alarms configured");
    } else {
        for (int i = 0; i < alarmCount; i++) {
            Serial.printf("Alarm #%d:\n", i);
            Serial.printf("  Time: %s\n", alarms[i].time.c_str());
            Serial.printf("  Enabled: %s\n", alarms[i].enabled ? "YES" : "NO");
            Serial.print("  Days: ");
            for (int d = 0; d < alarms[i].daysCount; d++) {
                Serial.print(alarms[i].days[d]);
                if (d < alarms[i].daysCount - 1) Serial.print(", ");
            }
            Serial.println();
        }
    }
    Serial.println("====================================");
}
