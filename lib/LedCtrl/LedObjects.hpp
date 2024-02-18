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
extern LedProxy     ledStripe1,ledStripe2,ledStripe3,ledStripe4;
extern LedCtrl      ledCtrl1,ledCtrl2,ledCtrl3,ledCtrl4;

// RGB LED
extern RgbLedProxy  rgbLedStrip1,rgbLedStrip2;
extern RgbLedCtrl   rgbCtrl1,rgbCtrl2;

// WS2812 LED stripe
extern WS2812FX         ws2812strip1,ws2812strip2; 
extern NeoStripeCtrl    neoStripeCtrl1,neoStripeCtrl2;

// WS 2812 LED matrix
extern Adafruit_NeoMatrix   neoMatrix1,neoMatrix2;
extern NeoMatrixCtrl        neoMatrixCtrl1,neoMatrixCtrl2;

bool setupLed();


