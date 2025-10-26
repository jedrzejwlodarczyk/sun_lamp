# Lamp Alarm Project - General Overview

## Project Summary

Smart lamp with programmable alarm functionality using ESP32 microcontroller and AWS IoT Core serverless architecture.

## Architecture

**Device:** ESP32 with WiFi connectivity
**Cloud:** AWS IoT Core with Device Shadow (serverless)
**Communication:** Two-way sync between physical device and cloud
**Approach:** Hybrid - device maintains local schedule, cloud stores master configuration

## Key Features

- **Physical Controls:** Two capacitive buttons for lamp and alarm control
- **Dual LED Strips:** Warm White and Cool White with PWM control
- **Boost Mode:** Secondary strip activation via binary transistors
- **Status Indicators:** Two LEDs for system and alarm status
- **Alarm System:** Persistent scheduling stored in flash memory
- **Offline Operation:** Device continues functioning without internet connection
- **Health Monitoring:** WiFi signal, uptime, and system health reporting

## Components

### Hardware

- ESP32 development board
- Capacitive button controller
- Dual LED strips (WW/CW with PWM)
- Boost transistors for secondary strip
- Status indicator LEDs

### Software

- ESP32 firmware (delegated development)
- AWS IoT Core with Device Shadow
- Lambda functions for user interface (future scope)
- NTP time synchronization
- Flash memory persistence

## Project Phases

**Phase 1 (Current):** ESP32 firmware development and testing
**Phase 2 (Future):** User interface implementation on Lambda
**Phase 3 (Future):** Hardware integration and final assembly

## Division of Work

- **Hardware Development:** Primary responsibility
- **ESP32 Firmware:** Delegated to team member
- **AWS Backend:** Primary responsibility
- **User Interface:** Future work (out of current scope)
