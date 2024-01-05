#pragma once

#include <Arduino.h>
#include <MainConfig.h>
#include <Adafruit_NeoMatrix.h>
#include <WS2812FX.h>
#include <LedCtrl.hpp>
#include <RgbLedCtrl.hpp>
#include <NeoStripeCtrl.hpp>
#include <NeoMatrixCtrl.hpp>



// LED switch stripes
extern Led          ledStripe1;
extern Led          ledStripe2;
extern Led          ledStripe3;
extern Led          ledStripe4;
extern LedCtrl *    pLedCtrl1;
extern LedCtrl *    pLedCtrl2;
extern LedCtrl *    pLedCtrl3;
extern LedCtrl *    pLedCtrl4;

/* ToDo .. instead of pointer and new .. change to LedObjects class with begin functonality */

// RGB LED
extern RgbLed       rgbLedStrip1;
extern RgbLed       rgbLedStrip2;
extern RgbLedCtrl * pRgbCtrl1;
extern RgbLedCtrl * pRgbCtrl2;

// WS2812 LED stripe
extern WS2812FX         ws2812strip1;
extern WS2812FX         ws2812strip2; 
extern NeoStripeCtrl *  pNeoStripeCtrl1;
extern NeoStripeCtrl *  pNeoStripeCtrl2;

// WS 2812 LED matrix
extern Adafruit_NeoMatrix   neoMatrix1;
extern Adafruit_NeoMatrix   neoMatrix2;
extern NeoMatrixCtrl *      pNeoMatrixCtrl1;
extern NeoMatrixCtrl *      pNeoMatrixCtrl2;

bool setupLed();


