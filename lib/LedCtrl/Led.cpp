#include "Led.hpp"

Led::Led(int pin,bool invers)
{
    _pin = pin; 
    _invers = invers;
    pinMode(_pin, OUTPUT);
    set(LED_OFF);
}

void Led::set(u8_t value){
    if(_invers == true){
        analogWrite(_pin, 255-value);
    } else {
        analogWrite(_pin, value);
    }
    _value = value;
}