#pragma once
#include <Arduino.h>
#include "Led.hpp"
#include <helper.hpp>

#define RGB_LED_MAX 255
#define RGB_LED_OFF 0
#define RGB_LED_MIN 0


class RgbLed
{
    public:
        RgbLed(int pinRed,int pinGreen,int pinBlue,bool invers=false);
        ~RgbLed() = default;

        void set(u8_t r,u8_t g,u8_t b);
        void set(u8_t r,u8_t g,u8_t b,u8_t dim) {set(dimColorChannel255(r,dim),dimColorChannel255(g,dim),dimColorChannel255(b,dim));};
        void set(u32_t col)                     {set(HH_BYTE(col),H_BYTE(col),L_BYTE(col));};
        void set(u32_t col,u8_t dim)            {set(HH_BYTE(col),H_BYTE(col),L_BYTE(col),dim);};
        
        u8_t getR()     {return _r;};
        u8_t getG()     {return _g;};
        u8_t getB()     {return _b;};
        
    private:
        int     _pinR,_pinG,_pinB;
        u8_t    _r,_g,_b;
        bool    _invers;
};
