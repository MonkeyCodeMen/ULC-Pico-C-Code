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
        RgbLed(): _pinR(0),_pinG(0),_pinB(0), _r(0),_g(0),_b(0), _invers(false)  {  }
        ~RgbLed() = default;

        virtual void begin(int pinRed=LED_BUILTIN,int pinGreen=LED_BUILTIN,int pinBlue=LED_BUILTIN,bool invers=false){
                // initialize LED pins
                pinMode(_pinR, OUTPUT);
                pinMode(_pinG, OUTPUT);
                pinMode(_pinB, OUTPUT);
                set(RGB_LED_OFF,RGB_LED_OFF,RGB_LED_OFF);
        }

        void set(uint8_t r,uint8_t g,uint8_t b,uint8_t dim) { set(dimColorChannel255(r,dim),dimColorChannel255(g,dim),dimColorChannel255(b,dim));   }
        void set(uint32_t col)                              { set(HH_BYTE(col),H_BYTE(col),L_BYTE(col));        }
        void set(uint32_t col,uint8_t dim)                  { set(HH_BYTE(col),H_BYTE(col),L_BYTE(col),dim);    }
        uint8_t getR()                                      { return _r;                                        }
        uint8_t getG()                                      { return _g;                                        }
        uint8_t getB()                                      { return _b;                                        }
        virtual void set(uint8_t r,uint8_t g,uint8_t b){
                if (_invers == true) { 
                    analogWrite(_pinR, 255-r);  
                    analogWrite(_pinG, 255-g); 
                    analogWrite(_pinB, 255-b);
                } else { 
                    analogWrite(_pinR, r);  
                    analogWrite(_pinG, g);  
                    analogWrite(_pinB, b);  
                }
                _r = r; _g = g; _b = b;
            }
        
    private:
        int     _pinR,_pinG,_pinB;
        uint8_t _r,_g,_b;
        bool    _invers;
};

class RgbLedProxy
{
    public:
        RgbLedProxy(): _pR(NULL),_pG(NULL),_pB(NULL), _r(0),_g(0),_b(0), _invers(false)  {  }
        ~RgbLedProxy() = default;

        virtual void begin(uint8_t* pR=NULL,uint8_t* pG=NULL,uint8_t* pB=NULL,bool invers=false){
            if(pR==NULL) STOP(F("pointer to PWM reg could not be NULL"));
            if(pG==NULL) STOP(F("pointer to PWM reg could not be NULL"));
            if(pB==NULL) STOP(F("pointer to PWM reg could not be NULL"));
            _pR = pR;
            _pG = pG;
            _pB = pB;
            _invers = invers;
            set(RGB_LED_OFF,RGB_LED_OFF,RGB_LED_OFF);
        }

        virtual void set(uint8_t r,uint8_t g,uint8_t b){
                if (_invers == true) { 
                    *_pR = 255-r;  
                    *_pG = 255-g; 
                    *_pB = 255-b;
                } else { 
                    *_pR = r;  
                    *_pG = g; 
                    *_pB = b;
                }
                _r = r; _g = g; _b = b;
            }
        
    private:
        uint8_t *_pR,*_pG,*_pB;
        uint8_t _r,_g,_b;
        bool    _invers;
};



