#include "RgbLed.hpp"
#include "Debug.hpp"
#include "helper.hpp"

RgbLed::RgbLed(int pinRed,int pinGreen,int pinBlue)
{
    _pinR = pinRed;
    _pinG = pinGreen;
    _pinB = pinBlue;

    // initialize LED pins
    pinMode(_pinR, OUTPUT);
    pinMode(_pinG, OUTPUT);
    pinMode(_pinB, OUTPUT);
    
    set(RGB_LED_OFF,RGB_LED_OFF,RGB_LED_OFF);
}

void RgbLed::set(u32_t value){
    u8_t r,g,b;
    r = unpackR(value);
    g = unpackG(value);
    b = unpackB(value);
    set(r,g,b);
}

void RgbLed::set(u32_t value,u16_t dim){
    u8_t r,g,b;
    r = dimChannel(unpackR(value),dim);
    g = dimChannel(unpackG(value),dim);
    b = dimChannel(unpackB(value),dim);
    set(r,g,b);
}

void RgbLed::set(u8_t r,u8_t g,u8_t b,u16_t dim){
    r = dimChannel(r,dim);
    g = dimChannel(g,dim);
    b = dimChannel(b,dim);
    set(r,g,b);
}



void RgbLed::set(u8_t r,u8_t g,u8_t b){
    // all set methods should use this method as backend
    // it handles invers etc.
    #if RGB_LED_PWM_RANGE == 255
        #if RGB_LED_LOGIC_INVERS == true
            analogWrite(_pinR, RGB_LED_PWM_RANGE-r);
            analogWrite(_pinG, RGB_LED_PWM_RANGE-g);
            analogWrite(_pinB, RGB_LED_PWM_RANGE-b);
        #else
            analogWrite(_pinR, r);
            analogWrite(_pinG, g);
            analogWrite(_pinB, b);
        #endif
    #else
        u32_t tempR = ((u32_t)r * RGB_LED_PWM_RANGE) / 255;
        u32_t tempG = ((u32_t)g * RGB_LED_PWM_RANGE) / 255;
        u32_t tempB = ((u32_t)b * RGB_LED_PWM_RANGE) / 255;
        #if RGB_LED_LOGIC_INVERS == true
            analogWrite(_pinR, RGB_LED_PWM_RANGE-tempR);
            analogWrite(_pinG, RGB_LED_PWM_RANGE-tempG);
            analogWrite(_pinB, RGB_LED_PWM_RANGE-tempB);
        #else
            analogWrite(_pinR, tempR);
            analogWrite(_pinG, tempG);
            analogWrite(_pinB, tempB);
        #endif
    #endif
}

u32_t RgbLed::pack(u8_t r,u8_t g, u8_t b) {
    u32_t res;
    res  = r<<16;
    res |= g<<8;
    res |= b;
    return res;
}

u8_t RgbLed::unpackR(u32_t value) {
    u8_t res = (value>>16) & 0xFF;
    return res;
}

u8_t RgbLed::unpackG(u32_t value) {
    u8_t res = (value>>8) & 0xFF;
    return res;
}

u8_t RgbLed::unpackB(u32_t value) {
    u8_t res = value & 0xFF;
    return res;
}


u8_t RgbLed::dimChannel(u8_t value,u16_t dim){
    dim = clamp(0,value,RGB_DIM_ACCURACY);
    return (value*dim)/RGB_DIM_ACCURACY;
}


