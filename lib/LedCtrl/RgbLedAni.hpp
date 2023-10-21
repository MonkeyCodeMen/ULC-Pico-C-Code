#pragma once
#include "Ani.hpp"
#include "RgbLed.hpp"
#include "helper.hpp"

class RgbLedAni:public Ani
{
	public:
		RgbLedAni(const char * pName) : Ani(pName) {};
		~RgbLedAni() = default;
		
		virtual void loop(u32_t time,RgbLed * pLed) {};

    // base class
        //String getName()		{return _name;};
		//virtual void reset()  {};
        //virtual void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData) {};
        // if nothing to do for this member functions you can stay with the default implentation from base class

};

class RgbLedOffAni : public RgbLedAni{
    public:
        RgbLedOffAni():RgbLedAni((const char *)F("off"))     {};
        void loop(u32_t time,RgbLed * pLed)                    {pLed->set(RGB_LED_OFF,RGB_LED_OFF,RGB_LED_OFF);};
};

class RgbLedOnAni : public RgbLedAni{
          /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  on
        -------+---------------+---------------------------
        p1:    | 0x00FF FFFF   |  0x00RR GGBB
               |               |  R: red value  
               |               |  G: green value  
               |               |  B: blue value  
        -------+---------------+---------------------------
        p2:    | 0x0           |  N/A
        -------+---------------+---------------------------
        p3:    | 0x0           |  N/A
        -------+---------------+---------------------------
        p4:    | 0x0           |  N/A
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */
    public:
        RgbLedOnAni()  : RgbLedAni((const char *)F("on"))   {};
        void reset()                                        {setup(0x00FFFFFF,0,0,0,0,NULL);};
        void loop(u32_t time,RgbLed * pLed)                 {pLed->set(_r,_g,_b);};
        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData) 
                                                            {_r=HH_BYTE(p1);_g=H_BYTE(p1);_b=L_BYTE(p1);};
	private:
		u8_t _r,_g,_b;
};

class RgbLedBlinkAni : public RgbLedAni{
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
        RgbLedBlinkAni()  : RgbLedAni((const char *)F("blink")) {};
        
        void reset()    { setup(0x0,0x00FFFFFF,250,250,0,NULL);  };
        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData)  {
            _state = stop; 
			_r1 = HH_BYTE(p1);
			_g1 = H_BYTE(p1);
			_b1 = L_BYTE(p1);
			_r2 = HH_BYTE(p2);
			_g2 = H_BYTE(p2);
			_b2 = L_BYTE(p2);

            _onTime1_ms = p3;
            _onTime2_ms = p4;
            _state = init;
        };

 
        void loop(u32_t time,RgbLed * pLed){
            u32_t diff;
            switch (_state){
                case stop:
                    // do nothing parameters are loopd by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state = state1;
                    pLed->set(_r1,_g1,_b1);   
                    _lastSwitchTime = time;
                    break;
                
                case state1:
                    diff = time-_lastSwitchTime;
                    if (diff > _onTime1_ms){
                        _state = state2;
                        pLed->set(_r2,_g2,_b2);   
                        _lastSwitchTime = time;
                    }
                    break;
                
                case state2:
                    diff = time-_lastSwitchTime;
                    if (diff > _onTime2_ms){
                        _state = state1;
                        pLed->set(_r1,_g1,_b1);   
                        _lastSwitchTime = time;
                    }
                    break;
            }
        };

    private:
        enum BlinkState {stop,init,state1,state2};
        volatile BlinkState _state;
        u8_t _r1,_g1,_b1,_r2,_g2,_b2;
        int _onTime1_ms,_onTime2_ms;
        u32_t _lastSwitchTime;
};

class RgbLedBreathAni : public RgbLedAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  dim
        -------+---------------+---------------------------
        p1:    | 0x0000 00FF   |  0x00RR GGBB
               |               |  R: red value  
               |               |  G: green value  
               |               |  B: blue value  
        -------+---------------+---------------------------
        p2:    | 0x0000 0130   |  inc time in ms
        -------+---------------+---------------------------
        p2:    | 0x0000 0130   |  dec time in ms
        -------+---------------+---------------------------
        -------+---------------+---------------------------
        p4:    | 0x0000 FF10   |  0x0000 UULL
               |               |  U: upper dim limit
               |               |  B: lower dim limit
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */

    public:
        RgbLedBreathAni()  : RgbLedAni((const char *)F("breath")) {};
        
        void reset() {  setup(0x000000FF,0x130,0x130,0x0000FF10,0,NULL); };
        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData)  {
            _state = stop; 
            _r = HH_BYTE(p1);
            _g = H_BYTE(p1);
            _b = L_BYTE(p1);
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
        };
 
        void loop(u32_t time,RgbLed * pLed){
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
                    pLed->set(dimColorChannel255(_r,dim),dimColorChannel255(_g,dim),dimColorChannel255(_b,dim));
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
                    pLed->set(dimColorChannel255(_r,dim),dimColorChannel255(_g,dim),dimColorChannel255(_b,dim));
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
                    pLed->set(dimColorChannel255(_r,dim),dimColorChannel255(_g,dim),dimColorChannel255(_b,dim));
                    break;
            }
        };


    private:
        enum BreathState {stop,init,inc,dec};
        volatile BreathState _state;
        u32_t   _incTimeMs,_decTimeMs;
        u8_t    _r,_g,_b;
        u32_t   _lastSwitchTime;
        u8_t    _upperLimit,_lowerLimit;
        u8_t    _dimDiff;

};

