#include "Led.hpp"

Led::Led(int pin)
{
    _pin = pin; 
    pinMode(_pin, OUTPUT);
    set(LED_OFF);
}

void Led::set(u8_t value){
    #if LED_LOGIC_INVERS == true
        analogWrite(_pin, 255-value);
    #else
        analogWrite(_pin, value);
    #endif
    _value = value;
}