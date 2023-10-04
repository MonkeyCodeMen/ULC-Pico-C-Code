#pragma once
#include <Arduino.h>
#include "Led.hpp"
#include "Ani.hpp"
#include "helper.hpp"

class LedAni : public Ani
{
    public:
        LedAni(String name) : Ani(name) {};
        ~LedAni() = default;

        virtual void loop(Led * pLed) {};

    // base class
        //String getName()		{return _name;};
		//virtual void reset()  {};
        //virtual void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData) {};
        // if nothing to do for this member functions you can stay with the default implentation from base class

};


class LedOffAni : public LedAni{
    public:
        LedOffAni()  : LedAni(String("off"))        {};
        void loop(Led * pLed)                       {pLed->set(LED_OFF);};
};

class LedOnAni : public LedAni{
    public:
        LedOnAni()  : LedAni(String("on"))          {};
        void loop(Led * pLed)                       {pLed->set(LED_MAX);};
};


class LedDimAni : public LedAni{
    public:
        LedDimAni()  : LedAni(String("dim"))        {};
        void reset()                                {_dimValue = LED_OFF;};
        void loop(Led * pLed)                       {pLed->set(_dimValue);};
        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData)  
                                                    {_dimValue = clamp(LED_OFF,p1,LED_MAX);}; 
    private:
        u8_t _dimValue;
};


class LedBlinkAni : public LedAni{
    public:
        LedBlinkAni()  : LedAni(String("blink"))    {};
        
        void reset() {
            _state=init;
            _onTime_ms = 250; 
            _offTime_ms = 250; 
            _dimValue = 50; 
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
            _dimValue = clamp(LED_OFF,p1,LED_MAX);
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

