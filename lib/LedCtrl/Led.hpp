#pragma once
#include <Arduino.h>

#define LED_MAX 255
#define LED_OFF 0
#define LED_MIN 0
#define LED_LOGIC_INVERS    false



class Led{
    /*
        use construtcor / begin pattern to enable full control 
        about the point in time where pins are setup/driven/switched

        because of this we need a two step setup/init:

        Led object;
        object.setup(LED_BUILTIN,false);
    */
    public:
        Led():_pin(LED_BUILTIN),_invers(false),_value(LED_OFF)      {  }
        ~Led() = default;

        virtual void begin(int pin=LED_BUILTIN,bool invers=false){
            _pin = pin;
            _invers = invers;
            pinMode(_pin, OUTPUT);   
            set(LED_OFF);
        }

        virtual void set(uint8_t value){
            if(_invers == true){
                analogWrite(_pin, 255-value);
            } else {
                analogWrite(_pin, value);
            }
            _value = value;
        }

        virtual uint8_t get() {return _value;}

    private:
        int     _pin;
        uint8_t _value;
        bool    _invers;
};

class LedProxy{
    public:
        LedProxy():_p(NULL),_value(0),_invers(false)  { }
        ~LedProxy() = default;

        virtual void begin(uint8_t * p=NULL,bool invers=false){
            if(p==NULL) STOP(F("LED proxy p to PWM reg could not be NULL"));
            _p = p;
            _invers = invers;
            set(0);
        }

        virtual void set(uint8_t value){
            if(_invers == true){
                *_p = (uint8_t) 255-value;
            } else {
                *_p = (uint8_t) value;
            }
            _value = value;
        }

    private:
        uint8_t * _p;
        uint8_t _value;
        bool    _invers;
};


