/*
 * ESP32 Capacitive Button Test Script
 * 
 * Hardware:
 *   - Button 1: GPIO34 (via AT42QT1070)
 *   - Button 2: GPIO35 (via AT42QT1070)
 *   - AT42QT1070 outputs: Active LOW (pressed = LOW)
 * 
 * Serial Commands (115200 baud):
 *   s - Print current button states
 *   t - Toggle continuous monitoring on/off
 * 
 * By default, prints button changes as they happen.
 */

// Pin definitions
const int PIN_BUTTON_1 = 34;
const int PIN_BUTTON_2 = 35;

// Button state tracking
bool button1State = HIGH;      // HIGH = not pressed (active LOW)
bool button2State = HIGH;
bool button1LastState = HIGH;
bool button2LastState = HIGH;

// Debounce
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
const unsigned long DEBOUNCE_DELAY = 50;  // ms

// Monitoring mode
bool continuousMonitoring = true;

// Press counters
int button1PressCount = 0;
int button2PressCount = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  
  // Configure button inputs (no pull-up needed - AT42QT1070 is push-pull)
  pinMode(PIN_BUTTON_1, INPUT);
  pinMode(PIN_BUTTON_2, INPUT);
  
  // Read initial states
  button1State = digitalRead(PIN_BUTTON_1);
  button2State = digitalRead(PIN_BUTTON_2);
  button1LastState = button1State;
  button2LastState = button2State;
  
  Serial.println();
  Serial.println("================================");
  Serial.println("ESP32 Button Test Script Ready");
  Serial.println("================================");
  Serial.println("Hardware: AT42QT1070 (Active LOW)");
  Serial.println("  Button 1: GPIO34");
  Serial.println("  Button 2: GPIO35");
  Serial.println("--------------------------------");
  Serial.println("Commands:");
  Serial.println("  s - Print current status");
  Serial.println("  t - Toggle monitoring on/off");
  Serial.println("================================");
  Serial.println();
  Serial.println("Monitoring button presses...");
  Serial.println("(Press buttons to test)");
  Serial.println();
  
  printStatus();
}

void loop() {
  // Handle serial commands
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.length() > 0) {
      processCommand(input);
    }
  }
  
  // Read and debounce buttons
  checkButton1();
  checkButton2();
}

void checkButton1() {
  bool reading = digitalRead(PIN_BUTTON_1);
  
  if (reading != button1LastState) {
    lastDebounceTime1 = millis();
  }
  
  if ((millis() - lastDebounceTime1) > DEBOUNCE_DELAY) {
    if (reading != button1State) {
      button1State = reading;
      
      if (continuousMonitoring) {
        if (button1State == LOW) {
          button1PressCount++;
          Serial.print("[BUTTON 1] PRESSED  (count: ");
          Serial.print(button1PressCount);
          Serial.println(")");
        } else {
          Serial.println("[BUTTON 1] RELEASED");
        }
      }
    }
  }
  
  button1LastState = reading;
}

void checkButton2() {
  bool reading = digitalRead(PIN_BUTTON_2);
  
  if (reading != button2LastState) {
    lastDebounceTime2 = millis();
  }
  
  if ((millis() - lastDebounceTime2) > DEBOUNCE_DELAY) {
    if (reading != button2State) {
      button2State = reading;
      
      if (continuousMonitoring) {
        if (button2State == LOW) {
          button2PressCount++;
          Serial.print("[BUTTON 2] PRESSED  (count: ");
          Serial.print(button2PressCount);
          Serial.println(")");
        } else {
          Serial.println("[BUTTON 2] RELEASED");
        }
      }
    }
  }
  
  button2LastState = reading;
}

void processCommand(String cmd) {
  char firstChar = cmd.charAt(0);
  
  switch (firstChar) {
    case 's':
    case 'S':
      printStatus();
      break;
      
    case 't':
    case 'T':
      continuousMonitoring = !continuousMonitoring;
      Serial.print("Continuous monitoring: ");
      Serial.println(continuousMonitoring ? "ON" : "OFF");
      break;
      
    default:
      Serial.print("Unknown command: ");
      Serial.println(cmd);
      break;
  }
}

void printStatus() {
  Serial.println("--- Current Status ---");
  Serial.print("  Button 1 (GPIO");
  Serial.print(PIN_BUTTON_1);
  Serial.print("): ");
  Serial.print(button1State == LOW ? "PRESSED" : "RELEASED");
  Serial.print("  [presses: ");
  Serial.print(button1PressCount);
  Serial.println("]");
  Serial.print("  Button 2 (GPIO");
  Serial.print(PIN_BUTTON_2);
  Serial.print("): ");
  Serial.print(button2State == LOW ? "PRESSED" : "RELEASED");
  Serial.print("  [presses: ");
  Serial.print(button2PressCount);
  Serial.println("]");
  Serial.print("  Monitoring: ");
  Serial.println(continuousMonitoring ? "ON" : "OFF");
  Serial.println("----------------------");
}
