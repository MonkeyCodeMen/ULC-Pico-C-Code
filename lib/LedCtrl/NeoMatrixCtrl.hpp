
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
#include "Ctrl.hpp"
#include <Adafruit_NeoMatrix.h>
#include <NeoMatrixAni.hpp>
#include <NeoMatrixBox.hpp>
#include <NeoMatrixGif.hpp>
#include <NeoMatrixCalendar.hpp>


class NeoMatrixCtrl : public Ctrl
{
    public:
        NeoMatrixCtrl() : Ctrl(),_pMatrix(NULL) { }
        ~NeoMatrixCtrl() {}

        void begin(Adafruit_NeoMatrix * pMatrix){
            ASSERT(pMatrix != NULL,F("pMatrix must not be NULL"));
            _pMatrix = pMatrix;
            _pMatrix->begin();
            pMatrix->setBrightness(255);  
            pMatrix->fillScreen(pMatrix->Color(0,0,0));
            pMatrix->show();
            addAni(new MatrixOffAni());
            addAni(new MatrixOnAni());
            addAni(new MatrixDimAni());
            addAni(new MatrixBreathAni());
            addAni(new MatrixBlinkAni());
            addAni(new MatrixMultiFlashAni());
            addAni(new MatrixRainbowAni());
            addAni(new MatrixBoxAni("rect"));
            addAni(new MatrixBoxAni("circle"));
            addAni(new MatrixBoxAni("ver"));
            addAni(new MatrixBoxAni("hor"));
            addAni(new MatrixGifFileAni());
            addAni(new MatrixGifCalendarAni());
            // select first
            select(0);
        }

        void loop(uint32_t time){
                if (isAniSelected() == false)       return;
                if (_pMatrix == NULL)               return;
                if (_mutexSetup.isLocked()==true)   return;  // do not wait 
               _mutexSetup.lock();
                ((NeoMatrixAni*)_currentNode.pAni)->loop(time,_pMatrix);
                _mutexSetup.free();
            }
    
    private:
        Adafruit_NeoMatrix * _pMatrix;
};

