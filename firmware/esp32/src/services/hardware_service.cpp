/**
 * @file hardware_service.cpp
 * @brief Hardware Abstraction Layer implementation
 * 
 * Implements all low-level hardware control following Single Responsibility Principle
 */

#include "hardware_service.h"
#include "hardware_config.h"
#include <Arduino.h>
#include <FastLED.h>

// ==================== WS2812B Status LEDs ====================
CRGB statusLEDs[NUM_STATUS_LEDS];

// ==================== Button Debouncing State ====================
struct ButtonState {
    bool lastState;
    unsigned long lastDebounceTime;
    bool currentState;
};

static ButtonState button1State = {HIGH, 0, HIGH};
static ButtonState button2State = {HIGH, 0, HIGH};

// ==================== Initialization ====================

void setupHardware() {
    Serial.println("🔧 Initializing hardware...");
    
    // Configure PWM for LED control
    ledcSetup(PWM_CHANNEL_WW, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_CW, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(PIN_WW_PWM, PWM_CHANNEL_WW);
    ledcAttachPin(PIN_CW_PWM, PWM_CHANNEL_CW);
    
    // Configure boost mode pin
    pinMode(PIN_BOOST, OUTPUT);
    digitalWrite(PIN_BOOST, LOW);
    
    // Configure button pins (active LOW with internal pull-up)
    pinMode(PIN_BUTTON_LAMP, INPUT_PULLUP);
    pinMode(PIN_BUTTON_ALARM, INPUT_PULLUP);
    
    // Initialize WS2812B status LEDs
    FastLED.addLeds<WS2812B, PIN_STATUS_LED, GRB>(statusLEDs, NUM_STATUS_LEDS);
    FastLED.setBrightness(50);  // 20% brightness to avoid blinding
    statusLEDs[STATUS_LED_CONNECTION] = CRGB::Black;
    statusLEDs[STATUS_LED_ALARM] = CRGB::Black;
    FastLED.show();
    
    Serial.println("✅ Hardware initialized");
}

// ==================== PWM LED Control ====================

void setWarmWhiteBrightness(uint8_t brightness) {
    ledcWrite(PWM_CHANNEL_WW, brightness);
}

void setCoolWhiteBrightness(uint8_t brightness) {
    ledcWrite(PWM_CHANNEL_CW, brightness);
}

void setBoostMode(bool enabled) {
    digitalWrite(PIN_BOOST, enabled ? HIGH : LOW);
}

// ==================== Button Input with Debouncing ====================

static bool readDebouncedButton(int pin, ButtonState& state) {
    bool reading = digitalRead(pin);
    
    // Active LOW: button pressed = LOW, not pressed = HIGH
    if (reading != state.lastState) {
        state.lastDebounceTime = millis();
    }
    
    if ((millis() - state.lastDebounceTime) > DEBOUNCE_DELAY_MS) {
        if (reading != state.currentState) {
            state.currentState = reading;
            
            // Return true only on falling edge (button press)
            if (state.currentState == LOW) {
                state.lastState = reading;
                return true;
            }
        }
    }
    
    state.lastState = reading;
    return false;
}

bool readButton1() {
    return readDebouncedButton(PIN_BUTTON_LAMP, button1State);
}

bool readButton2() {
    return readDebouncedButton(PIN_BUTTON_ALARM, button2State);
}

// ==================== WS2812B Status LED Control ====================

void setConnectionLED(uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    statusLEDs[STATUS_LED_CONNECTION] = CRGB(r, g, b);
}

void setAlarmLED(uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    statusLEDs[STATUS_LED_ALARM] = CRGB(r, g, b);
}

void updateStatusLEDs() {
    FastLED.show();
}

void turnOffAllLEDs() {
    setWarmWhiteBrightness(0);
    setCoolWhiteBrightness(0);
    setBoostMode(false);
    setConnectionLED(COLOR_OFF);
    setAlarmLED(COLOR_OFF);
    updateStatusLEDs();
}
