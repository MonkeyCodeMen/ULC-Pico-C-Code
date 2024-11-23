
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
