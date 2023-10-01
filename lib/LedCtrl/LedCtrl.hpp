#pragma once

#include <Arduino.h>
#include "Led.hpp"
#include "Ctrl.hpp"
#include "LedAni.hpp"


class LedCtrl : public Ctrl
{
    public:
        LedCtrl(int pin);
        ~LedCtrl();

        void loop();
    
    private:
        Led *   _pLed;
};
