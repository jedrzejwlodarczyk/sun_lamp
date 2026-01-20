# ESP32 Sun Lamp Firmware

Production-ready firmware for ESP32-based smart alarm lamp with AWS IoT Core integration.

## Architecture

**Design Principles:**

- ✅ **SOLID** - Single responsibility, dependency inversion, interface segregation
- ✅ **KISS** - Simple, backend-controlled thin client
- ✅ **DDD** - Clear domain separation (Hardware, Domain, Infrastructure)
- ✅ **Clean Architecture** - Layered design with proper boundaries

## Layer Structure

```
┌─────────────────────────────────────────┐
│         Application Layer               │
│         (main.cpp)                      │
├─────────────────────────────────────────┤
│         Domain Services                 │
│  - lamp_service (lamp state logic)     │
│  - alarm_service (schedule & triggers)  │
├─────────────────────────────────────────┤
│      Infrastructure Services            │
│  - aws_service (Device Shadow sync)    │
│  - wifi_service (network connection)    │
│  - ntp_service (time synchronization)   │
├─────────────────────────────────────────┤
│      Hardware Abstraction Layer         │
│  - hardware_service (GPIO/PWM/LED)     │
└─────────────────────────────────────────┘
```

## Features

### ✅ Implemented

- **AWS IoT Device Shadow** integration with proper delta handling
- **Hardware control** via PWM (warm/cool white LEDs) and GPIO (boost mode)
- **Capacitive touch buttons** with 50ms debouncing
- **WS2812B status LEDs** for connection and alarm indicators
- **Alarm system** with persistent flash storage and time-based triggering
- **Wake-up sequence** with gradual brightness increase (~30 minutes)
- **NTP time sync** for accurate alarm triggering
- **Offline resilience** - alarms work even without cloud connection
- **Health reporting** - RSSI, uptime, free heap metrics

### Hardware Pin Configuration

| Pin | Function | Details |
|-----|----------|---------|
| GPIO34 | Button 1 | Lamp toggle (AT42QT1070, active LOW) |
| GPIO35 | Button 2 | Alarm toggle (AT42QT1070, active LOW) |
| GPIO4 | Warm White PWM | 1kHz, 8-bit (0-255) |
| GPIO0 | Cool White PWM | 1kHz, 8-bit (0-255) |
| GPIO21 | Boost Mode | Digital HIGH/LOW |
| GPIO25 | Status LEDs | 2x WS2812B (connection + alarm) |

### AWS IoT Device Shadow Structure

**Reported State:**

```json
{
  "state": {
    "reported": {
      "lamp": { "on": true, "ww": 180, "cw": 120, "boost": false },
      "alarm": { "enabled": true },
      "health": { "rssi": -45, "uptime": 86400, "freeHeap": 120000 }
    }
  }
}
```

**Desired State (from cloud):**

```json
{
  "state": {
    "desired": {
      "lamp": { "on": true, "ww": 200, "cw": 150, "boost": true },
      "alarm": {
        "enabled": true,
        "schedule": [
          { "time": "07:00", "days": [1,2,3,4,5], "enabled": true }
        ]
      }
    }
  }
}
```

## Quick Start

### 1. Prerequisites

- Arduino IDE 2.x or PlatformIO
- ESP32 board support installed
- AWS IoT Thing provisioned (see [Setup Guide](../../docs/firmware-setup-guide.md))

### 2. Configure Secrets

Copy `include/secrets.h.example` to `include/secrets.h` and fill in:

- AWS IoT Thing name
- WiFi credentials
- AWS IoT endpoint
- X.509 certificates

### 3. Compile and Upload

**Arduino IDE:**

```
File > Open > firmware/esp32/src/main.cpp
Verify and Upload
```

**PlatformIO:**

```bash
cd firmware/esp32
pio run --target upload --target monitor
```

## File Structure

```
firmware/esp32/
├── platformio.ini                    # Build configuration
├── include/
│   ├── secrets.h                    # ⚠️ Credentials (git-ignored)
│   ├── hardware_config.h            # Pin definitions
│   ├── hardware_service.h           # HAL interface
│   ├── lamp_service.h               # Lamp domain
│   ├── alarm_service.h              # Alarm domain
│   ├── aws_service.h                # AWS IoT interface
│   ├── wifi_service.h               # WiFi interface
│   └── ntp_service.h                # Time sync interface
└── src/
    ├── main.cpp                     # Application entry point
    └── services/
        ├── hardware_service.cpp     # GPIO/PWM/LED control
        ├── lamp_service.cpp         # Lamp state management
        ├── alarm_service.cpp        # Alarm logic & persistence
        ├── aws_service.cpp          # Device Shadow sync
        ├── wifi_service.cpp         # Network connection
        └── ntp_service.cpp          # Time synchronization
```

