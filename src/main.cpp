#include <Arduino.h>
#include <Debug.hpp>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>


#include <TFT_eSPI.h> // Hardware-specific library
TFT_eSPI * pTFT;       // Invoke custom library
#define PIN_TFT_LED 17
#define TFT_DIM     int(255*1.0)

#include <cube.hpp>
Cube * pCube;

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

LedCtrl    * pLedCtrl1;
LedCtrl    * pLedCtrl2;
RgbLedCtrl * pRgbCtrl1;
NeoStripeCtrl  * pNeoStripeCtrl1;
NeoStripeCtrl  * pNeoStripeCtrl2;
NeoMatrixCtrl  * pNeoMatrixCtrl1;
NeoMatrixCtrl  * pNeoMatrixCtrl2;



#define PIN_LED_SWITCH_1  	7
#define PIN_LED_SWITCH_2    6
#define PIN_RGB1_LED_R      10
#define PIN_RGB1_LED_G      11
#define PIN_RGB1_LED_B      12
#define PIN_STRIPE_1        18
#define PIN_STRIPE_2        19
#define COUNT_STRIPE_1      100
#define COUNT_STRIPE_2      100
#define PIN_MATRIX_1        20
#define PIN_MATRIX_2        21



#include "Com.hpp"
Com * pCom;


void TestDebug();

bool SetupDebugDone = false;
/***********************************************************************************************************************************/
void setup() {
  Debug::start();

  LOG(F("setup 0: Test functions"));
  TestDebug();


  LOG(F("setup 0:"));
  SetupDebugDone = true;

  //analogWriteFreq(3200);
  //analogWriteRange(255);

  LOG(F("setup 0: Neo matrix"));
  pNeoMatrixCtrl1 = new NeoMatrixCtrl(PIN_MATRIX_1,
  8, 8, 4,4, 
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS    + NEO_MATRIX_PROGRESSIVE +
  NEO_TILE_COLUMNS   + NEO_TILE_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

  pNeoMatrixCtrl2 = new NeoMatrixCtrl(PIN_MATRIX_2,8, 8, 4,4, 
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS    + NEO_MATRIX_PROGRESSIVE +
  NEO_TILE_COLUMNS   + NEO_TILE_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);


  LOG(F("setup 0: Neo stripe"));
  pNeoStripeCtrl1 = new NeoStripeCtrl(COUNT_STRIPE_1, PIN_STRIPE_1 , NEO_GRB  + NEO_KHZ800);
  pNeoStripeCtrl2 = new NeoStripeCtrl(COUNT_STRIPE_2, PIN_STRIPE_2 , NEO_GRB  + NEO_KHZ800);

  LOG(F("setup 0: LED switch"));
  String mode;
  pLedCtrl1 = new LedCtrl(PIN_LED_SWITCH_1);
  pLedCtrl2 = new LedCtrl(PIN_LED_SWITCH_2);
  mode="blink";
  pLedCtrl1->setup(mode);
  pLedCtrl2->setup(mode);
  
  
  LOG(F("setup 0: RGB LED"));
  pRgbCtrl1 = new RgbLedCtrl(PIN_RGB1_LED_R,PIN_RGB1_LED_G,PIN_RGB1_LED_B);
  mode = "breath";
  pRgbCtrl1->setup(mode);  

  LOG(F("setup 0: COM interface"));
  pCom = new Com();
  pCom->setup();

  LOG(F("setup 0: done"));
}

void setup1() {
  while(SetupDebugDone == false);

  LOG(F("setup 1:"));


  LOG(F("setup 1: TFT"));
  pTFT = new TFT_eSPI();
  pTFT->init();
  pTFT->setRotation(1);
  pTFT->fillScreen(TFT_BLACK);
  pinMode(PIN_TFT_LED, OUTPUT);
  analogWrite(PIN_TFT_LED,TFT_DIM);

  LOG(F("setup 1: cube"));
  pCube = new Cube(pTFT);

  LOG(F("setup 1: done"));
}

/***********************************************************************************************************************************/
void loop() {
  static int counter=0;

  pCom->loop();
  switch(counter){
    case 0:   pLedCtrl1->loop();            break;
    case 1:   pLedCtrl2->loop();            break;
    case 2:   pRgbCtrl1->loop();            break;
    case 3:   pNeoStripeCtrl1->loop();      break;
    case 4:   pNeoStripeCtrl2->loop();      break;
    case 5:   pNeoMatrixCtrl1->loop();      break;
    case 6:   pNeoMatrixCtrl1->loop();      break;
    default:  counter = -1;                 break;
  }
  counter++;
}

void loop1(){
  pCube->Render();
  pCube->moveView();
  sleep_ms(10);
}

void TestDebug(){
  char list[]=",";
  StringList object(list,',');
  String res; 
  
  ASSERT(object.isEndReached() == false,"");
  ASSERT(object.getNextListEntry() == String(F("")),"");
  ASSERT(object.isEndReached() == false,"");
  ASSERT(object.getNextListEntry() == String(F("")),"");
  ASSERT(object.isEndReached() == true,"");

  object.rewind();

  ASSERT(object.isEndReached() == false,"");
  ASSERT(object.getNextListEntry() == String(F("")),"");
  ASSERT(object.isEndReached() == false,"");
  ASSERT(object.getNextListEntry() == String(F("")),"");
  ASSERT(object.isEndReached() == true,"");
}
