#pragma once

#include <Arduino.h>
#include "Led.hpp"
#include "Ctrl.hpp"
#include "LedAni.hpp"


class LedCtrl : public Ctrl
{
    public:
        LedCtrl(Led *pLed);
        ~LedCtrl();

        void loop(uint32_t time);
    
    private:
        Led *   _pLed;
};
