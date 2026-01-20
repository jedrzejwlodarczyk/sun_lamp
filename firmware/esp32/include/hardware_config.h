/**
 * @file hardware_config.h
 * @brief Hardware pin definitions and constants for ESP32 Sun Lamp
 * 
 * Hardware Configuration:
 * - GPIO34: Button 1 (Lamp toggle) - AT42QT1070, active LOW
 * - GPIO35: Button 2 (Alarm toggle) - AT42QT1070, active LOW
 * - GPIO4: PWM Warm White LED (1kHz, 8-bit)
 * - GPIO0: PWM Cool White LED (1kHz, 8-bit)
 * - GPIO21: Boost mode (binary HIGH/LOW)
 * - GPIO25: 2x WS2812B status LEDs
 */

#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

// Button Pins (AT42QT1070 capacitive touch controller)
#define PIN_BUTTON_LAMP    34  // Button 1: Lamp toggle (active LOW)
#define PIN_BUTTON_ALARM   35  // Button 2: Alarm toggle (active LOW)

// LED PWM Pins
#define PIN_WW_PWM         4   // Warm White LED PWM output
#define PIN_CW_PWM         0   // Cool White LED PWM output

// Boost Mode Pin
#define PIN_BOOST          21  // Binary HIGH/LOW for boost mode

// Status LED Pin (WS2812B)
#define PIN_STATUS_LED     25  // 2x WS2812B LEDs
#define NUM_STATUS_LEDS    2   // Total number of WS2812B LEDs

// PWM Configuration
#define PWM_FREQUENCY      1000  // 1kHz PWM frequency
#define PWM_RESOLUTION     8     // 8-bit resolution (0-255)
#define PWM_CHANNEL_WW     0     // LEDc channel for warm white
#define PWM_CHANNEL_CW     1     // LEDc channel for cool white

// Button Debounce
#define DEBOUNCE_DELAY_MS  50    // 50ms debounce delay

// WS2812B Status LED Indices
#define STATUS_LED_CONNECTION  0  // LED 0: Connection status
#define STATUS_LED_ALARM       1  // LED 1: Alarm status

// LED Colors (RGB)
#define COLOR_OFF          0x000000
#define COLOR_BLUE         0x0000FF  // WiFi connecting
#define COLOR_GREEN        0x00FF00  // Connected
#define COLOR_RED          0xFF0000  // Error/Disconnected
#define COLOR_YELLOW       0xFFFF00  // Alarm enabled
#define COLOR_PURPLE       0xFF00FF  // Alarm triggering

#endif
