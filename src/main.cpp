#include <Arduino.h>
#include <Debug.hpp>
#include <helper.hpp>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>


//#define WITH_DISPLAY
//#define WITH_SD_CARD

#ifdef WITH_DISPLAY
  #include <TFT_eSPI.h> // Hardware-specific library
  TFT_eSPI * pTFT;       // Invoke custom library
  #define PIN_TFT_LED 17
  #define TFT_DIM     int(255*1.0)

  #include <cube.hpp>
  Cube * pCube;
#endif



#include <WS2812FX.h>
#define LED_COUNT 30
#define PIN_LED_WS_1 18



// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
WS2812FX * pLedNeoStripe; 

#include <RgbLedCtrl.hpp>
#include <LedCtrl.hpp>
#include <NeoStripeCtrl.hpp>
#include <NeoMatrixCtrl.hpp>

#define PIN_LED_SWITCH_1  	7
#define PIN_LED_SWITCH_2    6
#define PIN_RGB1_LED_R      10
#define PIN_RGB1_LED_G      11
#define PIN_RGB1_LED_B      12
#define PIN_STRIPE_1        18
#define PIN_STRIPE_2        19
#define COUNT_STRIPE_1      250
#define COUNT_STRIPE_2      250
#define PIN_MATRIX_1        20
#define PIN_MATRIX_2        21


// LED objects

// LED Switch
Led     ledStripe1(PIN_LED_SWITCH_1);
Led     ledStripe2(PIN_LED_SWITCH_2);
// RGB LED
RgbLed  rgbLedStrip1(PIN_RGB1_LED_R,PIN_RGB1_LED_G,PIN_RGB1_LED_B);
// WS2812 LED stripe
WS2812FX ws2812strip1(COUNT_STRIPE_1, PIN_STRIPE_1 , NEO_GRB  + NEO_KHZ800);
WS2812FX ws2812strip2(COUNT_STRIPE_2, PIN_STRIPE_2 , NEO_GRB  + NEO_KHZ800);
// WS 2812 LED matrix
Adafruit_NeoMatrix neoMatrix1(
      PIN_MATRIX_1,8, 8, 1,1, 
      NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
      NEO_MATRIX_ROWS    + NEO_MATRIX_PROGRESSIVE +
      NEO_TILE_COLUMNS   + NEO_TILE_PROGRESSIVE,
      NEO_GRB            + NEO_KHZ800);
Adafruit_NeoMatrix neoMatrix2(
      PIN_MATRIX_2,8, 8, 4,4, 
      NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
      NEO_MATRIX_ROWS    + NEO_MATRIX_PROGRESSIVE +
      NEO_TILE_COLUMNS   + NEO_TILE_PROGRESSIVE,
      NEO_GRB            + NEO_KHZ800);

// control objects for LED's 
LedCtrl    * pLedCtrl1;
LedCtrl    * pLedCtrl2;
RgbLedCtrl * pRgbCtrl1;
NeoStripeCtrl  * pNeoStripeCtrl1;
NeoStripeCtrl  * pNeoStripeCtrl2;
NeoMatrixCtrl  * pNeoMatrixCtrl1;
NeoMatrixCtrl  * pNeoMatrixCtrl2;

#include "Com.hpp"
Com * pCom;


#ifdef WITH_SD_CARD
  #include <SD.h>
  SDFile root;
  #define PIN_SD_CS 16
  void printDirectory(SDFile dir, int numTabs);
#endif


void TestDebug();

volatile bool setupStartsecondCore = false;
volatile bool waitForsecondCore    = true;
//bool setupStarted   = false;
/***********************************************************************************************************************************/
void setup() {
  //setupStarted = true;
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  LOG(F_CONST("setup 0:"));
  //analogWriteFreq(3200);
  //analogWriteRange(255);

  LOG(F_CONST("setup 0: Test functions"));
  TestDebug();



  LOG(F_CONST("setup 0: LED switch"));
  pLedCtrl1 = new LedCtrl(&ledStripe1);
  pLedCtrl2 = new LedCtrl(&ledStripe2);
  pLedCtrl1->setup(F_CONST("blink"));
  pLedCtrl1->setup(0xFF,250,250,0,0,NULL);
  pLedCtrl2->setup(F_CONST("blink"));
  pLedCtrl2->setup(0xFF,250,250,0,0,NULL);
  
  
  LOG(F_CONST("setup 0: RGB LED"));
  pRgbCtrl1 = new RgbLedCtrl(&rgbLedStrip1);
  pRgbCtrl1->setup(F_CONST("multi flash"));  

  LOG(F_CONST("setup 0: Neo stripe"));
  pNeoStripeCtrl1 = new NeoStripeCtrl(&ws2812strip1);
  pNeoStripeCtrl2 = new NeoStripeCtrl(&ws2812strip2);
  pNeoStripeCtrl1->setup(13);  
  pNeoStripeCtrl1->setup(13);  


  LOG(F_CONST("setup 0: Neo matrix"));
  pNeoMatrixCtrl1 = new NeoMatrixCtrl(&neoMatrix1);
  pNeoMatrixCtrl2 = new NeoMatrixCtrl(&neoMatrix2);
  


  setupStartsecondCore = true;
  while(waitForsecondCore == true){
  }

  LOG(F_CONST("setup 0: done"));
  digitalWrite(LED_BUILTIN, LOW);
}

