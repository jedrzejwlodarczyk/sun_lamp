/**
 * @file wifi_service.h
 * @brief WiFi connection management service
 * 
 * Single Responsibility: Manages WiFi connection
 */

#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

/**
 * @brief Connect to WiFi network
 * Uses credentials from secrets.h
 */
void connectToWiFi();

/**
 * @brief Check if connected to WiFi
 * @return true if connected, false otherwise
 */
bool isWiFiConnected();

/**
 * @brief Get WiFi signal strength (RSSI)
 * @return RSSI value in dBm
 */
int getWiFiRSSI();

#endif
