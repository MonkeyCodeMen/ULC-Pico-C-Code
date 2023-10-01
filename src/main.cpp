#include <Arduino.h>




/*


 Blaulicht Doppelblitz: 500ms, ~25ms An, ~75ms Aus, ~25ms An (Aus Diagramm oben abgelesen)

 
 */

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
#define TIMER_MS 5000

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
WS2812FX * pLedNeoStripe; 
unsigned long last_change = 0;
unsigned long now = 0;


#include <RgbLedCtrl.hpp>
#include <LedCtrl.hpp>

LedCtrl    * pLedCtrl1;
LedCtrl    * pLedCtrl2;
RgbLedCtrl * pRgbCtrl1;


#define PIN_LED_SWITCH_1  7
#define PIN_LED_SWITCH_2  6
#define PIN_RGB1_LED_R     10
#define PIN_RGB1_LED_G     11
#define PIN_RGB1_LED_B     12


#include "Com.hpp"
Com * pCom;




/***********************************************************************************************************************************/
void setup() {
  Debug::start();
  LOG("setup");

  //analogWriteFreq(3200);
  //analogWriteRange(255);

  LOG("setup..ws2812fx");
  pLedNeoStripe = new WS2812FX(LED_COUNT, PIN_LED_WS_1, NEO_GRB  + NEO_KHZ800);
  pLedNeoStripe->init();
  pLedNeoStripe->setBrightness(255);
  pLedNeoStripe->setSpeed(1000);
  pLedNeoStripe->setColor(0x007BFF);
  pLedNeoStripe->setMode(FX_MODE_STATIC);
  pLedNeoStripe->start();

  LOG("setup LedSwitch");
  pLedCtrl1 = new LedCtrl(PIN_LED_SWITCH_1);
  pLedCtrl2 = new LedCtrl(PIN_LED_SWITCH_2);
  pLedCtrl1->setup("blink");
  pLedCtrl2->setup("blink");
  
  
  LOG("setup RgbLedSwitch");
  pRgbCtrl1 = new RgbLedCtrl(PIN_RGB1_LED_R,PIN_RGB1_LED_G,PIN_RGB1_LED_B);
  pRgbCtrl1->setup("breath");  

  LOG("setup COM interface");
  pCom = new Com();
  pCom->setup();

  LOG("setup..TFT");
  pTFT = new TFT_eSPI();
  pTFT->init();
  pTFT->setRotation(1);
  pTFT->fillScreen(TFT_BLACK);
  pinMode(PIN_TFT_LED, OUTPUT);
  analogWrite(PIN_TFT_LED,TFT_DIM);

  LOG("setup..cube");
  pCube = new Cube(pTFT);


  LOG(".. setup done");
}


/***********************************************************************************************************************************/
void loop() {
  now = millis();
  pCube->Render();
  pCube->moveView();

  pCom->loop();
  pLedNeoStripe->service();

  if(now - last_change > TIMER_MS) {
    pLedNeoStripe->setMode((pLedNeoStripe->getMode() + 1) % pLedNeoStripe->getModeCount());
    last_change = now;
    String name(pLedNeoStripe->getModeName(pLedNeoStripe->getMode()));
    String line = "new mode :"+name;
    LOG(line.c_str());
  }
  pLedCtrl1->loop();
  pLedCtrl2->loop();
  pRgbCtrl1->loop();

  delay(10);


}


