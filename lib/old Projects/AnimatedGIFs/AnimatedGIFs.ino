/*
 * Advents Kalender
 * 
 * Module:  adafruit RGB Matrix 32x32
 *          RTC DS1307
 *          SD Card shield
 *          
 * GIF Files are on SD Card 
 * sub Dir Structure on SD Card:
 *  --/--- 1     animated Gif files for day 1
 *    /--- 2     animated Gif files for Day 2
 *    ...
 *    /--- 24    animated Gif Files for Day 24
 *    /--- x     animated Gif Files for other days
 */


// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "Wire.h"

// I2Cdev and DS1307 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "DS1307.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif


#define RGBMATRIX_DO_PIN 6
// Chip select for SD card on the SmartMatrix Shield or Photon
#if defined (ARDUINO)
//#define SD_CS BUILTIN_SDCARD
#define SD_CS 15
#elif defined (SPARK)
#define SD_CS SS
#endif


#define MODE_0_PIN 10
#define MODE_1_PIN 11
#define MODE_2_PIN 12
#define MODE_3_PIN 13





#include <SD.h>
#include "GifDecoder.h"
#include "FilenameFunctions.h"

#define DISPLAY_TIME_SECONDS 10

#define ENABLE_SCROLLING  1

// range 0-255
const int defaultBrightness = 255;
#define MATRIX_WIDTH   32
#define MATRIX_HEIGHT  32

/* template parameters are maxGifWidth, maxGifHeight, lzwMaxBits
 * 
 * The lzwMaxBits value of 12 supports all GIFs, but uses 16kB RAM
 * lzwMaxBits can be set to 10 or 11 for small displays, 12 for large displays
 * All 32x32-pixel GIFs tested work with 11, most work with 10
 */
GifDecoder<MATRIX_WIDTH, MATRIX_HEIGHT ,12 > decoder;

#define GIF_DIRECTORY "/gifs/"
#define GIF_TEST   "/gifs/y"



Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, 4,4, RGBMATRIX_DO_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS    + NEO_MATRIX_PROGRESSIVE +
  NEO_TILE_COLUMNS   + NEO_TILE_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);



DS1307 rtc;

u16_t year;
u8_t month, day, dow, hours, minutes, seconds;





int num_files;

void screenClearCallback(void) {
  matrix.fillScreen(matrix.Color(0,0,0));
}

void updateScreenCallback(void) {
  matrix.show();
}

void drawPixelCallback(int16_t x, int16_t y, u8_t red, u8_t green, u8_t blue) {
  matrix.setPixelColor(x,y,red,green,blue);
}

// Setup method runs once, when the sketch starts
void setup() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println("Terminal started"); 
    Serial.flush();

    Serial.println("setup mode pins"); 
    Serial.flush();

    pinMode(MODE_0_PIN, INPUT);           // set pin to input
    pinMode(MODE_1_PIN, INPUT);           // set pin to input
    pinMode(MODE_2_PIN, INPUT);           // set pin to input
    pinMode(MODE_3_PIN, INPUT);           // set pin to input

    Serial.println("setup decoder"); 
    Serial.flush();
            
    decoder.setScreenClearCallback(screenClearCallback);
    decoder.setUpdateScreenCallback(updateScreenCallback);
    decoder.setDrawPixelCallback(drawPixelCallback);

    decoder.setFileSeekCallback(fileSeekCallback);
    decoder.setFilePositionCallback(filePositionCallback);
    decoder.setFileReadCallback(fileReadCallback);
    decoder.setFileReadBlockCallback(fileReadBlockCallback);

    // Seed the random number generator
    randomSeed(analogRead(14));

    
    // Initialize matrix
    Serial.println("setup matrix"); 
    Serial.flush();
    
    matrix.begin();
    matrix.setTextWrap(false);
    matrix.setBrightness(defaultBrightness);
    matrix.setTextColor(matrix.Color(255,0,0));

    // Clear screen
    matrix.fillScreen(matrix.Color(0,0,0));
    matrix.show();
    

    Serial.println("setup SD card"); 
    Serial.flush();
            
    if(initSdCard(SD_CS) < 0) {
        Serial.println("No SD card");
        while(1);
    }

    // Determine how many animated GIF files exist
    num_files = enumerateGIFFiles(GIF_DIRECTORY, false);

    if(num_files < 0) {
        Serial.println("No gifs directory");
        while(1);
    }

    if(!num_files) {
        Serial.println("Empty gifs directory");
        while(1);
    }

    // initialize I2C
    Serial.println("Initializing I2C devices...");
    rtc.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(rtc.testConnection() ? "DS1307 connection successful" : "DS1307 connection failed");

    rtc.getDateTime24(&year, &month, &day, &hours, &minutes, &seconds);

    // display YYYY-MM-DD hh:mm:ss time
    Serial.print("rtc:\t");
    Serial.print(year); Serial.print("-");
    if (month < 10) Serial.print("0");
    Serial.print(month); Serial.print("-");
    if (day < 10) Serial.print("0");
    Serial.print(day); Serial.print(" ");
    if (hours < 10) Serial.print("0");
    Serial.print(hours); Serial.print(":");
    if (minutes < 10) Serial.print("0");
    Serial.print(minutes); Serial.print(":");
    if (seconds < 10) Serial.print("0");
    Serial.println(seconds);

    
    Serial.println("init done");
    Serial.flush();
}

void modeStandard(){
    static unsigned long futureTime;

    int index = random(num_files);

    if(futureTime < millis()) {
        if (++index >= num_files) {
            index = 0;
        }

        if (openGifFilenameByIndex(GIF_DIRECTORY, index) >= 0) {
            // Can clear screen for new animation here, but this might cause flicker with short animations
            matrix.fillScreen(COLOR_BLACK);

            decoder.startDecoding();

            // Calculate time in the future to terminate animation
            futureTime = millis() + (DISPLAY_TIME_SECONDS * 1000);
        }
    }

    decoder.decodeFrame();
}

void modeTestRun(){
  static int led =0;
  led++;
  y=led/32;
  x=led%32;
  
  matrix.setPixelColor(x,y,0,255,0);

  matrix.show();
}

void loop() {
    int mode_0,mode_1,mode_2,mode_3;
    
    mode_0 = digitalRead(MODE_0_PIN);
    mode_1 = digitalRead(MODE_1_PIN);
    mode_2 = digitalRead(MODE_2_PIN);
    mode_3 = digitalRead(MODE_3_PIN);
    
    if (mode_0 == LOW)
    {
      
    }
    else if (mode_1 == LOW)
    {
      
    }
    else if (mode_2 == LOW)
    {
      
    }
    else if (mode_3 == LOW)
    {
      
    }
    else
    {
      modeStandard()
    }
    
  
} 
