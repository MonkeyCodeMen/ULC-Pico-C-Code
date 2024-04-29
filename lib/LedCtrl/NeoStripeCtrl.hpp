#pragma once

#include <Arduino.h>
#include "Ctrl.hpp"
#include <WS2812FX.h>

/*
cmd's:

"Off","Static","Blink","Breath","Color Wipe","Color Wipe Inverse","Color Wipe Reverse","Color Wipe Reverse Inverse","Color Wipe Random","Random Color","Single Dynamic",
"Multi Dynamic","Rainbow","Rainbow Cycle","Scan","Dual Scan","Fade","Theater Chase","Theater Chase Rainbow","Running Lights","Twinkle","Twinkle Random","Twinkle Fade",
"Twinkle Fade Random","Sparkle","Flash Sparkle","Hyper Sparkle","Strobe","Strobe Rainbow","Multi Strobe","Blink Rainbow","Chase White","Chase Color","Chase Random",
"Chase Rainbow","Chase Flash","Chase Flash Random","Chase Rainbow White","Chase Blackout","Chase Blackout Rainbow","Color Sweep Random","Running Color","Running Red Blue",
"Running Random","Larson Scanner","Comet","Fireworks","Fireworks Random","Merry Christmas","Fire Flicker","Fire Flicker (soft)","Fire Flicker (intense)","Circus Combustus",
"Halloween","Bicolor Chase","Tricolor Chase","TwinkleFOX","Rain",
"Custom 0","Custom 1","Custom 2","Custom 3","Custom 4","Custom 5","Custom 6","Custom 7",
*/

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

