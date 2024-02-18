#pragma once

#include <Arduino.h>
#include "Ctrl.hpp"
#include "RgbLed.hpp"
#include "RgbLedAni.hpp"
#include <Debug.hpp>


class RgbLedCtrl : public Ctrl
{
    public:
        RgbLedCtrl(): Ctrl(){     }
        ~RgbLedCtrl() {}

        void begin(RgbLed * pRgbLed=NULL){
            if(pRgbLed == NULL) STOP(F("pRgbLed object could not be NULL"));
            _pRgbLed = pRgbLed;
            addAni(new RgbLedOffAni());
            addAni(new RgbLedOnAni());
            addAni(new RgbLedBlinkAni());
            addAni(new RgbLedBreathAni());
            addAni(new RgbLedRainbowAni());
            addAni(new RgbLedMultiFlashAni());
            // select first
            setup(0);
        }

        void loop(uint32_t time){
                if (_pCurrentAni == NULL)           return;
                if (_pRgbLed == NULL)               return;
                if (_mutexSetup.isLocked()==true)   return;  // do not wait 
                if (_mutexSetup.isLocked()==true){
                    return;  // do not wait 
                }
                _mutexSetup.lock();
                ((RgbLedAni*)_pCurrentAni)->loop(time,_pRgbLed);
                _mutexSetup.free();
            }
                
    private:
        RgbLed *   _pRgbLed;
};
