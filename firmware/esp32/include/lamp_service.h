/**
 * @file lamp_service.h
 * @brief Lamp domain service
 * 
 * Single Responsibility: Manages lamp state and applies hardware changes
 * Domain logic for lamp brightness, boost mode, and state transitions
 */

#ifndef LAMP_SERVICE_H
#define LAMP_SERVICE_H

#include <cstdint>

/**
 * @brief Initialize lamp service with default state
 */
void initializeLamp();

/**
 * @brief Toggle lamp on/off state
 */
void toggleLamp();

/**
 * @brief Get current lamp on/off state
 * @return true if lamp is on, false otherwise
 */
bool isLampOn();

/**
 * @brief Get warm white brightness value
 * @return Brightness 0-255
 */
uint8_t getWarmWhite();

/**
 * @brief Get cool white brightness value
 * @return Brightness 0-255
 */
uint8_t getCoolWhite();

/**
 * @brief Get boost mode state
 * @return true if boost enabled, false otherwise
 */
bool isBoostEnabled();

/**
 * @brief Set lamp state from cloud (Device Shadow desired state)
 * @param on Lamp on/off state
 * @param ww Warm white brightness 0-255
 * @param cw Cool white brightness 0-255
 * @param boost Boost mode enabled
 */
void setLampState(bool on, uint8_t ww, uint8_t cw, bool boost);

/**
 * @brief Apply current lamp state to hardware
 * Updates PWM outputs and boost GPIO based on state
 */
void applyLampState();

/**
 * @brief Start gradual wake-up animation for alarm
 * Gradually increases brightness over ~30 minutes
 */
void startWakeUpSequence();

/**
 * @brief Update wake-up sequence (call in loop)
 * @return true if sequence is running, false if complete
 */
bool updateWakeUpSequence();

/**
 * @brief Stop wake-up sequence immediately
 */
void stopWakeUpSequence();

#endif
