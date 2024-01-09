#pragma once

#include <Arduino.h>
#include "Ctrl.hpp"
#include "RgbLed.hpp"
#include "RgbLedAni.hpp"


class RgbLedCtrl : public Ctrl
{
    public:
        RgbLedCtrl(RgbLed * pRgbLed);
        ~RgbLedCtrl();

        void loop(uint32_t time);
    
    private:
        RgbLed *   _pRgbLed;

};
