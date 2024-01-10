#pragma once
#include <Arduino.h>

#define LED_MAX 255
#define LED_OFF 0
#define LED_MIN 0
#define LED_LOGIC_INVERS    false



class Led
{
    public:
        Led(int pin=LED_BUILTIN,bool invers=false):
            _pin(pin),_invers(invers){
            pinMode(_pin, OUTPUT);   set(LED_OFF);   }
        ~Led() = default;

        void set(uint8_t value){
            if(_invers == true){
                analogWrite(_pin, 255-value);
            } else {
                analogWrite(_pin, value);
            }
            _value = value;
        }

        uint8_t get()              {return _value;}


    private:
        int     _pin;
        uint8_t    _value;
        bool    _invers;
};

