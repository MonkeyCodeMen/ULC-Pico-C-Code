#pragma once
#include <Arduino.h>
#include "Led.hpp"

#define RGB_LED_MAX 255
#define RGB_LED_OFF 0
#define RGB_LED_MIN 0


class RgbLed
{
    public:
        RgbLed(int pinRed,int pinGreen,int pinBlue);
        ~RgbLed() = default;

        void set(u8_t r,u8_t g,u8_t b);
        void set(u32_t value);

        void set(u8_t r,u8_t g,u8_t b,u8_t dimm);
        void set(u32_t value,u8_t dim);


        static u32_t pack(u8_t r,u8_t g, u8_t b);
        static u8_t unpackR(u32_t value);
        static u8_t unpackG(u32_t value);
        static u8_t unpackB(u32_t value);

        static u8_t  dimChannel(u8_t value,u8_t dim);

    private:
        int     _pinR,_pinG,_pinB;
};
