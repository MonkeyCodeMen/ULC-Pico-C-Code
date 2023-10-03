#include "RgbLed.hpp"
#include "Debug.hpp"

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


void RgbLed::set(u8_t r,u8_t g,u8_t b){
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


u32_t RgbLed::getPackedColorWheel(u8_t pos){
    u8_t r,g,b;
    pos = 255 - pos;
    if(pos < 85) {
        r = 255 - pos * 3;
        g = 0;
        b = pos*3;
    } else if(pos < 170) {
        pos -= 85;
        r = 0;
        g = pos * 3;
        b = 255 - pos * 3;
    } else {
        pos -= 170;
        r = pos * 3;
        g = 255 - pos * 3;
        b = 0;
    }

    u32_t res = pack(r,g,b);
    return res;
}


/*

void PWM_RGB::setWheelColor(u8_t pos) {
    pos = 255 - pos;
    if(pos < 85) {
        _r = 255 - pos * 3;
        _g = 0;
        _b = pos*3;
    } else if(pos < 170) {
        pos -= 85;
        _r = 0;
        _g = pos * 3;
        _b = 255 - pos * 3;
    } else {
        pos -= 170;
        _r = pos * 3;
        _g = 255 - pos * 3;
        _b = 0;
    }
    loop();
}

*/