#pragma once
#include <Arduino.h>
#include "Led.hpp"
#include <helper.h>

#define RGB_LED_MAX 255
#define RGB_LED_OFF 0
#define RGB_LED_MIN 0


class RgbLed
{
    public:
        RgbLed(int pinRed,int pinGreen,int pinBlue,bool invers=false):
        _pinR(pinRed),_pinG(pinGreen),_pinB(pinBlue),
        _r(0),_g(0),_b(0), _invers(invers)  {
                // initialize LED pins
                pinMode(_pinR, OUTPUT);
                pinMode(_pinG, OUTPUT);
                pinMode(_pinB, OUTPUT);
                set(RGB_LED_OFF,RGB_LED_OFF,RGB_LED_OFF);
            }

        ~RgbLed() = default;

        void set(uint8_t r,uint8_t g,uint8_t b,uint8_t dim) { set(dimColorChannel255(r,dim),dimColorChannel255(g,dim),dimColorChannel255(b,dim));   }
        void set(uint32_t col)                              { set(HH_BYTE(col),H_BYTE(col),L_BYTE(col));        }
        void set(uint32_t col,uint8_t dim)                  { set(HH_BYTE(col),H_BYTE(col),L_BYTE(col),dim);    }
        uint8_t getR()                                      { return _r;                                        }
        uint8_t getG()                                      { return _g;                                        }
        uint8_t getB()                                      { return _b;                                        }
        void set(uint8_t r,uint8_t g,uint8_t b){
                if (_invers == true) { analogWrite(_pinR, 255-r);  analogWrite(_pinG, 255-g); analogWrite(_pinB, 255-b);
                } else { analogWrite(_pinR, r);  analogWrite(_pinG, g);  analogWrite(_pinB, b);  }
                _r = r; _g = g; _b = b;
            }
        
    private:
        int     _pinR,_pinG,_pinB;
        uint8_t    _r,_g,_b;
        bool    _invers;
};



