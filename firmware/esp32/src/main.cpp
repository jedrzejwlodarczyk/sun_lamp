/**
 * @file main.cpp
 * @brief ESP32 Sun Lamp - Main Application
 * 
 * Smart alarm lamp with AWS IoT Core integration
 * 
 * Architecture:
 * - Thin client pattern: minimal logic, backend-controlled
 * - SOLID principles: service-oriented, single responsibility
 * - Clean separation: Hardware, Domain, Infrastructure layers
 * 
 * Hardware:
 * - ESP32 DevKit
 * - 2x WS2812B status LEDs (GPIO25)
 * - 2x Capacitive touch buttons via AT42QT1070 (GPIO34, GPIO35)
 * - Warm/Cool White LEDs with PWM (GPIO4, GPIO0)
 * - Boost mode output (GPIO21)
 */

#include <Arduino.h>
#include "hardware_config.h"
#include "hardware_service.h"
#include "wifi_service.h"
#include "ntp_service.h"
#include "aws_service.h"
#include "lamp_service.h"
#include "alarm_service.h"

// ==================== Timing Control ====================
unsigned long lastAlarmCheck = 0;
unsigned long lastStatusUpdate = 0;
unsigned long lastStateReport = 0;

const unsigned long ALARM_CHECK_INTERVAL = 60000;      // Check alarms every minute
const unsigned long STATUS_UPDATE_INTERVAL = 1000;     // Update status LEDs every second
const unsigned long STATE_REPORT_INTERVAL = 300000;    // Report state every 5 minutes

// ==================== Setup ====================

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n");
    Serial.println("========================================");
    Serial.println("   ESP32 Sun Lamp - Starting Up");
    Serial.println("========================================");
    
    // 1. Initialize Hardware Layer
    setupHardware();
    
    // 2. Initialize Domain Services
    initializeLamp();
    initializeAlarms();
    
    // 3. Connect to Infrastructure
    connectToWiFi();
    
    if (isWiFiConnected()) {
        initializeNTP();
        initializeAWS();
    } else {
        Serial.println("❌ WiFi connection failed - continuing offline");
        setConnectionLED(COLOR_RED);
        updateStatusLEDs();
    }
    
    Serial.println("========================================");
    Serial.println("   ✅ Initialization Complete");
    Serial.println("========================================\n");
    
    // Initial state report
    if (isAWSConnected()) {
        publishReportedState();
    }
}

// ==================== Main Loop ====================

void loop() {
    unsigned long now = millis();
    
    // ========== Button Input Handling ==========
    handleButtonInputs();
    
    // ========== Alarm Checking ==========
    if (now - lastAlarmCheck >= ALARM_CHECK_INTERVAL) {
        lastAlarmCheck = now;
        checkAlarmsAndTrigger();
    }
    
    // ========== Wake-Up Sequence Update ==========
    updateWakeUpSequence();
    
    // ========== Status LED Updates ==========
    if (now - lastStatusUpdate >= STATUS_UPDATE_INTERVAL) {
        lastStatusUpdate = now;
        updateStatusIndicators();
    }
    
    // ========== Periodic State Reporting ==========
    if (now - lastStateReport >= STATE_REPORT_INTERVAL) {
        lastStateReport = now;
        if (isAWSConnected()) {
            publishReportedState();
        }
    }
    
    // ========== Infrastructure Maintenance ==========
    maintainNTPSync();
    maintainAWSConnection();
    
    // Small delay to prevent watchdog issues
    delay(10);
}

// ==================== Button Handler ====================

void handleButtonInputs() {
    // Button 1: Lamp toggle
    if (readButton1()) {
        Serial.println("🔘 Button 1 pressed: Toggling lamp");
        toggleLamp();
        
        // Stop wake-up sequence if running
        stopWakeUpSequence();
        
        // Report state change to cloud
        if (isAWSConnected()) {
            publishReportedState();
        }
    }
    
    // Button 2: Alarm toggle
    if (readButton2()) {
        Serial.println("🔘 Button 2 pressed: Toggling alarm");
        toggleAlarmEnabled();
        
        // Update alarm status LED
        updateStatusIndicators();
        
        // Report state change to cloud
        if (isAWSConnected()) {
            publishReportedState();
        }
    }
}

// ==================== Status LED Updates ====================

void updateStatusIndicators() {
    // Connection status LED (LED 0)
    if (isAWSConnected()) {
        setConnectionLED(COLOR_GREEN);
    } else if (isWiFiConnected()) {
        setConnectionLED(COLOR_BLUE);
    } else {
        setConnectionLED(COLOR_RED);
    }
    
    // Alarm status LED (LED 1)
    if (isAnyAlarmEnabled()) {
        // Blink yellow when alarm is enabled
        static bool alarmLEDState = false;
        alarmLEDState = !alarmLEDState;
        setAlarmLED(alarmLEDState ? COLOR_YELLOW : COLOR_OFF);
    } else {
        setAlarmLED(COLOR_OFF);
    }
    
    updateStatusLEDs();
}
