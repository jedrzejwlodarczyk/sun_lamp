/**
 * @file ntp_service.h
 * @brief NTP time synchronization service
 * 
 * Single Responsibility: Manages time synchronization with NTP servers
 */

#ifndef NTP_SERVICE_H
#define NTP_SERVICE_H

/**
 * @brief Initialize NTP and sync time
 * Configures timezone and connects to NTP server
 */
void initializeNTP();

/**
 * @brief Maintain NTP sync (call in loop)
 * Periodically re-syncs time if connection lost
 */
void maintainNTPSync();

/**
 * @brief Print current local time to Serial
 * For debugging purposes
 */
void printCurrentTime();

#endif
