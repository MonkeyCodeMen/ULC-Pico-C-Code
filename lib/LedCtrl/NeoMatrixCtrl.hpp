#pragma once
#include "Ctrl.hpp"
#include <Adafruit_NeoMatrix.h>

class NeoMatrixCtrl : public Ctrl
{
    public:
        NeoMatrixCtrl(int pin,int dimX,int dimY,int countX,int countY,int order,int type);
        ~NeoMatrixCtrl();

        void loop();
    
    private:
        Adafruit_NeoMatrix * _pMatrix;
};

