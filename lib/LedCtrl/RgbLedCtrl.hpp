#pragma once

#include <Arduino.h>
#include "Ctrl.hpp"
#include "RgbLed.hpp"
#include "RgbLedAni.hpp"


class RgbLedCtrl : public Ctrl
{
    public:
        RgbLedCtrl(int pinR,int pinG,int pinB);
        ~RgbLedCtrl();

        void loop();
    
    private:
        RgbLed *   _pRgbLed;

};
