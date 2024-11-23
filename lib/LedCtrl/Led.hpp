
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


