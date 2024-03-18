#pragma once

#include <Arduino.h>
#include "Ctrl.hpp"
#include <WS2812FX.h>


class NeoStripeCtrl : public Ctrl
{
    public:
        NeoStripeCtrl();
        ~NeoStripeCtrl() = default;

        void begin(WS2812FX * pNeoStripe);

        // overwrite virtuals    
        int setup(int nr);         // change programm
        int setup(const char *pName);   // change programm
        int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData); // config program
        void loop(uint32_t time);

        const char * getName();
        int getNr() {return _current;}
    
    private:
        WS2812FX *  _pNeoStripe;
        uint8_t     _current;
        int         _count;

        void setStdParameter();
        void setOff();


};

