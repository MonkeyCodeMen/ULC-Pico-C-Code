
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


#include <LedObjects.hpp>



// LED Switch
LedProxy    ledStripe1,ledStripe2,ledStripe3,ledStripe4;
LedCtrl     ledCtrl1,ledCtrl2,ledCtrl3,ledCtrl4;

// RGB LED
RgbLedProxy rgbLedStrip1,rgbLedStrip2;
RgbLedCtrl  rgbCtrl1,rgbCtrl2;

// WS2812 LED stripe
#define COUNT_STRIPE_1      250
#define COUNT_STRIPE_2      250
WS2812FX ws2812strip1(COUNT_STRIPE_1, PIN_STRIPE_1 , NEO_GRB  + NEO_KHZ800);
WS2812FX ws2812strip2(COUNT_STRIPE_2, PIN_STRIPE_2 , NEO_GRB  + NEO_KHZ800);
NeoStripeCtrl  neoStripeCtrl1,neoStripeCtrl2;

// WS 2812 LED matrix
Adafruit_NeoMatrix neoMatrix1(
    8, 8, 4,4, 
    PIN_MATRIX_1,
    NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
    NEO_MATRIX_ROWS    + NEO_MATRIX_PROGRESSIVE +
    NEO_TILE_ROWS      + NEO_TILE_PROGRESSIVE,
    NEO_GRB            + NEO_KHZ800);
Adafruit_NeoMatrix neoMatrix2(
    16, 16, 2,2, 
    PIN_MATRIX_2,
    NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
    NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG +
    NEO_TILE_ROWS      + NEO_TILE_PROGRESSIVE,
    NEO_GRB            + NEO_KHZ800);
NeoMatrixCtrl  neoMatrixCtrl1,neoMatrixCtrl2;


CtrlPtr ctrlObjectList[CTRL_OBJECT_COUNT] = {     &ledCtrl1,&ledCtrl2,&ledCtrl3,&ledCtrl4,
                                                  &rgbCtrl1,&rgbCtrl2,&neoStripeCtrl1,&neoStripeCtrl2,
                                                  &neoMatrixCtrl1,&neoMatrixCtrl2
                                            }; 


String ctrlNameList[CTRL_OBJECT_COUNT] = {        String("LED0"), String("LED1"), String("LED2"), String("LED3"),
                                                  String("RGB0"), String("RGB1"), String("NEO0"), String("NEO1"),
                                                  String("MAT0"), String("MAT1")
                                        };



CtrlPtr objNameToPtr(String name){
    for (int i=0;i < CTRL_OBJECT_COUNT;i++){
        if (name == ctrlNameList[i]){
            return ctrlObjectList[i];
        }
    }
    return NULL;
}

bool setupLed(){
    LOG(F("setup: LED"));
    // stripes
    ledStripe1.begin(&I2C_slaveSoll.PWM_soll[0],false);
    ledStripe2.begin(&I2C_slaveSoll.PWM_soll[1],false);
    ledStripe3.begin(&I2C_slaveSoll.PWM_soll[2],false);
    ledStripe4.begin(&I2C_slaveSoll.PWM_soll[3],false);
    // ctrl objects
    ledCtrl1.begin((Led*)&ledStripe1);
    ledCtrl2.begin((Led*)&ledStripe2);
    ledCtrl3.begin((Led*)&ledStripe3);
    ledCtrl4.begin((Led*)&ledStripe4);
    // set start animation
    ledCtrl1.select("breath");
    ledCtrl2.select("breath");
    ledCtrl3.select("breath");
    ledCtrl4.select("breath");

    LOG(F("setup: RGB LED"));
    rgbLedStrip1.begin(&I2C_slaveSoll.PWM_soll[4],&I2C_slaveSoll.PWM_soll[5],&I2C_slaveSoll.PWM_soll[6],false);
    rgbLedStrip2.begin(&I2C_slaveSoll.PWM_soll[7],&I2C_slaveSoll.PWM_soll[8],&I2C_slaveSoll.PWM_soll[9],false);
    rgbCtrl1.begin((RgbLed*)&rgbLedStrip1);
    rgbCtrl2.begin((RgbLed*)&rgbLedStrip2);
    rgbCtrl1.select("rainbow");  
    rgbCtrl2.select("rainbow");  

    LOG(F("setup: Neo stripe"));
    neoStripeCtrl1.begin(&ws2812strip1);
    neoStripeCtrl2.begin(&ws2812strip2);
    neoStripeCtrl1.select(13);  
    neoStripeCtrl2.select(13);  

    LOG(F("setup: Neo matrix"));
    neoMatrixCtrl1.begin(&neoMatrix1);
    neoMatrixCtrl2.begin(&neoMatrix2);
    neoMatrixCtrl1.select("circle");
    neoMatrixCtrl2.select("circle");   

    return true;
}

