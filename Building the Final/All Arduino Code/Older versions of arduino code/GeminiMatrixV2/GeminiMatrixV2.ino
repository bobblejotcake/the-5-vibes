const byte ROWS = 2;
const byte COLS = 4;

// MIDI CC numbers for the 8 buttons (CC 10 to 17)
const byte ccNumbers[ROWS][COLS] = {
  {10, 11, 12, 13},
  {14, 15, 16, 17}
};

// Row pins (Inputs with Pullups) - 
const byte rowPins[ROWS] = {12, 11}; 

// Column pins (Driven LOW one by one)
const byte colPins[COLS] = {2, 3, 4, 5};

// Tracks the actual physical state of each button to prevent repeat triggers
bool isPressed[ROWS][COLS] = {
  {false, false, false, false},
  {false, false, false, false}
};

// Send standard 3-byte MIDI CC message
void sendMidiCC(byte channel, byte controlNumber, byte value) {
  byte midiPacket[3];
  midiPacket[0] = (byte)(0xB0 | (channel & 0x0F));
  midiPacket[1] = (byte)(controlNumber & 0x7F);
  midiPacket[2] = (byte)(value & 0x7F);
  Serial.write(midiPacket, 3);
}

void setup() {
  Serial.begin(115200);

  // Initialize Row pins as inputs with internal pullups (normally HIGH)
  for (byte r = 0; r < ROWS; r++) {
    pinMode(rowPins[r], INPUT_PULLUP);
  }

  // Initialize Column pins as inputs (High-Z) by default to prevent leakage
  for (byte c = 0; c < COLS; c++) {
    pinMode(colPins[c], INPUT);
  }

  delay(300);
  while (Serial.available() > 0) {
    Serial.read();
  }
}

void loop() {
  // Scan one column at a time
  for (byte c = 0; c < COLS; c++) {
    
    // 1. Activate column: Set to OUTPUT and drive LOW (0V)
    pinMode(colPins[c], OUTPUT);
    digitalWrite(colPins[c], LOW);
    
    // Give 5 microseconds for the capacitance of the breadboard to settle
    delayMicroseconds(5);

    // 2. Read each row in this active column
    for (byte r = 0; r < ROWS; r++) {
      // Button is pressed if the Row pin is pulled LOW (active-low)
      bool currentlyDown = (digitalRead(rowPins[r]) == LOW);

      // Check for state changes ONLY (prevents repeat spam)
      if (currentlyDown && !isPressed[r][c]) {
        isPressed[r][c] = true;
        sendMidiCC(0, ccNumbers[r][c], 127); // Press event
        //To check if members of the same "instrument" are selected
        //IF so, increase the complexity.
      } 
      else if (!currentlyDown && isPressed[r][c]) {
        isPressed[r][c] = false;
        sendMidiCC(0, ccNumbers[r][c], 0);   // Release event
      }
    }

    // 3. Deactivate column: Put back in High-Z mode (INPUT)
    pinMode(colPins[c], INPUT);
  }

  // 10ms debounce delay - sets scan rate to ~100Hz (zero perceivable lag)
  delay(10);
}
