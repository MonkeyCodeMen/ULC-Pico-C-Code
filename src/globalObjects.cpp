#include <Arduino.h>
#include <PinMapping.h>
#include <globalObjects.hpp>



// LED Switch
Led     ledStripe1(PIN_LED_SWITCH_1);
Led     ledStripe2(PIN_LED_SWITCH_2);
LedCtrl    * pLedCtrl1;
LedCtrl    * pLedCtrl2;

// RGB LED
RgbLed  rgbLedStrip1(PIN_RGB1_LED_R,PIN_RGB1_LED_G,PIN_RGB1_LED_B);
RgbLedCtrl * pRgbCtrl1;

// WS2812 LED stripe
#define COUNT_STRIPE_1      250
#define COUNT_STRIPE_2      50
WS2812FX ws2812strip1(COUNT_STRIPE_1, PIN_STRIPE_1 , NEO_GRB  + NEO_KHZ800);
WS2812FX ws2812strip2(COUNT_STRIPE_2, PIN_STRIPE_2 , NEO_GRB  + NEO_KHZ800);
NeoStripeCtrl  * pNeoStripeCtrl1;
NeoStripeCtrl  * pNeoStripeCtrl2;

// WS 2812 LED matrix
Adafruit_NeoMatrix neoMatrix1(
    8, 8, 1,1, 
    PIN_MATRIX_1,
    NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
    NEO_MATRIX_ROWS    + NEO_MATRIX_PROGRESSIVE +
    NEO_TILE_COLUMNS   + NEO_TILE_PROGRESSIVE,
    NEO_GRB            + NEO_KHZ800);
Adafruit_NeoMatrix neoMatrix2(
    8, 8, 4,4, 
    PIN_MATRIX_2,
    NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
    NEO_MATRIX_ROWS    + NEO_MATRIX_PROGRESSIVE +
    NEO_TILE_COLUMNS   + NEO_TILE_PROGRESSIVE,
    NEO_GRB            + NEO_KHZ800);
NeoMatrixCtrl  * pNeoMatrixCtrl1;
NeoMatrixCtrl  * pNeoMatrixCtrl2;

// COM
Com * pCom;



#ifdef WITH_DISPLAY
  #include <TFT_eSPI.h> // Hardware-specific library
  TFT_eSPI * pTFT;       // Invoke custom library
  #define TFT_DIM     int(255*1.0)

  #include <cube.hpp>
  Cube * pCube;
#endif

#ifdef WITH_SD_CARD
  #include <SD.h>
  SDFile root;
  #define PIN_SD_CS 16
  void printDirectory(SDFile dir, int numTabs);
#endif