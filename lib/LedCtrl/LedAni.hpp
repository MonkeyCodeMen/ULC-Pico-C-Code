#pragma once
#include <Arduino.h>
#include "Led.hpp"
#include "Ani.hpp"
#include "helper.hpp"

class LedAni : public Ani
{
    
    public:
        LedAni(const char * pName) : Ani(pName) {};
        ~LedAni() = default;

        virtual void loop(u32_t time,Led * pLed) {};

    // base class
        //String getName()		{return _name;};
		//virtual void reset()  {};
        //virtual int setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,String str,u32_t length,u8_t ** pData) {};
        // if nothing to do for this member functions you can stay with the default implentation from base class

};

class LedOffAni : public LedAni{
      /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  off
        -------+---------------+---------------------------
        p1:    | N/A           |  N/A  
        -------+---------------+---------------------------
        p2:    | N/A           |  N/A     
        -------+---------------+---------------------------
        p3:    | N/A           |  N/A
        -------+---------------+---------------------------
        p4:    |               |  N/A
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */
    public:
        LedOffAni()  : LedAni((const char *) F("off"))        {};
        void loop(u32_t time,Led * pLed)                       {pLed->set(LED_OFF);};
};

class LedOnAni : public LedAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  on
        -------+---------------+---------------------------
        p1:    | N/A           |  N/A  
        -------+---------------+---------------------------
        p2:    | N/A           |  N/A     
        -------+---------------+---------------------------
        p3:    | N/A           |  N/A
        -------+---------------+---------------------------
        p4:    |               |  N/A
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */
    public:
        LedOnAni()  : LedAni((const char *) F("on"))          {};
        void loop(u32_t time,Led * pLed)                       {pLed->set(LED_MAX);};
};

class LedDimAni : public LedAni{
      /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  dim
        -------+---------------+---------------------------
        p1:    | 0x0000 0080   |  0x0000 00DD
               |               |  D: dim value  
        -------+---------------+---------------------------
        p2:    | N/A           |  N/A     
        -------+---------------+---------------------------
        p3:    | N/A           |  N/A
        -------+---------------+---------------------------
        p4:    |               |  N/A
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */

    public:
        LedDimAni()  : LedAni((const char *) F("dim"))        {};
        void reset()                                {setup(0x80,0,0,0,"",0,NULL);};
        void loop(u32_t time,Led * pLed)            {pLed->set(_dimValue);};
        int setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,String str,u32_t length,u8_t ** pData)  
                                                    {_dimValue = L_BYTE(p1);return ANI_OK;}; 
    private:
        u8_t _dimValue;
};

class LedBlinkAni : public LedAni{
      /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  blink
        -------+---------------+---------------------------
        p1:    | 0x0000 0080   |  0x0000 00DD
               |               |  D: dim value  
        -------+---------------+---------------------------
        p2:    | 250           |  on time in ms     
        -------+---------------+---------------------------
        p3:    | 250           |  off time in ms
        -------+---------------+---------------------------
        p4:    |               |  N/A
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */
    public:
        LedBlinkAni()  : LedAni((const char *) F("blink"))    {};
        
        void reset() {  setup(0x80,250,250,0,"",0,NULL); };
        int setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,String str,u32_t length,u8_t ** pData)  { 
            _state = stop;
            _dimValue = L_BYTE(p1);
            _onTime_ms = p2;
            _offTime_ms = p3;
            _state = init;
            return ANI_OK;
        };


        void loop(u32_t time,Led * pLed){
            u32_t diff;
            switch (_state){
                case stop: break;       // do nothing parameters are content of change
                case init:
                    _state = off;
                    pLed->set(LED_OFF);   
                    _lastSwitchTime = time;
                    break;
                
                case off:
                    diff = time-_lastSwitchTime;
                    if (diff >= _offTime_ms){
                        _state = on;
                        pLed->set(_dimValue);   
                        _lastSwitchTime = time;
                    }
                    break;
                
                case on:
                    diff = time-_lastSwitchTime;
                    if (diff >= _onTime_ms){
                        _state = off;
                        pLed->set(LED_OFF);   
                        _lastSwitchTime = time;
                    }
                    break;
            }
        };

    private:
        enum BlinkState {stop,init,on,off};
        BlinkState _state;
        u8_t _dimValue;
        u32_t _onTime_ms,_offTime_ms;
        u32_t _lastSwitchTime;
};

