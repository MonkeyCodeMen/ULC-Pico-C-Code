
/*
 * MIT License
 * 
 * Copyright (c) 2024 MonkeyCodeMen@GitHub
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * provided to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#pragma once

#include <Arduino.h>

#include <MainConfig.h>
#include <PinMapping.h>

#include <I2C_register.h>

#include <helper.h>
#include <Adafruit_NeoMatrix.h>
#include <LedCtrl.hpp>
#include <RgbLedCtrl.hpp>
#include <NeoStripeCtrl.hpp>
#include <NeoMatrixCtrl.hpp>



#define max
#include <WS2812FX.h>
#undef max 


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


#define CTRL_OBJECT_COUNT   10
typedef Ctrl* CtrlPtr;

extern CtrlPtr ctrlObjectList[CTRL_OBJECT_COUNT];
extern String  ctrlNameList[CTRL_OBJECT_COUNT];



CtrlPtr   objNameToPtr(String name);


