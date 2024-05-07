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
            addAni(new RgbLedDimAni());
            addAni(new RgbLedBreathAni());
            addAni(new RgbLedBlinkAni());
            addAni(new RgbLedMultiFlashAni());
            addAni(new RgbLedRainbowAni());
            // select rainbow as standard
            select(6);
        }

        void loop(uint32_t time){
                if (isAniSelected() == false)       return;
                if (_pRgbLed == NULL)               return;
                if (_mutexSetup.isLocked()==true)   return;  // do not wait 
                if (_mutexSetup.isLocked()==true){
                    return;  // do not wait 
                }
                _mutexSetup.lock();
                ((RgbLedAni*)_currentNode.pAni)->loop(time,_pRgbLed);
                _mutexSetup.free();
            }
                
    private:
        RgbLed *   _pRgbLed;
};
