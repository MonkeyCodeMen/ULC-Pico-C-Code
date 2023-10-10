#pragma once

#include <Arduino.h>
#include "Ctrl.hpp"
#include <WS2812FX.h>


class NeoStripeCtrl : public Ctrl
{
    public:
        NeoStripeCtrl(u16_t num_leds, u8_t pin, neoPixelType type);
        ~NeoStripeCtrl() = default;

        // overwrite virtuals    
        void setup(int nr);         // change programm
        void setup(String& name);   // change programm
        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData); // config program
        void loop();

        String getName();
    
    private:
        WS2812FX *  _pNeoStripe;
        u8_t        _current;

        void setStdParameter();
        void setOff();


};

