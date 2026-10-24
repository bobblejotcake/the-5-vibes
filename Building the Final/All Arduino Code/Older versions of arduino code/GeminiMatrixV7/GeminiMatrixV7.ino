
#include <FastLED.h>

const byte ROWS = 4;
const byte COLS = 8;

//LED SETUP
//
// LED pins on the Mega (one per row)
#define PIN_A 7   
#define PIN_B 6
#define PIN_C 5
#define PIN_D 4   

// number of LEDs on each strip
#define NUM_A 32
#define NUM_B 40
#define NUM_C 74
#define NUM_D 89

CRGB ledsA[NUM_A];
CRGB ledsB[NUM_B];
CRGB ledsC[NUM_C];
CRGB ledsD[NUM_D];
CRGB* rowLeds[4] = {ledsA, ledsB, ledsC, ledsD};

// row colours: A red, B green, C blue, D orange
CRGB rowColour[4] = {CRGB(255, 0, 0), CRGB(0, 255, 0), CRGB(0, 0, 255), CRGB(255, 100, 0)};

// brightness levels (0 - 255)
const uint8_t off = 0;
const uint8_t dim = 50;
const uint8_t medium = 120;
const uint8_t high = 190;
const uint8_t bright = 255;

// first and last LED of each panel (from our spreadsheet)
const int panelRanges[4][8][2] = {
  {{0,3},  {4,7},   {8,11},  {12,15}, {16,19}, {20,23}, {24,27}, {28,31}},   // A
  {{0,4},  {5,9},   {10,14}, {15,19}, {20,24}, {25,29}, {30,34}, {35,39}},   // B
  {{0,8},  {9,17},  {18,26}, {27,35}, {36,45}, {46,55}, {56,64}, {65,73}},   // C
  {{0,10}, {11,21}, {22,33}, {34,44}, {45,55}, {56,66}, {67,77}, {78,88}}    // D
};

const char* panelNames[4][8] = {
  {"A1","A2","A3","A4","A5","A6","A7","A8"},   // A
  {"B1","B2","B3","B4","B5","B6","B7","B8"},    // B
  {"C1","C2","C3","C4","C5","C6","C7","C8"},    // C
  {"D1","D2","D3","D4","D5","D6","D7","D8"},    // D
};

// current brightness of each panel, 0 = off
uint8_t panelBrightness[4][8];

// call this once inside setup()
void setupLEDs() {
  FastLED.addLeds<WS2812B, PIN_A, GRB>(ledsA, NUM_A);
  FastLED.addLeds<WS2812B, PIN_B, GRB>(ledsB, NUM_B);
  FastLED.addLeds<WS2812B, PIN_C, GRB>(ledsC, NUM_C);
  FastLED.addLeds<WS2812B, PIN_D, GRB>(ledsD, NUM_D);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 4000);   // set to match the power supply
  FastLED.clear();  // clear all pixel data
  FastLED.show();
}

// light one panel at a brightness, e.g. lightPanel("A1", medium);
void lightPanel(const char* name, uint8_t brightness) {
  int row = name[0] - 'A';
  int panel = atoi(name + 1) - 1;
  if (row < 0 || row > 3 || panel < 0 || panel > 7) return;

  panelBrightness[row][panel] = brightness;

  CRGB colour = rowColour[row];
  colour.nscale8_video(brightness);
  for (int i = panelRanges[row][panel][0]; i <= panelRanges[row][panel][1]; i++) {
    rowLeds[row][i] = colour;
  }
  FastLED.show();
}

// make every lit panel in a row one level brighter, e.g. brightenRow('C');
void brightenRow(char rowLetter, bool toBrighten) {
  int row = rowLetter - 'A';
  if (row < 0 || row > 3) return;

  for (int panel = 0; panel < 8; panel++) {
    uint8_t b = panelBrightness[row][panel];
    if (b == 0) continue;

    if (toBrighten == true){
     if (b < medium) b = medium;
      else if (b < high) b = high;
      else b = bright;
    };
    if (toBrighten == false){
      if (b >= bright) b = high;
      else if (b >= high) b = medium;
      else if (b >= medium) b = dim;
      else if (b >= dim) b = off;
     };
    
    char name[3] = {rowLetter, char('1' + panel), '\0'};
    lightPanel(name, b);
  }
}


