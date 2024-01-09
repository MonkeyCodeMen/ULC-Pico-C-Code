#pragma once
#include "Ctrl.hpp"
#include <Adafruit_NeoMatrix.h>

class NeoMatrixCtrl : public Ctrl
{
    public:
        NeoMatrixCtrl(Adafruit_NeoMatrix * pMatrix);
        ~NeoMatrixCtrl();

        void loop(uint32_t time);
    
    private:
        Adafruit_NeoMatrix * _pMatrix;
};

