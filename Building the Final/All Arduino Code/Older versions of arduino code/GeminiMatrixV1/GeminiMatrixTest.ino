#include "Adafruit_Keypad.h"

const byte ROWS = 2;
const byte COLS = 4;

// Key labels for your 8 buttons
char keys[ROWS][COLS] = {
  {'1', '2', '3', '4'},
  {'5', '6', '7', '8'}
};

// Row connections: Row 0 -> D2, Row 1 -> D3
byte rowPins[ROWS] = {13, 12};

// Column connections: Col 0 -> D4, Col 1 -> D5, Col 2 -> D6, Col 3 -> D7
byte colPins[COLS] = {2, 3, 4, 5};

// Initialize the keypad instance
Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  customKeypad.begin();
}

void loop() {
  // Scan the matrix
  customKeypad.tick();

  // Process all button events
  while (customKeypad.available()) {
    keypadEvent e = customKeypad.read();

    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      Serial.print("Pressed: ");
      Serial.println((char)e.bit.KEY);
    } 
    else if (e.bit.EVENT == KEY_JUST_RELEASED) {
      Serial.print("Released: ");
      Serial.println((char)e.bit.KEY);
    }
  }

  delay(10); // Small delay to prevent swamping the serial buffer
}
