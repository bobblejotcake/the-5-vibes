const byte ROWS = 2;
const byte COLS = 4;

// MIDI CC numbers for the 8 buttons (CC 10 to 17)
const byte ccNumbers[ROWS][COLS] = {
  {10, 11, 12, 13},
  {14, 15, 16, 17}
};

// Row pins (Cathodes - Driven LOW one by one)
const byte rowPins[ROWS] = {12, 11}; 

// Column pins (Anodes - Inputs with Pullups)
const byte colPins[COLS] = {2, 3, 4, 5};

bool isPressed[ROWS][COLS] = {
  {false, false, false, false},
  {false, false, false, false}
};

void sendMidiCC(byte channel, byte controlNumber, byte value) {
  byte midiPacket[3];
  midiPacket[0] = (byte)(0xB0 | (channel & 0x0F));
  midiPacket[1] = (byte)(controlNumber & 0x7F);
  midiPacket[2] = (byte)(value & 0x7F);
  Serial.write(midiPacket, 3);
}

void setup() {
  Serial.begin(115200);

  // 1. Column pins are the inputs (Anodes held HIGH at 5V)
  for (byte c = 0; c < COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }

  // 2. Row pins stay high-impedance until actively pulsed
  for (byte r = 0; r < ROWS; r++) {
    pinMode(rowPins[r], INPUT);
  }

  delay(300);
  while (Serial.available() > 0) {
    Serial.read();
  }
}

void loop() {
  // Scan one ROW at a time
  for (byte r = 0; r < ROWS; r++) {
    
    // Activate row: pull it to LOW (0V)
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW);
    
    delayMicroseconds(5); // Settle breadboard capacitance

    // Read each COLUMN on this active row
    for (byte c = 0; c < COLS; c++) {
      // Pressed if column is pulled LOW through switch and diode to row
      bool currentlyDown = (digitalRead(colPins[c]) == LOW);

      if (currentlyDown && !isPressed[r][c]) {
        isPressed[r][c] = true;
        sendMidiCC(0, ccNumbers[r][c], 127);
      } 
      else if (!currentlyDown && isPressed[r][c]) {
        isPressed[r][c] = false;
        sendMidiCC(0, ccNumbers[r][c], 0);
      }
    }

    // Deactivate row: return to High-Z mode
    pinMode(rowPins[r], INPUT);
  }

  delay(10);
}
