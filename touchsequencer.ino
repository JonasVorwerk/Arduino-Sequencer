/*
A basic plug and play step sequencer using:

Music Maker FeatherWing
https://www.adafruit.com/product/3357

TFT FeatherWing - 2.4" 320x240 Touchscreen 
https://www.adafruit.com/product/3315

Feather 32u4 Basic Proto, 
but I guess any other Feather will work
https://www.adafruit.com/product/2771

Close the midi jumper on the bottom of the 
music featherwing and upload the sketch and
guaranteed hours of pleasure! 

23-08-2019 https://jonasvorwerk.nl

Changes:

23-08-2019
- Pattern display optimised, now runs smoother playing rhythms

22-08-2019
- Patterns added! Performance on 32u4 is not making me happy
  but you could disable some display features line SHOWSEQUENCE, 
  SHOWPATTERNS and SHOWGRID to improve timing

09-08-2019
- Now posible to change instuments using the INSTR button
- Change Sequencer size
- Separate midi channel for each track

*/

#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <Adafruit_STMPE610.h>

#ifdef ESP8266
   #define STMPE_CS 16
   #define TFT_CS   0
   #define TFT_DC   15
   #define SD_CS    2
#endif
#ifdef ESP32
   #define STMPE_CS 32
   #define TFT_CS   15
   #define TFT_DC   33
   #define SD_CS    14
#endif
#ifdef TEENSYDUINO
   #define TFT_DC   10
   #define TFT_CS   4
   #define STMPE_CS 3
   #define SD_CS    8
#endif
#ifdef ARDUINO_STM32_FEATHER
   #define TFT_DC   PB4
   #define TFT_CS   PA15
   #define STMPE_CS PC7
   #define SD_CS    PC5
#endif
#ifdef ARDUINO_NRF52832_FEATHER /* BSP 0.6.5 and higher! */
   #define TFT_DC   11
   #define TFT_CS   31
   #define STMPE_CS 30
   #define SD_CS    27
#endif
#if defined(ARDUINO_MAX32620FTHR) || defined(ARDUINO_MAX32630FTHR)
   #define TFT_DC   P5_4
   #define TFT_CS   P5_3
   #define STMPE_CS P3_3
   #define SD_CS    P3_2
#endif

// Anything else!
#if defined (__AVR_ATmega32U4__) || defined(ARDUINO_SAMD_FEATHER_M0) || \ 
    defined (__AVR_ATmega328P__) || defined(ARDUINO_SAMD_ZERO) || \ 
    defined(__SAMD51__) || defined(__SAM3X8E__) || defined(ARDUINO_NRF52840_FEATHER)
   #define STMPE_CS 6
   #define TFT_CS   9
   #define TFT_DC   10
   #define SD_CS    5
#endif


Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 3800
#define TS_MAXX 100
#define TS_MINY 100
#define TS_MAXY 3750

//AUDIO VS1053_MIDI
// Solder closed jumper on bottom!

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 31
#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79

#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_PROGRAM 0xC0
#if defined(__AVR_ATmega32U4__) || defined(ARDUINO_SAMD_FEATHER_M0) || \
defined(TEENSYDUINO) || defined(ARDUINO_STM32_FEATHER)
  #define VS1053_MIDI Serial1
#elif defined(ESP32)
  HardwareSerial Serial1(2);
  #define VS1053_MIDI Serial1
#elif defined(ESP8266)
  #define VS1053_MIDI Serial
#endif

//SEQUENCER LARGE
#define COLS 16
#define ROWS 8
#define BOXSIZE 20
#define BPMDIVIDER 4

//SEQUENCER SMALL
//#define COLS 8
//#define ROWS 5
//#define BOXSIZE 0
//#define BPMDIVIDER 2

#define DEBUG 0
#define COLOR 0xFFFF
#define BGCOLOR 0x0000
#define TOUCHDELAY 150
#define PATTR 4
#define PATPLAY 1

#define SHOWSEQUENCE 1
#define SHOWPATTERNS 1
#define SHOWGRID 0

bool power = true;
bool instrMode = false;
int bpm = 140;
uint8_t curstep = 0; 
uint8_t curpattern = 0; 

typedef struct trackstructure{
  uint8_t note;
  bool patterns[PATTR][COLS]; //one extra just to make sure it keeps working
};

trackstructure track[ROWS+1]; //one extra

//SEQUENCER timer
long previousMillis = 0;
long previousMillisButton = 0;

