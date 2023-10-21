#include "RgbLed.hpp"
#include "Debug.hpp"
#include "helper.hpp"

RgbLed::RgbLed(int pinRed,int pinGreen,int pinBlue,bool invers)
{
    _pinR = pinRed;
    _pinG = pinGreen;
    _pinB = pinBlue;

    // initialize LED pins
    pinMode(_pinR, OUTPUT);
    pinMode(_pinG, OUTPUT);
    pinMode(_pinB, OUTPUT);
    _invers = invers;
    
    set(RGB_LED_OFF,RGB_LED_OFF,RGB_LED_OFF);
}

void RgbLed::set(u8_t r,u8_t g,u8_t b){
    // all set methods should use this method as backend
    // it handles invers etc.
    if (_invers == true) {
        analogWrite(_pinR, 255-r);
        analogWrite(_pinG, 255-g);
        analogWrite(_pinB, 255-b);
    } else {
        analogWrite(_pinR, r);
        analogWrite(_pinG, g);
        analogWrite(_pinB, b);
    }
    _r = r;
    _g = g;
    _b = b;
}
