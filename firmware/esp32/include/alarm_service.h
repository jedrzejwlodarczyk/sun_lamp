/**
 * @file alarm_service.h
 * @brief Alarm domain service
 * 
 * Single Responsibility: Manages alarm schedule and triggers
 * - Stores alarm schedule in flash memory
 * - Checks current time against scheduled alarms
 * - Triggers wake-up sequence when alarm matches
 */

#ifndef ALARM_SERVICE_H
#define ALARM_SERVICE_H

#include <ArduinoJson.h>

/**
 * @brief Initialize alarm service and load schedule from flash
 */
void initializeAlarms();

/**
 * @brief Toggle global alarm enable/disable
 */
void toggleAlarmEnabled();

/**
 * @brief Check if any alarm is enabled
 * @return true if at least one alarm is enabled
 */
bool isAnyAlarmEnabled();

/**
 * @brief Load alarm schedule from Device Shadow delta
 * @param scheduleArray JSON array of alarm objects
 */
void updateAlarmSchedule(JsonArray scheduleArray);

/**
 * @brief Check if current time matches any enabled alarm
 * Should be called regularly (e.g., every minute)
 * @return true if alarm triggered, false otherwise
 */
bool checkAlarmsAndTrigger();

/**
 * @brief Print current alarm schedule to Serial
 * For debugging purposes
 */
void printAlarmSchedule();

/**
 * @brief Save current alarm schedule to flash memory
 */
void saveAlarmsToFlash();

/**
 * @brief Load alarm schedule from flash memory
 */
void loadAlarmsFromFlash();

#endif
