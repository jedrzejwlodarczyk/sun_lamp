/*
 * ESP32 LED Test Script
 * 
 * Hardware:
 *   - Binary output: GPIO21
 *   - PWM1 (Warm White): GPIO4
 *   - PWM2 (Cool White): GPIO0
 * 
 * Serial Commands (115200 baud):
 *   b1       - Binary output ON
 *   b0       - Binary output OFF
 *   w<0-255> - Set warm white PWM (e.g., w128)
 *   c<0-255> - Set cool white PWM (e.g., c255)
 *   a        - All ON (binary + both PWM at 255)
 *   x        - All OFF
 *   s        - Print current status
 *   t        - Run test sequence
 */

// Pin definitions
const int PIN_BINARY = 21;
const int PIN_PWM_WARM = 4;
const int PIN_PWM_COOL = 0;

// PWM configuration
const int PWM_FREQ = 1000;       // 1kHz
const int PWM_RESOLUTION = 8;    // 8-bit (0-255)

// Current state
bool binaryState = false;
int warmValue = 0;
int coolValue = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  
  // Configure binary output
  pinMode(PIN_BINARY, OUTPUT);
  digitalWrite(PIN_BINARY, LOW);
  
  // Configure PWM - ESP32 Core 3.x API
  ledcAttach(PIN_PWM_WARM, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(PIN_PWM_COOL, PWM_FREQ, PWM_RESOLUTION);
  
  // Initialize PWM to 0
  ledcWrite(PIN_PWM_WARM, 0);
  ledcWrite(PIN_PWM_COOL, 0);
  
  Serial.println();
  Serial.println("================================");
  Serial.println("ESP32 LED Test Script Ready");
  Serial.println("================================");
  Serial.println("Commands:");
  Serial.println("  b1/b0    - Binary ON/OFF");
  Serial.println("  w<0-255> - Warm white PWM");
  Serial.println("  c<0-255> - Cool white PWM");
  Serial.println("  a        - All ON");
  Serial.println("  x        - All OFF");
  Serial.println("  s        - Status");
  Serial.println("  t        - Test sequence");
  Serial.println("================================");
  printStatus();
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.length() == 0) {
      return;
    }
    
    processCommand(input);
  }
}

void processCommand(String cmd) {
  char firstChar = cmd.charAt(0);
  
  switch (firstChar) {
    case 'b':
    case 'B':
      if (cmd.length() >= 2) {
        if (cmd.charAt(1) == '1') {
          setBinary(true);
        } else if (cmd.charAt(1) == '0') {
          setBinary(false);
        } else {
          Serial.println("Error: Use b1 or b0");
        }
      }
      break;
      
    case 'w':
    case 'W':
      if (cmd.length() >= 2) {
        int val = cmd.substring(1).toInt();
        if (val >= 0 && val <= 255) {
          setWarm(val);
        } else {
          Serial.println("Error: Value must be 0-255");
        }
      }
      break;
      
    case 'c':
    case 'C':
      if (cmd.length() >= 2) {
        int val = cmd.substring(1).toInt();
        if (val >= 0 && val <= 255) {
          setCool(val);
        } else {
          Serial.println("Error: Value must be 0-255");
        }
      }
      break;
      
    case 'a':
    case 'A':
      setAll(true);
      break;
      
    case 'x':
    case 'X':
      setAll(false);
      break;
      
    case 's':
    case 'S':
      printStatus();
      break;
      
    case 't':
    case 'T':
      runTestSequence();
      break;
      
    default:
      Serial.print("Unknown command: ");
      Serial.println(cmd);
      break;
  }
}

void setBinary(bool state) {
  binaryState = state;
  digitalWrite(PIN_BINARY, state ? HIGH : LOW);
  Serial.print("Binary: ");
  Serial.println(state ? "ON" : "OFF");
}

void setWarm(int value) {
  warmValue = value;
  ledcWrite(PIN_PWM_WARM, value);
  Serial.print("Warm PWM: ");
  Serial.println(value);
}

void setCool(int value) {
  coolValue = value;
  ledcWrite(PIN_PWM_COOL, value);
  Serial.print("Cool PWM: ");
  Serial.println(value);
}

void setAll(bool on) {
  if (on) {
    setBinary(true);
    setWarm(255);
    setCool(255);
    Serial.println("All outputs ON");
  } else {
    setBinary(false);
    setWarm(0);
    setCool(0);
    Serial.println("All outputs OFF");
  }
}

void printStatus() {
  Serial.println("--- Current Status ---");
  Serial.print("  Binary (GPIO");
  Serial.print(PIN_BINARY);
  Serial.print("): ");
  Serial.println(binaryState ? "ON" : "OFF");
  Serial.print("  Warm PWM (GPIO");
  Serial.print(PIN_PWM_WARM);
  Serial.print("): ");
  Serial.println(warmValue);
  Serial.print("  Cool PWM (GPIO");
  Serial.print(PIN_PWM_COOL);
  Serial.print("): ");
  Serial.println(coolValue);
  Serial.println("----------------------");
}

void runTestSequence() {
  Serial.println();
  Serial.println("========== TEST SEQUENCE START ==========");
  
  // Step 1: All off baseline
  Serial.println();
  Serial.println("[1/8] Baseline - All OFF");
  setAll(false);
  printStatus();
  delay(1500);
  
  // Step 2: Binary only
  Serial.println();
  Serial.println("[2/8] Binary output ON");
  setBinary(true);
  printStatus();
  delay(1500);
  
  // Step 3: Binary off, warm white 50%
  Serial.println();
  Serial.println("[3/8] Binary OFF, Warm white 50%");
  setBinary(false);
  setWarm(128);
  printStatus();
  delay(1500);
  
  // Step 4: Warm white 100%
  Serial.println();
  Serial.println("[4/8] Warm white 100%");
  setWarm(255);
  printStatus();
  delay(1500);
  
  // Step 5: Warm off, cool white 50%
  Serial.println();
  Serial.println("[5/8] Warm OFF, Cool white 50%");
  setWarm(0);
  setCool(128);
  printStatus();
  delay(1500);
  
  // Step 6: Cool white 100%
  Serial.println();
  Serial.println("[6/8] Cool white 100%");
  setCool(255);
  printStatus();
  delay(1500);
  
  // Step 7: All on full
  Serial.println();
  Serial.println("[7/8] ALL ON - Full brightness");
  setAll(true);
  printStatus();
  delay(2000);
  
  // Step 8: All off
  Serial.println();
  Serial.println("[8/8] ALL OFF - End of test");
  setAll(false);
  printStatus();
  
  Serial.println();
  Serial.println("========== TEST SEQUENCE COMPLETE ==========");
  Serial.println();
}