class RgbLedRainbowAni : public RgbLedAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  dim
        -------+---------------+---------------------------
        p1:    | 0x0000 00FF   |  0x00RR GGBB
               |               |  R: red value  
               |               |  G: green value  
               |               |  B: blue value  
        -------+---------------+---------------------------
        p2:    | 0x0000 0130   |  inc time in ms
        -------+---------------+---------------------------
        p2:    | 0x0000 0130   |  dec time in ms
        -------+---------------+---------------------------
        -------+---------------+---------------------------
        p4:    | 0x0000 FF10   |  0x0000 UULL
               |               |  U: upper dim limit
               |               |  B: lower dim limit
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */    public:
        RgbLedRainbowAni()  : RgbLedAni((const char *)F("rainbow")) {};
        
        void reset() {
            _state      = stop;
            _dim        = 128;
            _wheelIndex = 0;
            _wheelInc   = 1;
            _timeInc    = 100;
            _state      = init;
        };
 
        void loop(u32_t time,RgbLed * pLed){
            u32_t diff,color;
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state = run;
                    _lastCallTime = time;
                    _wheelIndex = 0;
                    color = getColorWheel24Bit(_wheelIndex);
                    pLed->set(dimColorChannel255(_r,_dim),dimColorChannel255(_g,_dim),dimColorChannel255(_b,_dim));
                    break;
                
                case run:
                    diff = time - _lastCallTime;
                    if (diff >= _timeInc){
                        _lastCallTime = time;
                        _wheelIndex += _wheelInc;
                    }
                    color = getColorWheel24Bit(_wheelIndex);
                    pLed->set(color,_dim);
                    break;
            }
        };

        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData)  {
            _state      = stop;
            _dim        = L_BYTE(p1);
            _wheelInc   = L_BYTE(p2);
            _wheelIndex = H_BYTE(p2);
            _timeInc    = clamp(1,p3,100000);
            _state      = init;
        };

    private:
        enum RainbowState {stop,init,run};
        volatile RainbowState _state;
        u16_t _dim;
        u8_t  _wheelIndex,_wheelInc;
        u32_t _timeInc,_lastCallTime;
};

class RgbLedMultiFlashAni : public RgbLedAni{
    public:
        RgbLedMultiFlashAni()  : RgbLedAni((const char *)F("multi flash")) {};
        
        void reset() {
            // Blaulicht Doppelblitz: 500ms, ~25ms An, ~75ms Aus, ~25ms An (Aus Diagramm oben abgelesen)
            _state      = stop;
            _dim        = 128;
            _color      = 0x000000FF;
            _count      = 0;
            _flashCount = 2;
            _onTime     = 25;
            _offTime    = 75;
            _pauseTime  = 500;
            _state      = init;
        };
 
        void loop(u32_t time,RgbLed * pLed){
            u32_t diff,color;
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state = flashOn;
                    _lastCallTime = time;
                    pLed->set(_color,_dim);
                    break;
                
                case flashOn:
                    diff = time - _lastCallTime;
                    if (diff >= _onTime){
                        _lastCallTime = time;
                        pLed->set(0,0,0);
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
                        pLed->set(_color,_dim);
                        _state = flashOn;
                    }
                    break;

                case pause:
                    diff = time - _lastCallTime;
                    if (diff >= _pauseTime){
                        _lastCallTime = time;
                        pLed->set(_color,_dim);
                        _count = 0;
                        _state = flashOn;
                    }
                    break;


            }
        };

        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData)  {
            _state      = stop;
            _dim        = L_BYTE(p1);
            _color      = p2 & 0x00FFFFFF;
            _onTime     = H_WORD(p3);
            _offTime    = L_WORD(p3);
            _flashCount = H_WORD(p4);
            _pauseTime  = L_WORD(p4);
            _state      = init;
        };

    private:
        enum MultiFlashState {stop,init,flashOn,flashOff,pause};
        volatile MultiFlashState _state;
        u32_t _lastCallTime;
        u16_t _dim;
        u16_t _onTime,_offTime;
        u16_t _pauseTime;
        u16_t _flashCount,_count;
        u32_t _color;

};







