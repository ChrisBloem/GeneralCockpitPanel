#include <Arduino.h>
#include <Joystick.h>

constexpr uint8_t NUM_SWITCHES = 16;
const uint8_t switchPins[NUM_SWITCHES] = {
  2,3,4,5,6,7,8,9, 10,14,15,16, A0, A1, A2, A3
};

// ---- LED helpers -------------------------------------------------------------
#ifndef TXLED0
  // Fallback pin numbers (adjust if your clone differs)
  #define TX_LED_PIN 17
  #define RX_LED_PIN 30
  #define TXLED0 digitalWrite(TX_LED_PIN, LOW)   // ON  (active low)
  #define TXLED1 digitalWrite(TX_LED_PIN, HIGH)  // OFF
  #define RXLED0 digitalWrite(RX_LED_PIN, LOW)
  #define RXLED1 digitalWrite(RX_LED_PIN, HIGH)
#endif

void blinkTxRxPattern(uint8_t cycles = 3, uint16_t onMs = 120, uint16_t gapMs = 80) {
  for (uint8_t i = 0; i < cycles; i++) {
    TXLED0; RXLED1; delay(onMs);   // TX ON, RX OFF
    TXLED1; RXLED0; delay(onMs);   // RX ON, TX OFF
    TXLED1; RXLED1; delay(gapMs);  // both OFF
  }
}

// ---- HID ---------------------------------------------------------------
bool lastState[NUM_SWITCHES];

Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_GAMEPAD,
  NUM_SWITCHES, 0,        // 16 buttons total, 0 hats
  false,false,false,      // X,Y,Z
  false,false,false,      // Rx,Ry,Rz
  false,false,            // rudder, throttle
  false,false,false       // accelerator, brake, steering
);

inline bool readSwitch(uint8_t pin) {
  return digitalRead(pin) == LOW; // LOW == ON (pressed)
}

void setup() {
#ifdef TX_LED_PIN
  pinMode(TX_LED_PIN, OUTPUT);
  pinMode(RX_LED_PIN, OUTPUT);
  TXLED1; RXLED1; // start OFF
#endif

  for (uint8_t i = 0; i < NUM_SWITCHES; i++) {
    pinMode(switchPins[i], INPUT_PULLUP);
    lastState[i] = readSwitch(switchPins[i]);
  }

  Joystick.begin(false); // manual send mode
  for (uint8_t i = 0; i < NUM_SWITCHES; i++) {
    Joystick.setButton(i, lastState[i]);
  }
  Joystick.sendState();

  blinkTxRxPattern(); // confirmation blink
}

void loop() {
  bool changed = false;
  for (uint8_t i = 0; i < NUM_SWITCHES; i++) {
    bool current = readSwitch(switchPins[i]);
    if (current != lastState[i]) {
      lastState[i] = current;
      Joystick.setButton(i, current);
      changed = true;
    }
  }
  if (changed) {
    Joystick.sendState();
  }
  delay(5);
}