// call this when a button is pressed, e.g. pressPanel("C2");
// the first panel in a row lights dim, each new panel in that row brightens the whole row
void pressPanel(const char* name,bool pressed) {
  int row = name[0] - 'A';
  int panel = atoi(name + 1) - 1;
  if (row < 0 || row > 3 || panel < 0 || panel > 7) return; //Not in scope
  //if (panelBrightness[row][panel] > 0) return;   // already on

  // find how bright the rest of the row is
  uint8_t level = 0;
  for (int p = 0; p < 8; p++) {
    if (panelBrightness[row][p] > level) level = panelBrightness[row][p];
  }

  if (level == 0 && pressed == true) {
    lightPanel(name, dim);
  } else if (pressed == false) {
    lightPanel(name, off);
    brightenRow(name[0], false);
  } else{
    lightPanel(name, level);
    brightenRow(name[0], true);
  }
}


//MIDI/Matrix Setup
//

// MIDI CC numbers for the buttons
const byte ccNumbers[ROWS][COLS] = {
  {18, 11, 12, 13, 14, 15, 16, 17},
  {19, 20, 21, 22, 23, 24, 25, 26},
  {28, 29, 30, 31, 32, 33, 34, 35},
  {37, 38, 39, 40, 41 ,42, 43, 44}
};

// Row pins (Cathodes - Driven LOW one by one)
const byte rowPins[ROWS] = {30,32,34,36}; 

// Column pins (Anodes - Inputs with Pullups)
const byte colPins[COLS] = {31,33,35,37,39,41,43,45};

bool isPressed[ROWS][COLS] = {
  {false, false, false, false, false, false, false, false},
  {false, false, false, false, false, false, false, false},
  {false, false, false, false, false, false, false, false},
  {false, false, false, false, false, false, false, false}
};

//Sends a Midi packet through LOOPMIDI and EA Serial. Follows the pattern of ____.
void sendMidiCC(byte channel, byte controlNumber, byte value) {
  byte midiPacket[3];
  midiPacket[0] = (byte)(0xB0 | (channel & 0x0F));
  midiPacket[1] = (byte)(controlNumber & 0x7F);
  midiPacket[2] = (byte)(value & 0x7F);
  Serial.write(midiPacket, 3);
}