## Dependencies

Automatically managed by PlatformIO/Arduino:

- **WiFi** (built-in ESP32)
- **ArduinoJson** v7.0.4+ - JSON parsing
- **MQTT** v2.5.2+ - AWS IoT communication
- **FastLED** v3.6.0+ - WS2812B control

## Main Loop Flow

```cpp
void loop() {
    handleButtonInputs();        // 50ms debounced buttons
    checkAlarmsAndTrigger();     // Every 60 seconds
    updateWakeUpSequence();      // Progressive brightness
    updateStatusIndicators();    // LED blink patterns
    publishReportedState();      // Every 5 minutes
    maintainNTPSync();           // Hourly re-sync
    maintainAWSConnection();     // MQTT keep-alive
}
```

## Service Responsibilities

### Hardware Service

- **Single Responsibility:** Low-level hardware control
- Configures PWM channels, GPIO pins
- Provides button reading with debouncing
- Controls WS2812B status LEDs

### Lamp Service

- **Single Responsibility:** Lamp domain logic
- Manages lamp on/off state
- Stores brightness values (ww, cw, boost)
- Implements wake-up sequence animation

### Alarm Service

- **Single Responsibility:** Alarm scheduling
- Persists schedule to flash (survives reboot)
- Checks current time vs. alarm schedule
- Triggers wake-up when alarm matches

### AWS Service

- **Single Responsibility:** Cloud synchronization
- Connects to AWS IoT Core
- Subscribes to Device Shadow delta/accepted/rejected
- Routes shadow updates to domain services
- Publishes reported state

### WiFi Service

- **Single Responsibility:** Network connectivity
- Connects to WiFi with timeout
- Provides connection status

### NTP Service

- **Single Responsibility:** Time synchronization
- Syncs time from NTP servers
- Maintains accurate clock for alarms

## Serial Monitor Output

Successful boot:

```
========================================
   ESP32 Sun Lamp - Starting Up
========================================
🔧 Initializing hardware...
✅ Hardware initialized
💡 Lamp service initialized
⏰ Alarm service initialized
📡 Connecting to WiFi...
✅ WiFi connected
🕒 Initializing NTP...
✅ NTP synchronized
🔐 Configuring AWS IoT certificates...
✅ Connected to AWS IoT Core
========================================
   ✅ Initialization Complete
========================================
```

## Testing

### Manual Testing

1. Press Button 1 → Lamp toggles on/off
2. Press Button 2 → Alarm enabled/disabled
3. Update Device Shadow → Lamp responds
4. Set alarm → Wake-up sequence triggers at scheduled time

### Status LED Indicators

**LED 0 (Connection):**

- 🔴 Red: No WiFi
- 🔵 Blue: WiFi connected, no AWS
- 🟢 Green: AWS IoT connected

**LED 1 (Alarm):**

- ⚫ Off: Alarm disabled
- 🟡 Yellow (blinking): Alarm enabled

## Troubleshooting

See detailed troubleshooting guide in [`docs/firmware-setup-guide.md`](../../docs/firmware-setup-guide.md)

**Common Issues:**

- WiFi not connecting → Check SSID/password
- AWS connection fails → Verify certificates and endpoint
- Alarms not triggering → Check NTP sync and timezone
- Buttons not responding → Verify AT42QT1070 wiring

## Performance

**Memory Usage:**

- RAM: ~14% (45KB / 320KB)
- Flash: ~30% (386KB / 1.3MB)

**Power Consumption:**

- Idle: ~80mA @ 3.3V
- WiFi active: ~120mA @ 3.3V
- LEDs at full brightness: +500mA @ 12V

## Security

- ✅ X.509 mutual TLS authentication
- ✅ Certificates stored in PROGMEM (flash)
- ✅ `secrets.h` excluded from version control
- ✅ AWS IoT policy limits device permissions

## Future Enhancements

- [ ] OTA firmware updates via AWS IoT Jobs
- [ ] Deep sleep mode for battery operation
- [ ] Web-based WiFi provisioning (captive portal)
- [ ] Multiple alarm slots (currently limited to 10)
- [ ] Custom wake-up duration configuration

## Contributing

When modifying firmware:

1. Follow SOLID principles
2. Maintain layer separation
3. Add comments for hardware-specific code
4. Test thoroughly before committing
5. Update this README if adding features

## License

MIT License - See root LICENSE file

## Support

See full documentation: [`docs/firmware-setup-guide.md`](../../docs/firmware-setup-guide.md)
