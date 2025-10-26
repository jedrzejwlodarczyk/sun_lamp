# Lamp Alarm Project - Hardware Specification

## Overview

ESP32-based smart lamp controller with dual LED strips, physical button controls, and status indicators. Hardware designed for both cloud-connected and offline operation.

---

## Core Components

### Microcontroller

**ESP32 Development Board**

- Built-in WiFi connectivity
- Sufficient GPIO pins for all required interfaces
- PWM capability for LED brightness control
- Flash memory for alarm schedule persistence
- Processing power for AWS IoT communication

---

## Input Interfaces

### Capacitive Button Controller

**Configuration:**

- 2 capacitive touch inputs
- Binary response (pressed/not pressed)
- Controller handles capacitive sensing

**Button Functions:**

- **Button 1:** Lamp on/off toggle
- **Button 2:** Alarm enable/disable toggle (global switch)

**Testing Substitute:**

- Simple push buttons for firmware development
- Same binary logic applies
- Final hardware uses capacitive controller

---

## Output Interfaces

### LED Strip Control - Dual Channel PWM

**Warm White (WW) Channel:**

- 1 PWM pin
- 0-255 brightness control
- Independent from Cool White

**Cool White (CW) Channel:**

- 1 PWM pin
- 0-255 brightness control
- Independent from Warm White

**Total:** 2 PWM outputs

### Boost Mode - Secondary Strip Control

**Configuration:**

- 2 binary (on/off) transistor controls
- Enables secondary LED strip for brightness boost
- Digital output pins (not PWM)

**Function:**

- Activate additional LED strip when boost mode enabled
- Binary control: both transistors on/off together
- Provides additional brightness beyond base PWM strips

**Total:** 2 digital outputs for transistors

### Status Indicators

**LED 1 - System Status:**

- Connection state (WiFi/AWS IoT)
- Solid: Connected
- Blinking: Connecting/syncing
- Fast blink: Error state

**LED 2 - Alarm Status:**

- Solid: Alarm enabled
- Off: Alarm disabled

**Total:** 2 digital outputs for indicator LEDs

---

## GPIO Pin Summary

### Required Pins

- **2 GPIO** - Capacitive button inputs
- **2 PWM** - Warm White and Cool White LED control
- **2 Digital** - Boost transistor control
- **2 Digital** - Status indicator LEDs

**Total: 8 GPIO pins required**

ESP32 provides 34+ usable GPIO pins, leaving significant room for future expansion.

---

## Power Requirements

**Considerations:**

- Constant power required (USB or wall adapter)
- Device must maintain time and alarm schedule
- No battery backup (AC-powered device)

**Power Loss Behavior:**

- Alarm schedule persists in flash memory
- Device reconnects and syncs on power restoration
- NTP resync required after boot

---

## Testing Hardware

### Development Phase

**Provided to Firmware Developer:**

- ESP32 development board
- 2 transistors (PWM and boost tested separately)
- Simple push buttons (substitute for capacitive controller)
- LEDs for PWM output verification
- Standard lab equipment

**Limitations:**

- Only 2 transistors available during development
- Test PWM strips and boost mode separately
- Capacitive buttons substituted with simple buttons

### Final Assembly Phase

**Full Hardware:**

- Complete capacitive button controller
- All transistors for PWM and boost control
- Integrated LED strips
- Final enclosure and assembly

---

## Hardware Control Logic

### Lamp States

**Off State:**

- All PWM outputs: 0
- Boost transistors: OFF
- Lamp LED strips: Not illuminated

**On State - Normal Mode:**

- WW PWM: 0-255 (adjustable)
- CW PWM: 0-255 (adjustable)
- Boost transistors: OFF
- Base brightness via PWM control

**On State - Boost Mode:**

- WW PWM: 0-255 (adjustable)
- CW PWM: 0-255 (adjustable)
- Boost transistors: ON
- Maximum brightness with secondary strip

### Physical Button Behavior

**Button 1 (Lamp Toggle):**

- Press: Toggle lamp on/off
- Immediate local response
- State reported to cloud

**Button 2 (Alarm Toggle):**

- Press: Toggle alarm enabled/disabled
- Global switch for entire alarm system
- State reported to cloud

**Debouncing:**

- Required for reliable button detection
- Implemented in firmware

---

## Integration Points

### Firmware Interface

All hardware controlled through abstraction layer in ESP32 firmware:

- PWM driver for WW/CW control
- GPIO driver for transistors and buttons
- LED indicator driver for status lights

### Cloud Control

Hardware state changes commanded via AWS IoT Shadow:

- Lamp on/off and brightness levels
- Boost mode activation
- All changes confirmed back to cloud

---

## Testing Approach

### Component Testing

- **PWM Output:** Verify with LED and measurement equipment
- **Transistor Control:** Test binary on/off switching
- **Button Input:** Verify debouncing and state detection
- **Indicator LEDs:** Confirm status patterns

### Integration Testing

- **Firmware + Hardware:** All components working together
- **Physical Controls:** Button presses update device and cloud
- **Cloud Commands:** Shadow updates control hardware
- **Offline Operation:** Alarm triggers without internet

### Final Testing

- **Full Assembly:** Complete hardware with all components
- **Capacitive Buttons:** Real controller integration
- **All Transistors:** Complete PWM and boost system
- **End-to-End:** Physical device through cloud to user interface

---

## Design Considerations

### Modularity

- PWM and boost control can be tested independently
- Button interface easily swappable (simple vs capacitive)
- Status indicators separate from main control

### Reliability

- Flash persistence ensures alarm schedule survives power loss
- Offline operation maintains core alarm functionality
- Local time management via NTP

### Scalability

- Additional GPIO available for future features
- ESP32 has processing headroom
- Cloud architecture supports multiple devices

---

## Manufacturing Notes

### Development Phase

- Focus on firmware functionality
- Component-level testing acceptable
- Substitute components where needed

### Production Phase

- Full hardware integration required
- Final capacitive button controller
- Complete transistor set for PWM and boost
- Professional assembly and enclosure
