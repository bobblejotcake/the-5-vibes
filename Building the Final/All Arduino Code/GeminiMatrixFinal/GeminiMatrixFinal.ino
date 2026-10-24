      
#include <FastLED.h>

const byte ROWS = 4;
const byte COLS = 8;

//LED SETUP
//
// LED pins on the Mega (one per row)
#define PIN_A 7   
#define PIN_B 8
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


// call this when a button is pressed, e.g. pressPanel("C2", true);
// the first panel in a row lights dim, each new panel in that row brightens the whole row
void pressPanel(const char* name,bool pressed) {
  int row = name[0] - 'A';
  int panel = atoi(name + 1) - 1;
  if (row < 0 || row > 3 || panel < 0 || panel > 7) return; //Code to kick out if Row/Panel not in scope
  //if (panelBrightness[row][panel] >= 4) return;   // Code to check if lights are already on

  // find how bright the rest of the row is
  uint8_t level = 0;
  int countOnLights = 0;
  for (int p = 0; p < 8; p++) {
    if (panelBrightness[row][p] > 0) countOnLights += 1;
    if (panelBrightness[row][p] > level) level = panelBrightness[row][p];
    
  }
  //pressed value refers to whether the panel has been activated. if so, increase brightness;
  //If 4 lights are already active, just light the panel.
  if (countOnLights > 4){
    if (pressed == false){
      lightPanel(name, off);
      }
    else{
      lightPanel(name, level);
      }
    return;
   }

  //Otherwise, dim or increase brightness of the rows as needed.
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
//First digit is 18 and not midiCC 10 because it seems
//midiCC10 mutes tracks by default. 
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

//Sends a Midi packet through LOOPMIDI and EA Serial. Senda message through serial monitor that refers to the midi channel,.
// which elements to send through, and how loud it should be.
void sendMidiCC(byte channel, byte controlNumber, byte value) {
  byte midiPacket[3];
  midiPacket[0] = (byte)(0xB0 | (channel & 0x0F));
  midiPacket[1] = (byte)(controlNumber & 0x7F);
  midiPacket[2] = (byte)(value & 0x7F);
  Serial.write(midiPacket, 3);
}


const int volLoud = 100;
const int volMedium = 67;
const int volQuiet = 30;
const int volOff = 0;

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
             sendMidiCC(0, ccNumbers[rowA][0], volLoud);
            break;

            case 2:
            // 0 is active
            sendMidiCC(0, ccNumbers[rowA][1], volLoud);
            sendMidiCC(0, ccNumbers[rowA][2], volLoud);
            break;

            case 3:
            //1,2 is active
            sendMidiCC(0, ccNumbers[rowA][0], volOff);
            sendMidiCC(0, ccNumbers[rowA][1], volOff);
            sendMidiCC(0, ccNumbers[rowA][2], volOff);
            sendMidiCC(0, ccNumbers[rowA][3], volLoud);
            break;

            case 4: 
            //3 is active
            sendMidiCC(0, ccNumbers[rowA][1], volLoud);
            sendMidiCC(0, ccNumbers[rowA][2], volLoud);
            break;
            
            case 5:
            sendMidiCC(0, ccNumbers[rowA][3], volOff);
            sendMidiCC(0, ccNumbers[rowA][1], volOff);
            sendMidiCC(0, ccNumbers[rowA][2], volOff);
            sendMidiCC(0, ccNumbers[rowA][4], volLoud);
            
            break;
            
            case 6:
            //4 is active
            sendMidiCC(0, ccNumbers[rowA][1], volLoud);
            sendMidiCC(0, ccNumbers[rowA][2], volLoud);
            break;
            
            case 7:
            //4 is STILL active
            //Disable doubling + harmony vocals
            sendMidiCC(0, ccNumbers[rowA][1], volOff);
            sendMidiCC(0, ccNumbers[rowA][2], volOff);

            //Add all tracks
            sendMidiCC(0, ccNumbers[rowA][0], volLoud);
            sendMidiCC(0, ccNumbers[rowA][3], volLoud);
            
            break;

            case 8:
            //Add the rest, 0,3,4 are currently active
            sendMidiCC(0, ccNumbers[rowA][1], volLoud);
            sendMidiCC(0, ccNumbers[rowA][2], volLoud);
            
          }}
          
         else if(r == rowB){
          switch(totalPressed){
            case 1:
            sendMidiCC(0, ccNumbers[rowB][0], volLoud);
            break;

            case 2:
            sendMidiCC(0, ccNumbers[rowB][4], volLoud);
            break;

            case 3:
            sendMidiCC(0, ccNumbers[rowB][1], volMedium);
            break;

            case 4:
            sendMidiCC(0, ccNumbers[rowB][1], volOff);
            sendMidiCC(0, ccNumbers[rowB][2], volLoud);
            break;
            
            case 5:
            //B0, B4, 
            sendMidiCC(0, ccNumbers[rowB][2], volOff);
            sendMidiCC(0, ccNumbers[rowB][1], volMedium);
            sendMidiCC(0, ccNumbers[rowB][3], volMedium);
            break;
            
            case 6:
            // B0, B4
            sendMidiCC(0, ccNumbers[rowB][3], volOff);
            sendMidiCC(0, ccNumbers[rowB][2], volLoud);
            break;
            
            case 7: 
            //B0, B2 active
            sendMidiCC(0, ccNumbers[rowB][4], volOff);
            sendMidiCC(0, ccNumbers[rowB][1], volMedium);
            sendMidiCC(0, ccNumbers[rowB][3], volMedium);
            break;

            case 8:
            sendMidiCC(0, ccNumbers[rowB][4], volLoud);
            break;
            
          }}

          else if(r == rowC){
          switch(totalPressed){
            case 1:
            sendMidiCC(0, ccNumbers[rowC][0], volLoud);
            break;

            case 2:
            sendMidiCC(0, ccNumbers[rowC][0], volOff);
            sendMidiCC(0, ccNumbers[rowC][1], volLoud);
            sendMidiCC(0, ccNumbers[rowC][2], volLoud);
            break;

            case 3:
            //D2 active
            sendMidiCC(0, ccNumbers[rowC][1], volOff);
            sendMidiCC(0, ccNumbers[rowC][3], volLoud);
            break;

            case 4:
            sendMidiCC(0, ccNumbers[rowC][2], volOff);
            sendMidiCC(0, ccNumbers[rowC][3], volOff);
            sendMidiCC(0, ccNumbers[rowC][0], volLoud);
            sendMidiCC(0, ccNumbers[rowC][1], volLoud);
            break;
            
            case 5:
            sendMidiCC(0, ccNumbers[rowC][0], volOff);
            sendMidiCC(0, ccNumbers[rowC][1], volOff);
            sendMidiCC(0, ccNumbers[rowC][3], volLoud);
            sendMidiCC(0, ccNumbers[rowC][4], volLoud);
            break;
            
            case 6:
            sendMidiCC(0, ccNumbers[rowC][3], volOff);
            sendMidiCC(0, ccNumbers[rowC][4], volOff);
            sendMidiCC(0, ccNumbers[rowC][2], volLoud);
            sendMidiCC(0, ccNumbers[rowC][5], volLoud);
            break;
            
            case 7:
            sendMidiCC(0, ccNumbers[rowC][2], volOff);
            sendMidiCC(0, ccNumbers[rowC][5], volOff);
            sendMidiCC(0, ccNumbers[rowC][6], volLoud);
            break;

            case 8:
            sendMidiCC(0, ccNumbers[rowC][6], volOff);
            sendMidiCC(0, ccNumbers[rowC][7], volLoud);
            break;
            
          }}

          else if(r == rowD){
          switch(totalPressed){
            case 1:
            sendMidiCC(0, ccNumbers[rowD][0], volMedium);
            break;

            case 2:
            sendMidiCC(0, ccNumbers[rowD][0], volOff);
            sendMidiCC(0, ccNumbers[rowD][1], volLoud);
            break;

            case 3:
            sendMidiCC(0, ccNumbers[rowD][1], volOff);
            sendMidiCC(0, ccNumbers[rowD][2], volMedium);
            break;

            case 4:
            sendMidiCC(0, ccNumbers[rowD][2], volOff);
            sendMidiCC(0, ccNumbers[rowD][3], volMedium);
            break;
            
            case 5:
            sendMidiCC(0, ccNumbers[rowD][3], volOff);
            sendMidiCC(0, ccNumbers[rowD][0], volLoud);
            break;
            
            case 6:
            sendMidiCC(0, ccNumbers[rowD][0], volOff);
            sendMidiCC(0, ccNumbers[rowD][1], volLoud);
            break;
            
            case 7:
            sendMidiCC(0, ccNumbers[rowD][1], volOff);
            sendMidiCC(0, ccNumbers[rowD][2], volLoud);
            break;

            case 8:
            sendMidiCC(0, ccNumbers[rowD][2], volOff);
            sendMidiCC(0, ccNumbers[rowD][3], volLoud);
            break;
            
          }}
         
          }
        
       
      else if (!currentlyDown && isPressed[r][c]) {
        isPressed[r][c] = false;
        
        int totalPressed = 0;
        for (int panelMidi = 0; panelMidi < 8; panelMidi++){
          if (isPressed[r][panelMidi] == true){
            totalPressed = totalPressed + 1;
          }
        }
        pressPanel(panelNames[r][c],false);
        int rowA = 0;
        int rowB = 1;
        int rowC = 2;
        int rowD = 3;
        //Total Pressed would equal the amount of buttons currently pressed.
        // Therefore case 6 would be from when 7 buttons where initally pressed 
        // and 1 was released, making 6 total
        if(r == rowA){
            switch(totalPressed){
              case 0:
              //If no buttons are pressed. Turn off.
              for (int x = 0; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowA][x], volOff);
              }
              break;
              
              case 1:
              //remove backing and harmony, P1 is active
              sendMidiCC(0, ccNumbers[rowA][1], volOff);
              sendMidiCC(0, ccNumbers[rowA][2], volOff);
              break;

              case 2:
              //remove P2, add P1, harmony and double
              sendMidiCC(0, ccNumbers[rowA][3], volOff);
              sendMidiCC(0, ccNumbers[rowA][0], volLoud);
              sendMidiCC(0, ccNumbers[rowA][1], volLoud);
              sendMidiCC(0, ccNumbers[rowA][2], volLoud);
                
              break;

              case 3:
              //remove backing and harmony, P2 is active
              sendMidiCC(0, ccNumbers[rowA][1], volOff);
              sendMidiCC(0, ccNumbers[rowA][2], volOff);
              
              break;

              case 4:
              //remove P3, add P2, harmony and double.
              sendMidiCC(0, ccNumbers[rowA][4], volOff);
              sendMidiCC(0, ccNumbers[rowA][3], volLoud);
              sendMidiCC(0, ccNumbers[rowA][1], volLoud);
              sendMidiCC(0, ccNumbers[rowA][2], volLoud);
              break;

              case 5:
              //remove backing and harmony, P3 is active
              sendMidiCC(0, ccNumbers[rowA][1], volOff);
              sendMidiCC(0, ccNumbers[rowA][2], volOff);
              break;

              case 6:
              // Remove P1,P2, add harmony and double
              sendMidiCC(0, ccNumbers[rowA][0], volOff);
              sendMidiCC(0, ccNumbers[rowA][3], volOff);
              
              sendMidiCC(0, ccNumbers[rowA][1], volLoud);
              sendMidiCC(0, ccNumbers[rowA][2], volLoud);
              break;

              case 7:
              //Remove backing and harmony
              sendMidiCC(0, ccNumbers[rowA][1], volOff);
              sendMidiCC(0, ccNumbers[rowA][2], volOff);
              break;
            }}

          else if(r == rowB){
            switch(totalPressed){
              case 0:
               for (int x = 0; x < 8; x++){
                sendMidiCC(0, ccNumbers[rowB][x], volOff);
              }
              break;
              
              case 1:
              sendMidiCC(0, ccNumbers[rowB][4], volOff);
              break;

              case 2:
              sendMidiCC(0, ccNumbers[rowB][1], volOff);
              break;

              case 3:
              sendMidiCC(0, ccNumbers[rowB][2], volOff);
              sendMidiCC(0, ccNumbers[rowB][1], volMedium);
              break;

              case 4:
              sendMidiCC(0, ccNumbers[rowB][3], volOff);
              sendMidiCC(0, ccNumbers[rowB][1], volOff);
              sendMidiCC(0, ccNumbers[rowB][2], volLoud);
              break;

              case 5:
              sendMidiCC(0, ccNumbers[rowB][2], volOff);
              sendMidiCC(0, ccNumbers[rowB][3], volMedium);
              break;

              case 6:
              sendMidiCC(0, ccNumbers[rowB][3], volOff);
              sendMidiCC(0, ccNumbers[rowB][4], volLoud);
              break;

              case 7:
              sendMidiCC(0, ccNumbers[rowB][4], volOff);
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
                sendMidiCC(0, ccNumbers[rowC][1], volOff);
                sendMidiCC(0, ccNumbers[rowC][2], volOff);
                sendMidiCC(0, ccNumbers[rowC][0], volLoud);
              break;

              case 2:
                //2 is active
                sendMidiCC(0, ccNumbers[rowC][3], volOff);
                sendMidiCC(0, ccNumbers[rowC][1], volLoud);
              break;

              case 3:
                sendMidiCC(0, ccNumbers[rowC][0], volOff);
                sendMidiCC(0, ccNumbers[rowC][1], volOff);
                sendMidiCC(0, ccNumbers[rowC][2], volLoud);
                sendMidiCC(0, ccNumbers[rowC][3], volLoud);
              break;

              case 4:
                sendMidiCC(0, ccNumbers[rowC][3], volOff);
                sendMidiCC(0, ccNumbers[rowC][4], volOff);
                sendMidiCC(0, ccNumbers[rowC][0], volLoud);
                sendMidiCC(0, ccNumbers[rowC][1], volLoud);
              break;

              case 5:
                sendMidiCC(0, ccNumbers[rowC][2], volOff);
                sendMidiCC(0, ccNumbers[rowC][5], volOff);
                sendMidiCC(0, ccNumbers[rowC][3], volLoud);
                sendMidiCC(0, ccNumbers[rowC][4], volLoud);
              break;

              case 6:
                sendMidiCC(0, ccNumbers[rowC][6], volOff);
                sendMidiCC(0, ccNumbers[rowC][5], volLoud);
                sendMidiCC(0, ccNumbers[rowC][2], volLoud);
              break;

              case 7:
                sendMidiCC(0, ccNumbers[rowC][7], 0);
                sendMidiCC(0, ccNumbers[rowC][6], volLoud);
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
              sendMidiCC(0, ccNumbers[rowD][1], volOff);
              sendMidiCC(0, ccNumbers[rowD][0], volMedium);
              break;

              case 2:
              sendMidiCC(0, ccNumbers[rowD][2], volOff);
              sendMidiCC(0, ccNumbers[rowD][1], volMedium);
              break;

              case 3:
              sendMidiCC(0, ccNumbers[rowD][3], volOff);
              sendMidiCC(0, ccNumbers[rowD][2], volMedium);
              break;

              case 4:
              sendMidiCC(0, ccNumbers[rowD][0], volOff);
              sendMidiCC(0, ccNumbers[rowD][3], volMedium);
              break;

              case 5:
              sendMidiCC(0, ccNumbers[rowD][1], volOff);
              sendMidiCC(0, ccNumbers[rowD][0], volLoud);
              break;

              case 6:
              sendMidiCC(0, ccNumbers[rowD][2], volOff);
              sendMidiCC(0, ccNumbers[rowD][1], volLoud);
              break;

              case 7:
              sendMidiCC(0, ccNumbers[rowD][3], volOff);
              sendMidiCC(0, ccNumbers[rowD][2], volLoud);
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