void setup1() {
  while(setupStartsecondCore == false){
  }

  LOG(F_CONST("setup 1:"));

  LOG(F_CONST("setup 1: COM interface"));
  pCom = new Com();
  pCom->setup();

  #ifdef WITH_DISPLAY
    LOG(F_CONST("setup 1: TFT"));
    pTFT = new TFT_eSPI();
    pTFT->init();
    pTFT->setRotation(1);
    pTFT->fillScreen(TFT_BLACK);
    pinMode(PIN_TFT_LED, OUTPUT);
    analogWrite(PIN_TFT_LED,TFT_DIM);

    LOG(F_CONST("setup 1: cube"));
    pCube = new Cube(pTFT);
  #endif

  #ifdef WITH_SD_CARD
    LOG(F_CONST("setup 1: Initializing SD card..."));
    if (!SD.begin(4)) {
      LOG(F_CONST("setup 1: SD card initialization failed!"));
      while (1);
    }
    LOG(F_CONST("setup 1: SD card initialization done."));
    root = SD.open("/");
    printDirectory(root, 0);
  #endif
 
  LOG(F_CONST("setup 1: done"));
}

/***********************************************************************************************************************************/
void loop() {
  static u8_t prgState=1;
  static u8_t ledState=0;
  static u32_t lastSwitch=0;
  u32_t now = millis();
  u32_t diff = now-lastSwitch;

  switch(ledState){
    case 0:   if (diff >= 250){
                  digitalWrite(LED_BUILTIN, HIGH);
                  lastSwitch = now;
                  ledState = 1;
                  }
              break;
    case 1:   if (diff >= 250){
                  digitalWrite(LED_BUILTIN, LOW);
                  lastSwitch = now;
                  ledState = 0;
                  }
              break;
    default:  LOG(F_CONST("loop 0 unknown ledState"));
              digitalWrite(LED_BUILTIN, LOW);
              lastSwitch = now;
              ledState = 0;
              break;
  }


  switch(prgState){
    case 1:   pLedCtrl1->loop(now);            break;
    case 2:   pLedCtrl2->loop(now);            break;
    case 3:   pRgbCtrl1->loop(now);            break;
    case 4:   pNeoStripeCtrl1->loop(now);      break;
    case 5:   pNeoStripeCtrl2->loop(now);      break;
    case 6:   pNeoMatrixCtrl1->loop(now);      break;
    case 7:   pNeoMatrixCtrl2->loop(now);      break;
    default:  prgState = 0;                      break;
  }
  prgState++;
}

void loop1(){
  static u32_t lastCycle=0;
  u32_t now = millis();

  if (now-lastCycle > 25){
    #ifdef WITH_DISPLAY
      pCube->Render();
      pCube->moveView();
    #endif
    lastCycle = now;
  }
  pCom->loop();
}

void TestDebug(){
  // place code to debug here (single core before startup)
}


 
#ifdef WITH_SD_CARD
  void printDirectory(SDFile dir, int numTabs) {
    while (true) {

      SDFile entry =  dir.openNextFile();
      if (! entry) {
        // no more files
        break;
      }
      for (uint8_t i = 0; i < numTabs; i++) {
        Serial.print('\t');
      }
      LOG(entry.name());
      if (entry.isDirectory()) {
        LOG("/");
        printDirectory(entry, numTabs + 1);
      } else {
        // files have sizes, directories do not
        LOG("\t\t");
        LOG(String(entry.size()).c_str());
      }
      entry.close();
    }
  }
#endif