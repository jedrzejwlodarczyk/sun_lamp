/*
 * ESP32 WS2812B Status LED Test Script
 * 
 * Hardware:
 *   - WS2812B Data: GPIO25
 *   - Number of LEDs: 2
 * 
 * Requires: FastLED library
 *   Install via: Sketch > Include Library > Manage Libraries > search "FastLED"
 * 
 * Serial Commands (115200 baud):
 *   1r / 1g / 1b / 1w / 1o  - LED 1: Red/Green/Blue/White/Off
 *   2r / 2g / 2b / 2w / 2o  - LED 2: Red/Green/Blue/White/Off
 *   ar / ag / ab / aw / ao  - All LEDs: Red/Green/Blue/White/Off
 *   s                       - Print current status
 *   t                       - Run test sequence
 */

#include <FastLED.h>

// Pin and LED configuration
const int PIN_WS2812B = 25;
const int NUM_LEDS = 2;

// LED array
CRGB leds[NUM_LEDS];

// Track current colors for status display
String led1Color = "OFF";
String led2Color = "OFF";

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  
  // Initialize FastLED
  FastLED.addLeds<WS2812B, PIN_WS2812B, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(128);  // 50% brightness for testing
  
  // Start with all LEDs off
  FastLED.clear();
  FastLED.show();
  
  Serial.println();
  Serial.println("================================");
  Serial.println("ESP32 WS2812B Test Script Ready");
  Serial.println("================================");
  Serial.println("Hardware: 2x WS2812B on GPIO25");
  Serial.println("--------------------------------");
  Serial.println("Commands:");
  Serial.println("  1r/1g/1b/1w/1o - LED 1 color");
  Serial.println("  2r/2g/2b/2w/2o - LED 2 color");
  Serial.println("  ar/ag/ab/aw/ao - All LEDs");
  Serial.println("  s              - Status");
  Serial.println("  t              - Test sequence");
  Serial.println("--------------------------------");
  Serial.println("Colors: r=Red g=Green b=Blue");
  Serial.println("        w=White o=Off");
  Serial.println("================================");
  printStatus();
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    input.toLowerCase();
    
    if (input.length() > 0) {
      processCommand(input);
    }
  }
}

void processCommand(String cmd) {
  if (cmd.length() < 1) return;
  
  char firstChar = cmd.charAt(0);
  
  // Single character commands
  if (cmd.length() == 1) {
    if (firstChar == 's') {
      printStatus();
      return;
    } else if (firstChar == 't') {
      runTestSequence();
      return;
    }
  }
  
  // Two character commands (LED + color)
  if (cmd.length() >= 2) {
    char colorChar = cmd.charAt(1);
    CRGB color = parseColor(colorChar);
    String colorName = getColorName(colorChar);
    
    if (colorName == "UNKNOWN") {
      Serial.println("Error: Unknown color. Use r/g/b/w/o");
      return;
    }
    
    switch (firstChar) {
      case '1':
        setLed(0, color, colorName);
        break;
      case '2':
        setLed(1, color, colorName);
        break;
      case 'a':
        setAllLeds(color, colorName);
        break;
      default:
        Serial.print("Unknown command: ");
        Serial.println(cmd);
        break;
    }
  }
}

CRGB parseColor(char c) {
  switch (c) {
    case 'r': return CRGB::Red;
    case 'g': return CRGB::Green;
    case 'b': return CRGB::Blue;
    case 'w': return CRGB::White;
    case 'o': return CRGB::Black;
    default:  return CRGB::Black;
  }
}

String getColorName(char c) {
  switch (c) {
    case 'r': return "RED";
    case 'g': return "GREEN";
    case 'b': return "BLUE";
    case 'w': return "WHITE";
    case 'o': return "OFF";
    default:  return "UNKNOWN";
  }
}

void setLed(int index, CRGB color, String colorName) {
  if (index < 0 || index >= NUM_LEDS) return;
  
  leds[index] = color;
  FastLED.show();
  
  if (index == 0) {
    led1Color = colorName;
  } else {
    led2Color = colorName;
  }
  
  Serial.print("LED ");
  Serial.print(index + 1);
  Serial.print(": ");
  Serial.println(colorName);
}

void setAllLeds(CRGB color, String colorName) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  FastLED.show();
  
  led1Color = colorName;
  led2Color = colorName;
  
  Serial.print("All LEDs: ");
  Serial.println(colorName);
}

void printStatus() {
  Serial.println("--- Current Status ---");
  Serial.print("  LED 1: ");
  Serial.println(led1Color);
  Serial.print("  LED 2: ");
  Serial.println(led2Color);
  Serial.println("----------------------");
}

void runTestSequence() {
  Serial.println();
  Serial.println("========== TEST SEQUENCE START ==========");
  
  // Step 1: All off
  Serial.println();
  Serial.println("[1/9] All OFF (baseline)");
  setAllLeds(CRGB::Black, "OFF");
  printStatus();
  delay(1000);
  
  // Step 2: LED 1 Red
  Serial.println();
  Serial.println("[2/9] LED 1 RED");
  setLed(0, CRGB::Red, "RED");
  printStatus();
  delay(1000);
  
  // Step 3: LED 1 Green
  Serial.println();
  Serial.println("[3/9] LED 1 GREEN");
  setLed(0, CRGB::Green, "GREEN");
  printStatus();
  delay(1000);
  
  // Step 4: LED 1 Blue
  Serial.println();
  Serial.println("[4/9] LED 1 BLUE");
  setLed(0, CRGB::Blue, "BLUE");
  printStatus();
  delay(1000);
  
  // Step 5: LED 1 off, LED 2 Red
  Serial.println();
  Serial.println("[5/9] LED 1 OFF, LED 2 RED");
  setLed(0, CRGB::Black, "OFF");
  setLed(1, CRGB::Red, "RED");
  printStatus();
  delay(1000);
  
  // Step 6: LED 2 Green
  Serial.println();
  Serial.println("[6/9] LED 2 GREEN");
  setLed(1, CRGB::Green, "GREEN");
  printStatus();
  delay(1000);
  
  // Step 7: LED 2 Blue
  Serial.println();
  Serial.println("[7/9] LED 2 BLUE");
  setLed(1, CRGB::Blue, "BLUE");
  printStatus();
  delay(1000);
  
  // Step 8: Both white
  Serial.println();
  Serial.println("[8/9] All WHITE");
  setAllLeds(CRGB::White, "WHITE");
  printStatus();
  delay(1500);
  
  // Step 9: All off
  Serial.println();
  Serial.println("[9/9] All OFF (end)");
  setAllLeds(CRGB::Black, "OFF");
  printStatus();
  
  Serial.println();
  Serial.println("========== TEST SEQUENCE COMPLETE ==========");
  Serial.println();
}
