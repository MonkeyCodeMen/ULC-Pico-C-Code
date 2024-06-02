#pragma once
#include "Ctrl.hpp"
#include <Adafruit_NeoMatrix.h>
#include <NeoMatrixAni.hpp>
#include <NeoMatrixBox.hpp>
#include <NeoMatrixGif.hpp>


class NeoMatrixCtrl : public Ctrl
{
    public:
        NeoMatrixCtrl() : Ctrl(),_pMatrix(NULL) { }
        ~NeoMatrixCtrl() {}

        void begin(Adafruit_NeoMatrix * pMatrix){
            ASSERT(pMatrix != NULL,F("pMatrix must not be NULL"));
            _pMatrix = pMatrix;
            _pMatrix->begin();
            // does not work to dim colors down and then UP (original color is not stored)
            // the implement dimming by color recalculation
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

