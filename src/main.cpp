#include <Arduino.h>
#include <Debug.hpp>
#include <helper.hpp>
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


Led     ledStripe1(PIN_LED_SWITCH_1);
Led     ledStripe2(PIN_LED_SWITCH_2);
RgbLed  rgbLedStrip1(PIN_RGB1_LED_R,PIN_RGB1_LED_G,PIN_RGB1_LED_B);
WS2812FX ws2812strip1(COUNT_STRIPE_1, PIN_STRIPE_1 , NEO_GRB  + NEO_KHZ800);
WS2812FX ws2812strip2(COUNT_STRIPE_2, PIN_STRIPE_1 , NEO_GRB  + NEO_KHZ800);
Adafruit_NeoMatrix neoMatrix1(
      PIN_MATRIX_1,8, 8, 4,4, 
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




LedCtrl    * pLedCtrl1;
LedCtrl    * pLedCtrl2;
RgbLedCtrl * pRgbCtrl1;
NeoStripeCtrl  * pNeoStripeCtrl1;
NeoStripeCtrl  * pNeoStripeCtrl2;
NeoMatrixCtrl  * pNeoMatrixCtrl1;
NeoMatrixCtrl  * pNeoMatrixCtrl2;






#include "Com.hpp"
Com * pCom;


void TestDebug();

volatile bool setupStartsecondCore = false;
//bool setupStarted   = false;
/***********************************************************************************************************************************/
void setup() {
  //setupStarted = true;
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  LOG(F("setup 0:"));
  //analogWriteFreq(3200);
  //analogWriteRange(255);

  LOG(F("setup 0: Test functions"));
  TestDebug();

  LOG(F("setup 0: LED switch"));
  pLedCtrl1 = new LedCtrl(&ledStripe1);
  pLedCtrl2 = new LedCtrl(&ledStripe2);
  pLedCtrl1->setup(F_CONST("blink"));
  pLedCtrl2->setup(F_CONST("blink"));
  
  
  LOG(F("setup 0: RGB LED"));
  pRgbCtrl1 = new RgbLedCtrl(&rgbLedStrip1);
  pRgbCtrl1->setup(F_CONST("breath"));  

  LOG(F("setup 0: Neo stripe"));
  pNeoStripeCtrl1 = new NeoStripeCtrl(&ws2812strip1);
  pNeoStripeCtrl2 = new NeoStripeCtrl(&ws2812strip2);


  LOG(F("setup 0: Neo matrix"));
  pNeoMatrixCtrl1 = new NeoMatrixCtrl(&neoMatrix1);
  pNeoMatrixCtrl2 = new NeoMatrixCtrl(&neoMatrix2);
  
  setupStartsecondCore = true;

  LOG(F("setup 0: COM interface"));
  pCom = new Com();
  pCom->setup();

  LOG(F("setup 0: done"));
  digitalWrite(LED_BUILTIN, LOW);
}

void setup1() {
  while(setupStartsecondCore == false){

  }

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
  static u32_t counter=1;
  static u32_t lastCycle=0;
  u32_t time;

  time = millis();

  if (time-lastCycle < 250) 
    { digitalWrite(LED_BUILTIN, LOW);}
  else if (time-lastCycle < 500) 
    { digitalWrite(LED_BUILTIN, HIGH);  }
  else 
    { lastCycle = time;}

  pCom->loop();
  switch(counter){
    case 1:   pLedCtrl1->loop(time);            break;
    case 2:   pLedCtrl2->loop(time);            break;
    //case 3:   pRgbCtrl1->loop(time);            break;
    //case 4:   pNeoStripeCtrl1->loop(time);      break;
    //case 5:   pNeoStripeCtrl2->loop(time);      break;
    //case 6:   pNeoMatrixCtrl1->loop(time);      break;
    //case 7:   pNeoMatrixCtrl2->loop(time);      break;
    default:  counter = 0;                      break;
  }
  counter++;
}

void loop1(){
  pCube->Render();
  pCube->moveView();
  sleep_ms(10);
}

void TestDebug(){
  ledStripe1.set(0);
  ledStripe1.set(128);
  ledStripe1.set(255);
  ledStripe1.set(0);

  ledStripe2.set(0);
  ledStripe2.set(128);
  ledStripe2.set(255);
  ledStripe2.set(0);
  

}
