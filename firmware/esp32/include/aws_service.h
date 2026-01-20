/**
 * @file aws_service.h
 * @brief AWS IoT Core Device Shadow integration service
 * 
 * Single Responsibility: Manages AWS IoT connection and Device Shadow sync
 * - Connects to AWS IoT Core using X.509 certificates
 * - Subscribes to Device Shadow delta/accepted/rejected topics
 * - Publishes reported state updates
 * - Routes shadow messages to appropriate handlers
 */

#ifndef AWS_SERVICE_H
#define AWS_SERVICE_H

/**
 * @brief Initialize and connect to AWS IoT Core
 * Sets up TLS connection, subscribes to shadow topics
 */
void initializeAWS();

/**
 * @brief Check if connected to AWS IoT
 * @return true if connected, false otherwise
 */
bool isAWSConnected();

/**
 * @brief Publish current device state to Device Shadow
 * Reports lamp state, alarm status, and health metrics
 */
void publishReportedState();

/**
 * @brief Request current Device Shadow from AWS
 * Triggers shadow/get which responds with current desired state
 */
void requestDeviceShadow();

/**
 * @brief Maintain AWS connection (call in loop)
 * Handles reconnection and MQTT keep-alive
 */
void maintainAWSConnection();

#endif
