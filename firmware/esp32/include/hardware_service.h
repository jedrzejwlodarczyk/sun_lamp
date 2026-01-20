/**
 * @file hardware_service.h
 * @brief Hardware Abstraction Layer for ESP32 Sun Lamp
 * 
 * Single Responsibility: Manages all low-level hardware interactions
 * - PWM LED control (warm white, cool white)
 * - Digital output (boost mode)
 * - Button input reading
 * - WS2812B status LED control
 */

#ifndef HARDWARE_SERVICE_H
#define HARDWARE_SERVICE_H

#include <cstdint>

/**
 * @brief Initialize all hardware peripherals
 * Configures PWM channels, GPIO pins, and status LEDs
 */
void setupHardware();

/**
 * @brief Set warm white LED brightness
 * @param brightness PWM value 0-255 (0=off, 255=max)
 */
void setWarmWhiteBrightness(uint8_t brightness);

/**
 * @brief Set cool white LED brightness
 * @param brightness PWM value 0-255 (0=off, 255=max)
 */
void setCoolWhiteBrightness(uint8_t brightness);

/**
 * @brief Set boost mode state
 * @param enabled true=boost ON, false=boost OFF
 */
void setBoostMode(bool enabled);

/**
 * @brief Read button 1 state (lamp toggle) with debouncing
 * @return true if button pressed, false otherwise
 */
bool readButton1();

/**
 * @brief Read button 2 state (alarm toggle) with debouncing
 * @return true if button pressed, false otherwise
 */
bool readButton2();

/**
 * @brief Set connection status LED color
 * @param color RGB color (0xRRGGBB format)
 */
void setConnectionLED(uint32_t color);

/**
 * @brief Set alarm status LED color
 * @param color RGB color (0xRRGGBB format)
 */
void setAlarmLED(uint32_t color);

/**
 * @brief Update status LEDs to display current colors
 * Must be called after setConnectionLED() or setAlarmLED()
 */
void updateStatusLEDs();

/**
 * @brief Turn off all LEDs (lamp and status)
 */
void turnOffAllLEDs();

#endif
