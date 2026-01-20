/**
 * @file lamp_service.cpp
 * @brief Lamp domain service implementation
 * 
 * Manages lamp state and coordinates with hardware layer
 * Implements wake-up sequence for alarm functionality
 */

#include "lamp_service.h"
#include "hardware_service.h"
#include <Arduino.h>

// ==================== Lamp State ====================
struct LampState {
    bool on;
    uint8_t warmWhite;
    uint8_t coolWhite;
    bool boost;
};

static LampState currentState = {false, 0, 0, false};

// ==================== Wake-Up Sequence State ====================
struct WakeUpState {
    bool active;
    unsigned long startTime;
    unsigned long lastUpdate;
    uint8_t currentBrightness;
};

static WakeUpState wakeUpState = {false, 0, 0, 0};

// Constants for wake-up sequence (~30 minutes)
const uint8_t WAKE_UP_MAX_BRIGHTNESS = 255;
const uint8_t WAKE_UP_STEP = 5;              // Brightness increment per step
const unsigned long WAKE_UP_STEP_DELAY = 7000;  // 7 seconds per step
const uint8_t WAKE_UP_TOTAL_STEPS = WAKE_UP_MAX_BRIGHTNESS / WAKE_UP_STEP;  // 51 steps
// Total time: 51 steps * 7 sec = 357 seconds = ~6 minutes
// Adjust WAKE_UP_STEP_DELAY to 35000 for true 30-minute wake-up

// ==================== Initialization ====================

void initializeLamp() {
    currentState.on = false;
    currentState.warmWhite = 0;
    currentState.coolWhite = 0;
    currentState.boost = false;
    applyLampState();
    Serial.println("💡 Lamp service initialized");
}

// ==================== State Getters ====================

bool isLampOn() {
    return currentState.on;
}

uint8_t getWarmWhite() {
    return currentState.warmWhite;
}

uint8_t getCoolWhite() {
    return currentState.coolWhite;
}

bool isBoostEnabled() {
    return currentState.boost;
}

// ==================== State Setters ====================

void toggleLamp() {
    currentState.on = !currentState.on;
    
    // If turning off, set brightness to 0
    // If turning on, restore to default or last brightness
    if (!currentState.on) {
        currentState.warmWhite = 0;
        currentState.coolWhite = 0;
        currentState.boost = false;
    } else {
        // Set to default warm brightness if no previous value
        if (currentState.warmWhite == 0 && currentState.coolWhite == 0) {
            currentState.warmWhite = 180;
            currentState.coolWhite = 120;
        }
    }
    
    applyLampState();
    Serial.printf("💡 Lamp toggled: %s\n", currentState.on ? "ON" : "OFF");
}

void setLampState(bool on, uint8_t ww, uint8_t cw, bool boost) {
    currentState.on = on;
    currentState.warmWhite = ww;
    currentState.coolWhite = cw;
    currentState.boost = boost;
    
    applyLampState();
    Serial.printf("💡 Lamp state updated: on=%d, ww=%d, cw=%d, boost=%d\n", 
                  on, ww, cw, boost);
}

void applyLampState() {
    if (currentState.on) {
        setWarmWhiteBrightness(currentState.warmWhite);
        setCoolWhiteBrightness(currentState.coolWhite);
        setBoostMode(currentState.boost);
    } else {
        setWarmWhiteBrightness(0);
        setCoolWhiteBrightness(0);
        setBoostMode(false);
    }
}

// ==================== Wake-Up Sequence ====================

void startWakeUpSequence() {
    wakeUpState.active = true;
    wakeUpState.startTime = millis();
    wakeUpState.lastUpdate = millis();
    wakeUpState.currentBrightness = 0;
    
    // Turn lamp on and start from 0 brightness
    currentState.on = true;
    currentState.warmWhite = 0;
    currentState.coolWhite = 0;
    currentState.boost = false;
    applyLampState();
    
    Serial.println("🌅 Wake-up sequence started");
}

bool updateWakeUpSequence() {
    if (!wakeUpState.active) {
        return false;
    }
    
    unsigned long now = millis();
    
    // Check if it's time for the next step
    if (now - wakeUpState.lastUpdate >= WAKE_UP_STEP_DELAY) {
        wakeUpState.lastUpdate = now;
        
        // Increment brightness
        if (wakeUpState.currentBrightness < WAKE_UP_MAX_BRIGHTNESS) {
            wakeUpState.currentBrightness += WAKE_UP_STEP;
            if (wakeUpState.currentBrightness > WAKE_UP_MAX_BRIGHTNESS) {
                wakeUpState.currentBrightness = WAKE_UP_MAX_BRIGHTNESS;
            }
            
            // Apply equal brightness to both warm and cool white
            currentState.warmWhite = wakeUpState.currentBrightness;
            currentState.coolWhite = wakeUpState.currentBrightness;
            applyLampState();
            
            Serial.printf("🌅 Wake-up brightness: %d/%d\n", 
                         wakeUpState.currentBrightness, WAKE_UP_MAX_BRIGHTNESS);
        } else {
            // Sequence complete - activate boost mode
            currentState.boost = true;
            applyLampState();
            wakeUpState.active = false;
            
            unsigned long totalTime = (now - wakeUpState.startTime) / 1000;
            Serial.printf("🌅 Wake-up sequence complete! Total time: %lu seconds\n", totalTime);
            return false;
        }
    }
    
    return true;
}

void stopWakeUpSequence() {
    if (wakeUpState.active) {
        wakeUpState.active = false;
        Serial.println("🌅 Wake-up sequence stopped");
    }
}
