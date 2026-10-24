#include <FastLED.h>

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

// current brightness of each panel, 0 = off
uint8_t panelBrightness[4][8];

int delayTime = 0;

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


void setup(){
  setupLEDs();
  Serial.begin(9600);
  
}

void loop(){
  pressPanel("A1",true);
  delay(delayTime);
  pressPanel("A2",true);
  delay(delayTime);
  pressPanel("A3",true);
  delay(delayTime);
  pressPanel("A4",true);
  delay(delayTime);
  pressPanel("A5",true);
  delay(delayTime);
  pressPanel("A6",true);
  delay(delayTime);
  pressPanel("A7",true);
  delay(delayTime);
  pressPanel("A8",true);
  delay(delayTime);
//  pressPanel("A1",false);
//  delay(delayTime);
//  pressPanel("A2",false);
//  delay(delayTime);
//  pressPanel("A3",false);
//  delay(delayTime);
//  pressPanel("A4",false);
//  delay(delayTime);
//  pressPanel("A5",false);
//  delay(delayTime);
//  pressPanel("A6",false);
//  delay(delayTime);
//  pressPanel("A7",false);
//  delay(delayTime);
//  pressPanel("A8",false);
//  delay(delayTime);

//  
  pressPanel("B1",true);
  delay(delayTime);
  pressPanel("B2",true);
  delay(delayTime);
  pressPanel("B3",true);
  delay(delayTime);
  pressPanel("B4",true);
  delay(delayTime);
  pressPanel("B5",true);
  delay(delayTime);
  pressPanel("B6",true);
  delay(delayTime);
  pressPanel("B7",true);
  delay(delayTime);
  pressPanel("B8",true);
  delay(delayTime);
//  pressPanel("B1",false);
//  delay(delayTime);
//  pressPanel("B2",false);
//  delay(delayTime);
//  pressPanel("B3",false);
//  delay(delayTime);
//  pressPanel("B4",false);
//  delay(delayTime);
//  pressPanel("B5",false);
//  delay(delayTime);
//  pressPanel("B6",false);
//  delay(delayTime);
//  pressPanel("B7",false);
//  delay(delayTime);
//  pressPanel("B8",false);
//  delay(delayTime);
//
//
//  
  pressPanel("C1",true);
  delay(delayTime);
  pressPanel("C2",true);
  delay(delayTime);
  pressPanel("C3",true);
  delay(delayTime);
  pressPanel("C4",true);
  delay(delayTime);
  pressPanel("C5",true);
  delay(delayTime);
  pressPanel("C6",true);
  delay(delayTime);
  pressPanel("C7",true);
  delay(delayTime);
  pressPanel("C8",true);
  delay(delayTime);

  
  pressPanel("D1",true);
  delay(delayTime);
  pressPanel("D2",true);
  delay(delayTime);
  pressPanel("D3",true);
  delay(delayTime);
  pressPanel("D4",true);
  delay(delayTime);
  pressPanel("D5",true);
  delay(delayTime);
  pressPanel("D6",true);
  delay(delayTime);
  pressPanel("D7",true);
  delay(delayTime);
  pressPanel("D8",true);
  delay(delayTime);
//  pressPanel("D1",false);
//  delay(delayTime);
//  pressPanel("D2",false);
//  delay(delayTime);
//  pressPanel("D3",false);
//  delay(delayTime);
//  pressPanel("D4",false);
//  delay(delayTime);
//  pressPanel("D5",false);
//  delay(delayTime);
//  pressPanel("D6",false);
//  delay(delayTime);
//  pressPanel("D7",false);
//  delay(delayTime);
//  pressPanel("D8",false);
//  delay(delayTime);
};
