#pragma once

#include <Arduino.h>
#include "Ctrl.hpp"
#include <WS2812FX.h>


class NeoStripeCtrl : public Ctrl
{
    public:
        NeoStripeCtrl(WS2812FX * pNeoStripe);
        ~NeoStripeCtrl() = default;

        // overwrite virtuals    
        int setup(int nr);         // change programm
        int setup(const char *pName);   // change programm
        int setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,String str,u32_t length,u8_t ** pData); // config program
        void loop(u32_t time);

        const char * getName();
    
    private:
        WS2812FX *  _pNeoStripe;
        u8_t        _current;

        void setStdParameter();
        void setOff();


};

