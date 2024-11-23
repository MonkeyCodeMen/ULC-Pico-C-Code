
/*
 * MIT License
 * 
 * Copyright (c) 2024 MonkeyCodeMen@GitHub
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * provided to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#pragma once

#include <Arduino.h>
#include <Debug.hpp>
#include "Led.hpp"
#include "Ctrl.hpp"
#include "LedAni.hpp"


class LedCtrl : public Ctrl
{
    public:
        LedCtrl():Ctrl(),_pLed(NULL){
            }

        ~LedCtrl(){  }

        void begin(Led *pLed){
                // create LED object
                if (pLed == NULL) STOP(F("pLed must not be NULL"));
                _pLed = pLed;
                // fill ani List
                addAni(new LedOffAni());
                addAni(new LedOnAni());
                addAni(new LedDimAni());
                addAni(new LedBreathAni());
                addAni(new LedBlinkAni());
                addAni(new LedMultiFlashAni());
                // setup first
                select(0);
        }


        void loop(uint32_t time){
                if (isAniSelected()==false)         return;
                if (_pLed == NULL)                  return;
                if (_mutexSetup.isLocked()==true)   return;  // do not wait 
                _mutexSetup.lock();
                ((LedAni*)_currentNode.pAni)->loop(time,_pLed);
                _mutexSetup.free();
            }
    
    private:
        Led *   _pLed;
};
