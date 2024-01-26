#include <Arduino.h>
#include <LedObjects.hpp>
#include <PinMapping.h>



// LED Switch
Led     ledStripe1(PIN_LED_SWITCH_1);
Led     ledStripe2(PIN_LED_SWITCH_2);
Led     ledStripe3(PIN_LED_SWITCH_3);
Led     ledStripe4(PIN_LED_SWITCH_4);
LedCtrl    * pLedCtrl1;
LedCtrl    * pLedCtrl2;
LedCtrl    * pLedCtrl3;
LedCtrl    * pLedCtrl4;

// RGB LED
RgbLed  rgbLedStrip1(PIN_RGB1_LED_R,PIN_RGB1_LED_G,PIN_RGB1_LED_B);
RgbLed  rgbLedStrip2(PIN_RGB2_LED_R,PIN_RGB2_LED_G,PIN_RGB2_LED_B);
RgbLedCtrl * pRgbCtrl1;
RgbLedCtrl * pRgbCtrl2;

// WS2812 LED stripe
#define COUNT_STRIPE_1      250
#define COUNT_STRIPE_2      50
WS2812FX ws2812strip1(COUNT_STRIPE_1, PIN_STRIPE_1 , NEO_GRB  + NEO_KHZ800);
WS2812FX ws2812strip2(COUNT_STRIPE_2, PIN_STRIPE_2 , NEO_GRB  + NEO_KHZ800);
NeoStripeCtrl  * pNeoStripeCtrl1;
NeoStripeCtrl  * pNeoStripeCtrl2;

// WS 2812 LED matrix
Adafruit_NeoMatrix neoMatrix1(
    8, 8, 4,4, 
    PIN_MATRIX_1,
    NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
    NEO_MATRIX_ROWS    + NEO_MATRIX_PROGRESSIVE +
    NEO_TILE_ROWS      + NEO_TILE_PROGRESSIVE,
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


bool setupLed(){
    LOG(F("setup: LED"));
    pLedCtrl1 = new LedCtrl(&ledStripe1);
    pLedCtrl2 = new LedCtrl(&ledStripe2);
    pLedCtrl3 = new LedCtrl(&ledStripe3);
    pLedCtrl4 = new LedCtrl(&ledStripe4);
    pLedCtrl1->setup("breath");
    pLedCtrl2->setup("breath");
    pLedCtrl3->setup("breath");
    pLedCtrl4->setup("breath");

    LOG(F("setup: RGB LED"));
    pRgbCtrl1 = new RgbLedCtrl(&rgbLedStrip1);
    pRgbCtrl2 = new RgbLedCtrl(&rgbLedStrip2);
    pRgbCtrl1->setup("rainbow");  
    pRgbCtrl2->setup("rainbow");  

    LOG(F("setup: Neo stripe"));
    pNeoStripeCtrl1 = new NeoStripeCtrl(&ws2812strip1);
    pNeoStripeCtrl2 = new NeoStripeCtrl(&ws2812strip2);
    pNeoStripeCtrl1->setup(13);  
    pNeoStripeCtrl2->setup(13);  

    LOG(F("setup: Neo matrix"));
    pNeoMatrixCtrl1 = new NeoMatrixCtrl(&neoMatrix1);
    pNeoMatrixCtrl2 = new NeoMatrixCtrl(&neoMatrix2);
    pNeoMatrixCtrl1->setup("breath");
    pNeoMatrixCtrl2->setup("breath");   

    return true;
}