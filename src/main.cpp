#include <Arduino.h>
#include <Debug.hpp>
#include <SPI.h>


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

LedCtrl    * pLedCtrl1;
LedCtrl    * pLedCtrl2;
RgbLedCtrl * pRgbCtrl1;
NeoStripeCtrl  * pNeoStripeCtrl1;
NeoStripeCtrl  * pNeoStripeCtrl2;



#define PIN_LED_SWITCH_1  	7
#define PIN_LED_SWITCH_2    6
#define PIN_RGB1_LED_R      10
#define PIN_RGB1_LED_G      11
#define PIN_RGB1_LED_B      12
#define PIN_STRIPE_1        18
#define PIN_STRIPE_2        19
#define COUNT_STRIPE_1      100
#define COUNT_STRIPE_2      100



#include "Com.hpp"
Com * pCom;


void NeoStripeLoop();

bool SetupDebugDone = false;
/***********************************************************************************************************************************/
void setup() {
  Debug::start();
  LOG("setup 0:");
  SetupDebugDone = true;

  //analogWriteFreq(3200);
  //analogWriteRange(255);

  LOG("setup 0: Neo Stripe");
  pNeoStripeCtrl1 = new NeoStripeCtrl(COUNT_STRIPE_1, PIN_STRIPE_1 , NEO_GRB  + NEO_KHZ800);
  pNeoStripeCtrl2 = new NeoStripeCtrl(COUNT_STRIPE_2, PIN_STRIPE_2 , NEO_GRB  + NEO_KHZ800);

  LOG("setup 0: LedSwitch");
  String mode;
  pLedCtrl1 = new LedCtrl(PIN_LED_SWITCH_1);
  pLedCtrl2 = new LedCtrl(PIN_LED_SWITCH_2);
  mode="blink";
  pLedCtrl1->setup(mode);
  pLedCtrl2->setup(mode);
  
  
  LOG("setup 0: RgbLedSwitch");
  pRgbCtrl1 = new RgbLedCtrl(PIN_RGB1_LED_R,PIN_RGB1_LED_G,PIN_RGB1_LED_B);
  mode = "breath";
  pRgbCtrl1->setup(mode);  

  LOG("setup 0: COM interface");
  pCom = new Com();
  pCom->setup();

  LOG("setup 0: done");
}

void setup1() {
  while(SetupDebugDone == false);

  LOG("setup 1:");


  LOG("setup 1: TFT");
  pTFT = new TFT_eSPI();
  pTFT->init();
  pTFT->setRotation(1);
  pTFT->fillScreen(TFT_BLACK);
  pinMode(PIN_TFT_LED, OUTPUT);
  analogWrite(PIN_TFT_LED,TFT_DIM);

  LOG("setup 1: cube");
  pCube = new Cube(pTFT);


  LOG("setup 1: done");
}

/***********************************************************************************************************************************/
void loop() {
  static u32_t counter=0;

  pCom->loop();
  switch(counter){
    case 0:   pLedCtrl1->loop();    break;
    case 1:   pLedCtrl2->loop();    break;
    case 2:   pRgbCtrl1->loop();    break;
    case 5:   NeoStripeLoop();      break;
    case 100: counter = 0;          break;
    default:
          // do nothing
          break;
  }
  counter++;
}

void loop1(){
  pCube->Render();
  pCube->moveView();
  sleep_ms(10);
}


void NeoStripeLoop(){
  static u32_t last_change = 0;
  u32_t now,diff;
  #define TIMER_MS 5000

  now = millis();
  diff = now-last_change;

  // mode change ?
  if(diff > TIMER_MS) {
    pLedNeoStripe->setMode((pLedNeoStripe->getMode() + 1) % pLedNeoStripe->getModeCount());
    last_change = now;
    String name(pLedNeoStripe->getModeName(pLedNeoStripe->getMode()));
    String line = "new mode :"+name;
    LOG(line.c_str());
  }

  // loop 
  pLedNeoStripe->service();
}
