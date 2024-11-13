#pragma once

#include <Arduino.h>
#include <helper.h>
#include <Ctrl.hpp>
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
        int select(int nr);                 // change programm
        int select(const char *pName);      // change programm
        int config(AniCfg cfg);             // config program
        void loop(uint32_t time);

        const char * getName();
        int getNr()                 { return _current;}
        uint32_t    getAniCount()   { return _count;  } 
        
    private:
        WS2812FX *  _pNeoStripe;
        uint8_t     _current;
        int         _count;
        Ani         _aniInterface;
        uint32_t    _cfgCounter;
};

