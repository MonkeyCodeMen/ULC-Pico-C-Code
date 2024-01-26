#pragma once

#include <Arduino.h>
#include <Debug.hpp>
#include "Led.hpp"
#include "Ctrl.hpp"
#include "LedAni.hpp"


class LedCtrl : public Ctrl
{
    public:
        LedCtrl(Led *pLed):Ctrl(){
                // create LED object
                ASSERT(pLed != NULL,F("pLed must not be NULL"));
                _pLed = pLed;
                // fill ani List
                addAni(new LedOffAni());
                addAni(new LedOnAni());
                addAni(new LedDimAni());
                addAni(new LedBreathAni());
                addAni(new LedBlinkAni());
                addAni(new LedMultiFlashAni());
                // setup first
                setup(0);
            }

        ~LedCtrl(){  }


        void loop(uint32_t time){
                if (_pCurrentAni == NULL)           return;
                if (_pLed == NULL)                  return;
                if (_mutexSetup.isLocked()==true)   return;  // do not wait 
                _mutexSetup.lock();
                ((LedAni*)_pCurrentAni)->loop(time,_pLed);
                _mutexSetup.free();
            }
    
    private:
        Led *   _pLed;
};