void setup() {
  Serial.begin(115200);
  setupLEDs();

  //Setup every column.
  for (byte c = 0; c < COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }
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
        pressPanel(panelNames[r][c],true);
        //Check the row.
        int totalPressed = 0;
        for (int panelMidi = 0; panelMidi < 8; panelMidi++){
          if (isPressed[r][panelMidi] == true){
            totalPressed = totalPressed + 1;
          }
        }   

        int rowA = 0;
        int rowB = 1;
        int rowC = 2;
        int rowD = 3;
        
        //Depending on the amount of buttons pressed, increase the complexity.
        if(r == rowA){
          switch(totalPressed){
            case 1:
            for ( int x = 0; x < 1; x++){
             sendMidiCC(0, ccNumbers[rowA][x], 127);
            }
            break;

            case 2:
            for ( int x = 0; x < 2; x++){
             sendMidiCC(0, ccNumbers[rowA][x], 127);
            }
            break;

            case 3:
            for ( int x = 0; x < 3; x++){
             sendMidiCC(0, ccNumbers[rowA][x], 127);
            }
            break;

            case 4:
            for ( int x = 0; x < 4; x++){
             sendMidiCC(0, ccNumbers[rowA][x], 127);
            }
            break;
            
            case 5:
            for ( int x = 0; x < 5; x++){
             sendMidiCC(0, ccNumbers[rowA][x], 127);
            }
            break;
            
            case 6:
            for ( int x = 0; x < 6; x++){
             sendMidiCC(0, ccNumbers[rowA][x], 127);
            }
            break;
            
            case 7:
            for ( int x = 0; x < 7; x++){
             sendMidiCC(0, ccNumbers[rowA][x], 127);
            }
            break;
          }}
          
         else if(r == rowB){
          switch(totalPressed){
            case 1:
            for ( int x = 0; x < 1; x++){
             sendMidiCC(0, ccNumbers[rowB][x], 127);
            }
            break;

            case 2:
            for ( int x = 0; x < 2; x++){
             sendMidiCC(0, ccNumbers[rowB][x], 127);
            }
            break;

            case 3:
            for ( int x = 0; x < 3; x++){
             sendMidiCC(0, ccNumbers[rowB][x], 127);
            }
            break;

            case 4:
            for ( int x = 0; x < 4; x++){
             sendMidiCC(0, ccNumbers[rowB][x], 127);
            }
            break;
            
            case 5:
            for ( int x = 0; x < 5; x++){
             sendMidiCC(0, ccNumbers[rowB][x], 127);
            }
            break;
            
            case 6:
            for (int x = 0; x < 6; x++){
             sendMidiCC(0, ccNumbers[rowB][x], 127);
            }
            break;
            
            case 7:
            for ( int x = 0; x < 7; x++){
             sendMidiCC(0, ccNumbers[rowB][x], 127);
            }
            break;
            
          }}

          else if(r == rowC){
          switch(totalPressed){
            case 1:
            sendMidiCC(0, ccNumbers[rowC][0], 127);
            break;

            case 2:
            sendMidiCC(0, ccNumbers[rowC][0], 0);
            sendMidiCC(0, ccNumbers[rowC][1], 127);
            break;

            case 3:
            //Need to mute the previous track while playing the new track?!?
            sendMidiCC(0, ccNumbers[rowC][1], 0);
            sendMidiCC(0, ccNumbers[rowC][2], 127);
            break;

            case 4:
            sendMidiCC(0, ccNumbers[rowC][3], 127);
            break;
            
            case 5:
            sendMidiCC(0, ccNumbers[rowC][4], 127);
            break;
            
            case 6:
            sendMidiCC(0, ccNumbers[rowC][5], 127);
            break;
            
            case 7:
            sendMidiCC(0, ccNumbers[rowC][6], 127);
            break;
            
          }}

          else if(r == rowD){
          switch(totalPressed){
            case 1:
            for ( int x = 0; x < 1; x++){
             sendMidiCC(0, ccNumbers[rowD][x], 127);
            }
            break;

            case 2:
            for ( int x = 0; x < 2; x++){
             sendMidiCC(0, ccNumbers[rowD][x], 127);
            }
            break;

            case 3:
            for ( int x = 0; x < 3; x++){
             sendMidiCC(0, ccNumbers[rowD][x], 127);
            }
            break;

            case 4:
            for ( int x = 0; x < 4; x++){
             sendMidiCC(0, ccNumbers[rowD][x], 127);
            }
            break;
            
            case 5:
            for ( int x = 0; x < 5; x++){
             sendMidiCC(0, ccNumbers[rowD][x], 127);
            }
            break;
            
            case 6:
            for (int x = 0; x < 6; x++){
             sendMidiCC(0, ccNumbers[rowD][x], 127);
            }
            break;
            
            case 7:
            for ( int x = 0; x < 7; x++){
             sendMidiCC(0, ccNumbers[rowD][x], 127);
            }
            break;
            
          }}
         
          }
        
       
      else if (!currentlyDown && isPressed[r][c]) {
        isPressed[r][c] = false;
        pressPanel(panelNames[r][c],false);

        int totalPressed = 0;
        for (int panelMidi = 0; panelMidi < 8; panelMidi++){
          if (isPressed[r][panelMidi] == true){
            totalPressed = totalPressed + 1;
          }
        }
        int rowA = 0;
        int rowB = 1;
        int rowC = 2;
        int rowD = 3;

        if(r == rowA){
            switch(totalPressed){
              case 0:
              for (int x = 0; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowA][x], 0);
              }
              break;
              
              case 1:
              for (int x = 1; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowA][x], 0);
              }
              break;

              case 2:
              for (int x = 2; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowA][x], 0);
              }
              break;

              case 3:
              for (int x = 3; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowA][x], 0);
              }
              break;

              case 4:
              for (int x = 4; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowA][x], 0);
              }
              break;

              case 5:
              for (int x = 5; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowA][x], 0);
              }
              break;

              case 6:
              for (int x = 6; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowA][x], 0);
              }
              break;

              case 7:
              for (int x = 7; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowA][x], 0);
              }
              break;
            }}

          else if(r == rowB){
            switch(totalPressed){
              case 0:
              for (int x = 0; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowB][x], 0);
              }
              break;
              
              case 1:
              for (int x = 1; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowB][x], 0);
              }
              break;

              case 2:
              for (int x = 2; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowB][x], 0);
              }
              break;

              case 3:
              for (int x = 3; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowB][x], 0);
              }
              break;

              case 4:
              for (int x = 4; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowB][x], 0);
              }
              break;

              case 5:
              for (int x = 5; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowB][x], 0);
              }
              break;

              case 6:
              for (int x = 6; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowB][x], 0);
              }
              break;

              case 7:
              for (int x = 7; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowB][x], 0);
              }
              break;
            } 
         }

          else if(r == rowC){
            switch(totalPressed){
              case 0:
              for (int x = 0; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowC][x], 0);
              }
              break;
              
              case 1:
              sendMidiCC(0, ccNumbers[rowC][0], 127);
              sendMidiCC(0, ccNumbers[rowC][1], 0);
              sendMidiCC(0, ccNumbers[rowC][2], 0);
              sendMidiCC(0, ccNumbers[rowC][3], 0);
              sendMidiCC(0, ccNumbers[rowC][4], 0);
              sendMidiCC(0, ccNumbers[rowC][5], 0);
              sendMidiCC(0, ccNumbers[rowC][6], 0);
              sendMidiCC(0, ccNumbers[rowC][7], 0);
              break;

              case 2:
              sendMidiCC(0, ccNumbers[rowC][0], 0);
              sendMidiCC(0, ccNumbers[rowC][1], 127);
              sendMidiCC(0, ccNumbers[rowC][2], 0);
              sendMidiCC(0, ccNumbers[rowC][3], 0);
              sendMidiCC(0, ccNumbers[rowC][4], 0);
              sendMidiCC(0, ccNumbers[rowC][5], 0);
              sendMidiCC(0, ccNumbers[rowC][6], 0);
              sendMidiCC(0, ccNumbers[rowC][7], 0);
              break;

              case 3:
              sendMidiCC(0, ccNumbers[rowC][0], 0);
              sendMidiCC(0, ccNumbers[rowC][1], 0);
              sendMidiCC(0, ccNumbers[rowC][2], 127);
              sendMidiCC(0, ccNumbers[rowC][3], 0);
              sendMidiCC(0, ccNumbers[rowC][4], 127);
              sendMidiCC(0, ccNumbers[rowC][5], 127);
              sendMidiCC(0, ccNumbers[rowC][6], 127);
              sendMidiCC(0, ccNumbers[rowC][7], 127);
              break;

              case 4:
              sendMidiCC(0, ccNumbers[rowC][0], 0);
              sendMidiCC(0, ccNumbers[rowC][1], 0);
              sendMidiCC(0, ccNumbers[rowC][2], 0);
              sendMidiCC(0, ccNumbers[rowC][3], 127);
              sendMidiCC(0, ccNumbers[rowC][4], 0);
              sendMidiCC(0, ccNumbers[rowC][5], 0);
              sendMidiCC(0, ccNumbers[rowC][6], 0);
              sendMidiCC(0, ccNumbers[rowC][7], 0);
              break;

              case 5:
              sendMidiCC(0, ccNumbers[rowC][0], 0);
              sendMidiCC(0, ccNumbers[rowC][1], 0);
              sendMidiCC(0, ccNumbers[rowC][2], 0);
              sendMidiCC(0, ccNumbers[rowC][3], 0);
              sendMidiCC(0, ccNumbers[rowC][4], 127);
              sendMidiCC(0, ccNumbers[rowC][5], 0);
              sendMidiCC(0, ccNumbers[rowC][6], 0);
              sendMidiCC(0, ccNumbers[rowC][7], 0);
              break;

              case 6:
              sendMidiCC(0, ccNumbers[rowC][0], 0);
              sendMidiCC(0, ccNumbers[rowC][1], 0);
              sendMidiCC(0, ccNumbers[rowC][2], 0);
              sendMidiCC(0, ccNumbers[rowC][3], 0);
              sendMidiCC(0, ccNumbers[rowC][4], 0);
              sendMidiCC(0, ccNumbers[rowC][5], 127);
              sendMidiCC(0, ccNumbers[rowC][6], 0);
              sendMidiCC(0, ccNumbers[rowC][7], 0);
              break;

              case 7:
              sendMidiCC(0, ccNumbers[rowC][0], 0);
              sendMidiCC(0, ccNumbers[rowC][1], 0);
              sendMidiCC(0, ccNumbers[rowC][2], 0);
              sendMidiCC(0, ccNumbers[rowC][3], 0);
              sendMidiCC(0, ccNumbers[rowC][4], 0);
              sendMidiCC(0, ccNumbers[rowC][5], 0);
              sendMidiCC(0, ccNumbers[rowC][6], 127);
              sendMidiCC(0, ccNumbers[rowC][7], 0);
              break;
            } 
         }
          else if(r == rowD){
            switch(totalPressed){
              case 0:
              for (int x = 0; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowD][x], 0);
              }
              break;
              
              case 1:
              for (int x = 1; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowD][x], 0);
              }
              break;

              case 2:
              for (int x = 2; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowD][x], 0);
              }
              break;

              case 3:
              for (int x = 3; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowD][x], 0);
              }
              break;

              case 4:
              for (int x = 4; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowD][x], 0);
              }
              break;

              case 5:
              for (int x = 5; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowD][x], 0);
              }
              break;

              case 6:
              for (int x = 6; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowD][x], 0);
              }
              break;

              case 7:
              for (int x = 7; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowD][x], 0);
              }
              break;
            } 
         }
      }
    }

    // Deactivate row: return to High-Z mode
    pinMode(rowPins[r], INPUT);
  }

  delay(10);
}