class LedMultiFlashAni : public LedAni{
      /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  dim
        -------+---------------+---------------------------
        p1:    | 0x0000 00FF   |  0x0000 00DD
               |               |  D: dim value  
        -------+---------------+---------------------------
        p2:    | 0x0           |  N/A
        -------+---------------+---------------------------
        p3:    | 0x0020 0060   |  0xAAAA BBBB
               |               |  A: on Time in ms       
               |               |  B: off Time in ms       
        -------+---------------+---------------------------
        p4:    | 0x0002 01F4   |  0xAAAA BBBB
               |               |  A: count of flash group
               |               |  B: pause time between flash group      
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */
    public:
        LedMultiFlashAni()  : LedAni((const char *) F("multi flash")) {};
        
        void reset() {  setup(0xFF,0,0x00200060,0x000201F4,"",0,NULL); };
        int setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,String str,u32_t length,u8_t ** pData)  {
            _state      = stop;
            _dim        = L_BYTE(p1);
            _onTime     = H_WORD(p3);
            _offTime    = L_WORD(p3);
            _flashCount = H_WORD(p4);
            _pauseTime  = L_WORD(p4);
            _state      = init;
            return ANI_OK;
        };
 
        void loop(u32_t time,Led * pLed){
            u32_t diff,color;
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state = flashOn;
                    _lastCallTime = time;
                    pLed->set(_dim);
                    break;
                
                case flashOn:
                    diff = time - _lastCallTime;
                    if (diff >= _onTime){
                        _lastCallTime = time;
                        pLed->set(0);
                        _count++;
                        if (_count >= _flashCount){
                            _state = pause;
                        } else {
                            _state = flashOff;
                        }
                    }
                    break;

                case flashOff:
                    diff = time - _lastCallTime;
                    if (diff >= _offTime){
                        _lastCallTime = time;
                        pLed->set(_dim);
                        _state = flashOn;
                    }
                    break;

                case pause:
                    diff = time - _lastCallTime;
                    if (diff >= _pauseTime){
                        _lastCallTime = time;
                        pLed->set(_dim);
                        _count = 0;
                        _state = flashOn;
                    }
                    break;
            }
        };


    private:
        enum MultiFlashState {stop,init,flashOn,flashOff,pause};
        volatile MultiFlashState _state;
        u32_t _lastCallTime;
        u8_t _dim;
        u16_t _onTime,_offTime;
        u16_t _pauseTime;
        u16_t _flashCount,_count;
};

class LedBreathAni : public LedAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  breath
        -------+---------------+---------------------------
        p1:    | N/A           |  N/A
        -------+---------------+---------------------------
        p2:    | 0x0000 1000   |  inc time in ms
        -------+---------------+---------------------------
        p3:    | 0x0000 1000   |  dec time in ms
        -------+---------------+---------------------------
        p4:    | 0x0000 FF10   |  0x0000 UULL
               |               |  U: upper dim limit
               |               |  B: lower dim limit
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */

    public:
        LedBreathAni()  : LedAni((const char *)F("breath")) {};
        
        void reset() {  setup(0,0x1000,0x1000,0x0000FF10,"",0,NULL); };
        int setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,String str,u32_t length,u8_t ** pData)  {
            _state = stop; 
            _incTimeMs = p2;
            _decTimeMs = p3;
            _upperLimit = H_BYTE(p4);
            _lowerLimit = L_BYTE(p4);
            if (_lowerLimit > _upperLimit){
                u8_t temp = _upperLimit;
                _upperLimit = _lowerLimit;
                _lowerLimit = temp;
            }
            _state = init;
            return ANI_OK;
        };
 
        void loop(u32_t time,Led * pLed){
            u32_t diff;
            u8_t dim;
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state = inc;
                    _lastSwitchTime = time;
                    dim = _lowerLimit;
                    _dimDiff = _upperLimit-_lowerLimit;
                    pLed->set(dim);
                    break;
                
                case inc:
                    diff = time-_lastSwitchTime;
                    if (diff >= _incTimeMs){
                        _lastSwitchTime = time;
                        _state = dec;
                        dim = _upperLimit;
                    } else {
                        dim = _lowerLimit + (diff * _dimDiff) / _incTimeMs;
                    }
                    pLed->set(dim);
                    break;
                
                case dec:
                    diff = time-_lastSwitchTime;
                    if (diff >= _decTimeMs){
                        _lastSwitchTime = time;
                        _state = inc;
                        dim = _lowerLimit;
                    } else {
                        dim = _upperLimit - (diff * _dimDiff) / _decTimeMs;
                    }
                    pLed->set(dim);
                    break;
            }
        };


    private:
        enum BreathState {stop,init,inc,dec};
        volatile BreathState _state;
        u32_t   _incTimeMs,_decTimeMs;
        u32_t   _lastSwitchTime;
        u8_t    _upperLimit,_lowerLimit;
        u8_t    _dimDiff;

};