void setup(void) {
  //while (!Serial); 
  Serial.begin(115200);

  delay(10);
  Serial.println("FeatherWing TFT");
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  Serial.println("Touchscreen started");
  
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);

  //enable midi
  VS1053_MIDI.begin(31250); // MIDI uses a 'strange baud rate'
  delay(100);
  
  //separate midi channel for each track
  for (uint8_t r = 0; r < ROWS; r++) {
    midiSetChannelBank(r, VS1053_BANK_DRUMS1);
    midiSetChannelVolume(r, 127);
    midiSetInstrument(r, 5);
  }
  
  delay(100);

  //define track info note and patterns
  //make sure you assign the same amount of patterns as your PATTR
  
  track[0] = { 36, {
    { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0 },
    { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0 },
    { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0 },
    { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0 },
  }};
  track[1] = { 40, {
    { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
    { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
    { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
    { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  }};
  track[2] = { 42, {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1 },
    { 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1 },
    { 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1 },
  }};
  track[3] = { 51, {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1 },
    { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
  }};
  track[4] = { 37, {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  }};
  track[5] = { 38, {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  }};
  track[6] = { 75, {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  }};
  track[7] = { 76, {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
  }};

// default empty tracks with just notes assigned
//  track[0] = { 36, {{}} };
//  track[1] = { 40, {{}} };
//  track[2] = { 42, {{}} };
//  track[3] = { 51, {{}} };
//  track[4] = { 37, {{}} };
//  track[5] = { 76, {{}} };
//  track[6] = { 77, {{}} };
//  track[7] = { 39, {{}} };

  showMenu();

  //show sequence
  showSequence(true);
  //showFirstSequence();

  //draw grid lines
  drawGrid();

  //draw pattern buttons
  drawPatterns();

  delay(500);
}

void loop() {
  
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > (60000 / bpm)/BPMDIVIDER && power) {
    previousMillis = currentMillis;

    //clear step indicators
    for (int c = 0; c < COLS; c = c+1) { 
      tft.drawRect(0, BOXSIZE*c, BOXSIZE*ROWS, BOXSIZE, 0x0000);
    }
    //show current step inducator
    tft.drawRect(0, BOXSIZE*curstep, BOXSIZE*ROWS, BOXSIZE, 0xFFFF); 

    //play note
    for (uint8_t r = 0; r < ROWS+1; r++) {
      if (track[r].patterns[curpattern][curstep]) {         
        midiNoteOn(r, track[r].note, 127);
      } else {
        midiNoteOff(r, track[r].note, 0);
      }
    }

    //increase step amount
    curstep++;
    if (curstep >= COLS) {
      curstep = 0;

      //update pattern only if there are any and if PATPLAY is enabled
      if(PATTR > 1 && PATPLAY){
        if(curpattern < PATTR-1){
          curpattern++;
        } else {
          curpattern = 0;
        }
        drawPatterns();
        showSequence(false);
      }
    }
  }

  //clear touchscreen buffer
  if (ts.bufferEmpty()) {
    return;
  }

  //retrieve a touchscreen point  
  TS_Point p = ts.getPoint();
 
  //scale from ~0->4000 to tft.width using the calibration #'s
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

  if (ts.touched() && p.x < (ROWS * BOXSIZE) && (currentMillis - previousMillisButton > TOUCHDELAY)){

    previousMillisButton = currentMillis;

    uint8_t r = int(p.x)/BOXSIZE;
    uint8_t c = int(p.y)/BOXSIZE;

    if(!instrMode){
      if(track[r].patterns[curpattern][c]) {
        track[r].patterns[curpattern][c] = false;
        tft.fillRect(BOXSIZE*r, BOXSIZE*c, BOXSIZE, BOXSIZE, BGCOLOR);
      } else {
        track[r].patterns[curpattern][c] = true;
        tft.fillRect(BOXSIZE*r, BOXSIZE*c, BOXSIZE, BOXSIZE, COLOR);
      }
    } else {
      //set note (for drumkit its the instument)
      uint8_t note = map(p.y, 0, tft.height(), 27, 87);

      Serial.print("track:");
      Serial.print(r);
      Serial.print(" ");
      Serial.print("note:");
      Serial.println(note);

      //clear all notes before playing a new one
      for(uint8_t i; i<=87; i++){
        midiNoteOff(r, i, 127);
      }

      //play preview note (instrument)
      midiNoteOn(r, note, 127);

      //set note
      track[r].note = note;

      //draw rectangle for visual feedback
      tft.fillRect(BOXSIZE*r, 0, BOXSIZE, tft.height(), 0x0000);
      tft.fillRect(BOXSIZE*r, 0, BOXSIZE, map(note, 27, 87, 0, tft.height()), COLOR);
    }
  }

  //if touched in upper bar area
  if(ts.touched() && p.x > 200 && (currentMillis - previousMillisButton > TOUCHDELAY)){

    previousMillisButton = currentMillis; //reset button timer
    //Serial.println(p.y);

    //bpm decrease
    if (p.y > 0 && p.y < 55){
      if(bpm > 30){
        bpm = bpm - 10;
        tft.fillRect(200, 55, 30, 50, 0x0000);
        tft.setRotation(1);   
        tft.setCursor(60, 10); 
        tft.print(bpm);
        tft.setRotation(0);
      }
    }

    //bpm increase
    if (p.y > 55 && p.y < 110){
      if(bpm < 400){
        bpm = bpm + 10;
        tft.fillRect(200, 55, 30, 50, 0x0000);
        tft.setRotation(1);   
        tft.setCursor(60, 10); 
        tft.print(bpm);
        tft.setRotation(0);
      }
    }

    //instruments panel
    if (p.y > 110 && p.y < 170){
      
      if(!instrMode){  
            
        power = false; 
        instrMode = true;
        tft.fillScreen(ILI9341_BLACK);
        showMenu();
        for (uint8_t r = 0; r < ROWS; r++) {
          tft.fillRect(BOXSIZE*r, 0, BOXSIZE, map(track[r].note, 27, 87, 0, tft.height()), COLOR);
        }

      } else {  
         
        instrMode = false;   
        tft.fillScreen(ILI9341_BLACK);
        showMenu();
        drawGrid();
        showSequence(true);
              
      }
    }
    
    //power on / off
    if (p.y > 180 && p.y < 220){
      tft.fillRect(200, 175, 40, 65, 0x0000);

      if(instrMode){     
        drawGrid();
        showSequence(true);
        instrMode = false;
      }
      
      if(power){
        power = false;  
        tft.setRotation(1);   
        tft.setCursor(185, 10); 
        tft.print("PLAY");
        tft.setRotation(0);
      } else {
        power = true;
        tft.setRotation(1);   
        tft.setCursor(185, 10); 
        tft.print("STOP");
        tft.setRotation(0);    
      }
    } 

    //clear all tracks
    if (p.y > 240 && p.y < 320){
      clearSequence();
    }

  }
  
}

void showMenu(){
  tft.setRotation(1);
  tft.setTextColor(0xFFFF);  
  tft.setTextSize(2);
  tft.setCursor(10, 10); 
  tft.print("BPM:");
  tft.setCursor(60, 10); 
  tft.print(bpm);
  tft.setCursor(110, 10); 
  tft.print("INSTR"); 
  tft.setCursor(185, 10);
  if(power){
    tft.print("STOP");
  } else {
    tft.print("PLAY");
  }
  tft.setCursor(250, 10); 
  tft.print("CLEAR");  
  tft.setRotation(0);
}

void drawPatterns(){
  //delay(100); // small hack to make sure the pattern is ready and the 1st notes are playing
  if(PATTR > 1 && SHOWPATTERNS){
    for (uint8_t p = 0; p < PATTR; p++) {
      if(curpattern == p){
        tft.fillRect( tft.width()-55, tft.height()/PATTR*p, BOXSIZE, tft.height()/PATTR,COLOR);
      } else {
        tft.fillRect( tft.width()-55, tft.height()/PATTR*p, BOXSIZE, tft.height()/PATTR,BGCOLOR);
        //tft.drawRect( tft.width()-55, tft.height()/PATTR*p, BOXSIZE, tft.height()/PATTR,COLOR);
      }
    }
  }
}

void drawGrid(){
  if(SHOWGRID){
    for (uint8_t c = 1; c < COLS; c++) { //not start at 0 because we don't need the first line
        if (c % 4 == 0) {
          tft.drawLine(0, BOXSIZE*c, 205, BOXSIZE*c, COLOR);
        }
    }
  }
}

void showSequence(bool redrawAll){

  int prevpattern = curpattern -1;
  if(prevpattern < 0) prevpattern = PATTR-1;
        
  if(SHOWSEQUENCE){
  
    for (uint8_t r = 0; r < ROWS; r++) {     
      for (uint8_t c = 0; c < COLS; c++) { 

        //only change if different so the screen doesn't take to long to opdate
        if(track[r].patterns[prevpattern][c] != track[r].patterns[curpattern][c] || redrawAll){  
               
          if(track[r].patterns[curpattern][c]){
            tft.fillRect(BOXSIZE*r, BOXSIZE*c, BOXSIZE, BOXSIZE, COLOR);
          } else {
            tft.fillRect(BOXSIZE*r, BOXSIZE*c, BOXSIZE, BOXSIZE, BGCOLOR);
          }
          
        } 
         
      }
    }
    
  }
}

void clearSequence(){
  for (uint8_t r = 0; r < ROWS; r++) {     
    for (uint8_t c = 0; c < COLS; c++) { 
      track[r].patterns[curpattern][c] = false;
      tft.fillRect(BOXSIZE*r, BOXSIZE*c, BOXSIZE, BOXSIZE, BGCOLOR);
    }
  }
}

void midiSetInstrument(uint8_t chan, uint8_t inst) {
  if (chan > 15) return;
  inst --; // page 32 has instruments starting with 1 not 0 :(
  if (inst > 127) return;
  
  VS1053_MIDI.write(MIDI_CHAN_PROGRAM | chan);  
  delay(10);
  VS1053_MIDI.write(inst);
  delay(10);
}

void midiSetChannelVolume(uint8_t chan, uint8_t vol) {
  if (chan > 15) return;
  if (vol > 127) return;
  
  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write(MIDI_CHAN_VOLUME);
  VS1053_MIDI.write(vol);
}

void midiSetChannelBank(uint8_t chan, uint8_t bank) {
  if (chan > 15) return;
  if (bank > 127) return;
  
  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write((uint8_t)MIDI_CHAN_BANK);
  VS1053_MIDI.write(bank);
}

void midiNoteOn(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;
  
  VS1053_MIDI.write(MIDI_NOTE_ON | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

void midiNoteOff(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;
  
  VS1053_MIDI.write(MIDI_NOTE_OFF | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}
