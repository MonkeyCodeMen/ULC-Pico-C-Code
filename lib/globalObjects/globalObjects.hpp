#pragma once

#include <Arduino.h>
#include <MainConfig.h>
#include <Debug.hpp>
#include <helper.hpp>
#include <Adafruit_NeoMatrix.h>
#include <WS2812FX.h>
#include <LedCtrl.hpp>
#include <RgbLedCtrl.hpp>
#include <NeoStripeCtrl.hpp>
#include <NeoMatrixCtrl.hpp>
#include <Mutex.hpp>



// LED switch stripes
extern Led          ledStripe1;
extern Led          ledStripe2;
extern LedCtrl *    pLedCtrl1;
extern LedCtrl *    pLedCtrl2;

// RGB LED
extern RgbLed       rgbLedStrip1;
extern RgbLedCtrl * pRgbCtrl1;

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

// HW mngt. 
extern Mutex globalSPI0_mutex;



#ifdef WITH_SD_CARD
    #include <SPI.h>
    #include <SD.h>
    extern SDClass globalSDcard0;
#endif 

