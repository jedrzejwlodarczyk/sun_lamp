# Lamp Alarm Project - Software Architecture

## Overview

Serverless architecture using AWS IoT Core for device communication and Lambda for user interface. ESP32 firmware acts as thin client executing commands and reporting state.

---

## Architecture Pattern

**Thin Client Model:**

- ESP32 executes hardware commands
- AWS handles business logic and decision-making
- Two-way state synchronization via Device Shadow
- Hybrid operation: local + cloud coordination

---

## AWS IoT Core

### Device Shadow

Primary mechanism for state management and communication between device and cloud.

**Shadow Structure:**

- **Desired State:** Commands from cloud to device
- **Reported State:** Current device status sent to cloud
- **Delta:** Differences between desired and reported states

**Communication Topics:**

- `$aws/things/{thingName}/shadow/update` - Device publishes state
- `$aws/things/{thingName}/shadow/update/delta` - Device receives commands
- `$aws/things/{thingName}/shadow/update/accepted` - Confirmation messages
- `$aws/things/{thingName}/shadow/get` - Retrieve current shadow

### Authentication

- X.509 certificates for device authentication
- Certificate-based secure connection
- Per-device credentials

---

## State Management

### Reported State (Device → Cloud)

Device publishes current hardware state:

- Lamp status (on/off, brightness levels, boost mode)
- Alarm status (enabled/disabled)
- Health metrics (WiFi RSSI, uptime, free memory)
- Error conditions and status codes

### Desired State (Cloud → Device)

Cloud sends commands:

- Lamp control (on/off, WW/CW levels, boost activation)
- Alarm schedule updates
- Configuration changes

### Error Reporting

Errors reported via Shadow reported state with status field:

- Success: `"status": "ok"`
- Errors: `"status": "hw_fault"`, `"error": "description"`
- AWS handles business logic based on error states

---

## ESP32 Firmware Architecture

### Responsibilities

- Connect to WiFi and AWS IoT Core
- Subscribe to Shadow delta messages
- Execute hardware commands received from cloud
- Publish hardware state changes to Shadow
- Handle physical button inputs
- Manage alarm timing and triggering
- Persist alarm schedule in flash memory
- Maintain offline operation capability

### Design Principles

- **SOLID:** Especially Single Responsibility Principle
- **KISS:** Simple, minimal device logic
- **DDD:** Clear domain separation (hardware, connectivity, state)
- **Layered Architecture:** HAL, services, controllers

### Key Components

- Hardware Abstraction Layer (GPIO, PWM control)
- Shadow synchronization controller
- Button input handler
- Alarm timing service
- Flash memory persistence
- WiFi and IoT connectivity management

---

## Hybrid Operation Model

### Cloud-Connected Mode

1. Device receives alarm schedule from cloud via Shadow
2. Stores schedule in local flash memory
3. Reports state changes to cloud
4. Cloud can update schedule at any time

### Offline Mode

1. Device continues with locally stored alarm schedule
2. Alarm triggers based on local time (NTP synced)
3. Physical buttons continue to work
4. On reconnection, syncs state with cloud Shadow

---

## Time Management

### NTP Synchronization

- Device syncs time on boot via NTP
- Maintains accurate local time for alarm triggering
- Required for offline alarm functionality

### Alarm Scheduling

- Alarm schedule stored in flash memory (persistent)
- Device checks current time against schedule periodically
- Triggers lamp routine when time matches
- Schedule format includes time and day-of-week

---

## Health Monitoring

### Automatic Lifecycle Events

AWS IoT Core publishes device connect/disconnect events automatically

### Health Metrics Reporting

Device includes in Shadow updates:

- WiFi RSSI (signal strength)
- Uptime (seconds since boot)
- Free heap memory
- Reported with each state update

---

## User Interface (Future Scope)

### Lambda Functions

- Handle user requests from web/mobile interface
- Update Device Shadow with desired state
- Query current device state from Shadow
- Business logic for alarm management

**Current Status:** Out of scope, to be implemented later

---

## Communication Flow Examples

### Physical Button Press

1. User presses button on device
2. ESP32 immediately updates local hardware
3. ESP32 publishes new state to Shadow (reported)
4. Lambda receives Shadow update notification
5. User interface reflects new state

### Cloud Command

1. User changes setting via interface
2. Lambda updates Shadow (desired state)
3. Shadow generates delta message
4. ESP32 receives delta
5. ESP32 executes command on hardware
6. ESP32 reports result back to Shadow

### Offline Alarm Trigger

1. Device loses WiFi connection
2. Alarm time arrives
3. Device triggers alarm using local schedule
4. Device reconnects to WiFi
5. Device syncs state with Shadow

---

## Security

### Device Authentication

- X.509 certificate-based authentication
- Unique credentials per device
- Certificates must not be committed to version control

### Data in Transit

- TLS encryption for all MQTT communication
- Secure connection to AWS IoT endpoint

---

## Development Approach

### ESP32 Firmware

- Developed by team member
- Delivered via GitHub with Pull Request workflow
- Code review and approval before merge
- Testing against real AWS IoT Core

### AWS Backend

- Developed by project owner
- Lambda functions for user interface
- Business logic implementation

---

## Testing Strategy

### Firmware Testing

- Direct AWS IoT connection with real credentials
- Python scripts simulate cloud commands
- Verify two-way communication
- Test offline behavior and reconnection
- Flash persistence verification

### Integration Testing

- Full end-to-end flow testing
- User interface interaction (when implemented)
- Error handling and edge cases

---

## Key Technical Decisions

**Why AWS IoT Core:**

- Eliminates need for web server on ESP32
- Serverless, no infrastructure management
- Built-in Device Shadow for state sync
- Scales easily if additional devices added

**Why Hybrid Approach:**

- Maintains functionality during internet outages
- Reduces latency for critical alarm function
- Cloud provides convenience and remote access
- Device provides reliability

**Why Thin Client Pattern:**

- Simplifies ESP32 code
- Easier to update business logic (cloud-side only)
- Better testability
- Clear separation of concerns
