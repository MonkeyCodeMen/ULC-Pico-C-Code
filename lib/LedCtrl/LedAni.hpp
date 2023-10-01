#pragma once
#include <Arduino.h>
#include "Led.hpp"
#include "Ani.hpp"

class LedAni : public Ani
{
    public:
        LedAni(String name) : Ani(name) {};
        ~LedAni() = default;

        virtual void loop(Led * pLed) {};
};


class LedOffAni : public LedAni{
    public:
        LedOffAni()  : LedAni(String("off"))  {};
        void reset()                                {};
        void loop(Led * pLed)                     {pLed->set(LED_OFF);};
        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData) {};
};

class LedOnAni : public LedAni{
    public:
        LedOnAni()  : LedAni(String("on"))  {};
        void reset()                                {};
        void loop(Led * pLed)                     {pLed->set(LED_MAX);};
        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData) {};
};


class LedDimAni : public LedAni{
    public:
        LedDimAni()  : LedAni(String("dim"))    {_dimValue = LED_OFF;};
        void reset()                                {_dimValue = LED_OFF;};
        void loop(Led * pLed)                     {pLed->set(_dimValue);};
        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData)  { 
            if (p1 > LED_MAX) p1 = LED_MAX;
            if (p1 < LED_OFF) p1 = LED_OFF;
            _dimValue = p1 & 0xFF;
        };
    private:
        u8_t _dimValue;
};


class LedBlinkAni : public LedAni{
    public:
        LedBlinkAni()  : LedAni(String("blink")) {   
            _state=init;
            _onTime_ms = 250; 
            _offTime_ms = 250; 
            _dimValue = 50; 
        };
        
        void reset() {
            _state=init;
        };

        void loop(Led * pLed){
            u32_t diff;
            switch (_state){
                case init:
                    _state = off;
                    pLed->set(LED_OFF);   
                    _lastSwitchTime = millis();
                    break;
                
                case off:
                    diff = millis()-_lastSwitchTime;
                    if (diff > _offTime_ms){
                        _state = on;
                        pLed->set(_dimValue);   
                        _lastSwitchTime = millis();
                    }
                    break;
                
                case on:
                    diff = millis()-_lastSwitchTime;
                    if (diff > _onTime_ms){
                        _state = off;
                        pLed->set(LED_OFF);   
                        _lastSwitchTime = millis();
                    }
                    break;
            }
        };

        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData)  { 
            if (p1 > LED_MAX) p1 = LED_MAX;
            if (p1 < LED_OFF) p1 = LED_OFF;
            _dimValue = p1;
            _onTime_ms = p2;
            _offTime_ms = p3;
            _state = init;

        };

    private:
        enum BlinkState {init,on,off};
        BlinkState _state;
        u8_t _dimValue;
        u32_t _onTime_ms,_offTime_ms;
        u32_t _lastSwitchTime;
};

