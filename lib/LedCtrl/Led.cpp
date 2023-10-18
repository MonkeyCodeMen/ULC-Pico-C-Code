#include "Led.hpp"

Led::Led(int pin)
{
    if (pin < 0){
        _sim = true;
        _pin = -1;
    } else {
        _sim = false;
        _pin = pin; 
        pinMode(_pin, OUTPUT);
    }
    set(LED_OFF);
}

void Led::set(u8_t value){
    _value = value;
    if (_sim == false){
        #if LED_PWM_RANGE == 255
            #if LED_LOGIC_INVERS == true
                analogWrite(_pin, LED_PWM_RANGE-value);
            #else
                analogWrite(_pin, value);
            #endif
        #else
            u32_t temp = ((u32_t)value * LED_PWM_RANGE) / 255;
            #if LED_LOGIC_INVERS == true
                analogWrite(_pin, LED_PWM_RANGE-temp);
            #else
                analogWrite(_pin, temp);
            #endif
        #endif
    }
}