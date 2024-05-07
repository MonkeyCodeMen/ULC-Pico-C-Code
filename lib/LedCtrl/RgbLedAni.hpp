#pragma once
#include "Ani.hpp"
#include "RgbLed.hpp"
#include "helper.h"
#include "ColorSelector.hpp"

class RgbLedAni:public Ani
{
	public:
		RgbLedAni(const char * pName) : Ani(pName) {}
		~RgbLedAni() = default;

		virtual void loop(uint32_t time,RgbLed * pLed) {}
};

class RgbLedOffAni : public RgbLedAni{
    public:
        RgbLedOffAni():RgbLedAni("off")     {}
        void loop(uint32_t time,RgbLed * pLed)         {pLed->set(RGB_LED_OFF,RGB_LED_OFF,RGB_LED_OFF);}
};

class RgbLedOnAni : public RgbLedAni{
    public:
        RgbLedOnAni()  : RgbLedAni("on")   {};
        void reset()                                        {setup(0x00FFFFFF,0,0,0,"",0,NULL);}
        void loop(uint32_t time,RgbLed * pLed)                 {pLed->set(_r,_g,_b);}
        int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData) 
                                                            {_r=HH_BYTE(p1);_g=H_BYTE(p1);_b=L_BYTE(p1);return ANI_OK;}
	private:
		uint8_t _r,_g,_b;
};

class RgbLedBlinkAni : public RgbLedAni{
      /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  blink
        -------+---------------+---------------------------
        p1:    | 0x0000 0000   |  0x00RR GGBB
               |               |  R: red value 1 
               |               |  G: green value 1 
               |               |  B: blue value 1
        -------+---------------+---------------------------
        p2:    | 0x00FF FFFF   |  0x00RR GGBB
               |               |  R: red value 2 
               |               |  G: green value 2 
               |               |  B: blue value 2
        -------+---------------+---------------------------
        p3:    | 250           |  on time 1 in ms
        -------+---------------+---------------------------
        p4:    | 250           |  on time 2 in ms
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */
    public:
        RgbLedBlinkAni()  : RgbLedAni("blink") {}
        
        void reset()    { setup(0x0,0x00FFFFFF,250,250,"",0,NULL);  }
        int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData)  {
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
            return ANI_OK;
        }
 
        void loop(uint32_t time,RgbLed * pLed){
            uint32_t diff;
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
        }

    private:
        enum BlinkState {stop,init,state1,state2};
        volatile BlinkState _state;
        uint8_t _r1,_g1,_b1,_r2,_g2,_b2;
        int _onTime1_ms,_onTime2_ms;
        uint32_t _lastSwitchTime;
};

class RgbLedBreathAni : public RgbLedAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  breath
        -------+---------------+---------------------------
        p1:    | 0x000A 2020   |  0xSSSS UUDD
               |               |  S: step time in ms
               |               |  U: nr steps for dim up
               |               |  D: nr steps for dim down
        -------+---------------+---------------------------
        p2:    | 0x0000 FF10   |  0x0000 UULL
               |               |  U: upper dim limit
               |               |  B: lower dim limit
        -------+---------------+---------------------------
        p3:    | 0x0000 0001   |  0x00SS 00II
               |               |  S:start index for color wheel/list
               |               |  I:inc step for color wheel/list (full wheel 255)
        -------+---------------+---------------------------
        p4     | N/A           | N/A
        -------+---------------+---------------------------
        str    | ""            | color list
        -------+---------------+---------------------------
        pData: | N/A           |  length (multiple of 4),pData:
               |               |    color list       


        breath blue only:
        setup(0x00204040,0xFF10,0,0,"0x000000FF",0,NULL)

        breath rainbow:
        setup(0x00204040,0x0000FF10,1,0,"",0,NULLL)

        breath color list:
        setup(0x02000404,0x0000FF10,1,0,"0xFFFFFF,0x00FF0000,0x00FF00,0x0000FF",0,NULL)

    */

    public:
        RgbLedBreathAni()  : RgbLedAni("breath") {}
        
        void reset() {  setup(0x00204040,0x0000FF10,1,0,"",0,NULL); }
        int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData)  {
            _state = stop; 
            _stepTime  = H_WORD(p1);
            _upSteps   = H_BYTE(p1);
            _downSteps = L_BYTE(p1);
            _upperLimit = H_BYTE(p2);
            _lowerLimit = L_BYTE(p2);
            _colorGen.setup(255,HH_BYTE(p3),L_BYTE(p3),str,length,*pData);

            // do some basic checks/correction of parameter set
            if (_lowerLimit > _upperLimit){
                uint8_t temp = _upperLimit;
                _upperLimit = _lowerLimit;
                _lowerLimit = temp;
            }
            if (_upSteps == 0)      _upSteps = 1;
            if (_downSteps == 0)    _downSteps = 1;
            if (_stepTime == 0)     _stepTime = 1;

            _state = init;
            return ANI_OK;
        }
 
        void loop(uint32_t time,RgbLed * pLed){
            uint32_t diff,color24;
            uint8_t dim;
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state       = up;
                    _stepCounter = 0;
                    _lastUpdate  = time;
                    _dimDiff     = _upperLimit-_lowerLimit;
                    dim          = _lowerLimit;
                    color24      = _colorGen.getNextColor(dim);
                    pLed->set(color24);
                    break;
                
                case up:
                    diff = time-_lastUpdate;
                    if (diff >= _stepTime){
                        _lastUpdate = time;
                        if (_stepCounter >= _upSteps){
                            dim = _upperLimit;
                            _stepCounter=0;    
                            _state = down;
                        } else {
                            dim = _lowerLimit + (_stepCounter * _dimDiff) / _upSteps;
                            _stepCounter++;
                        }
                        color24  = _colorGen.getNextColor(dim);
                        pLed->set(color24);
                    } 
                    break;
                
                case down:
                    diff = time-_lastUpdate;
                    if (diff >= _stepTime){
                        _lastUpdate = time;
                        if (_stepCounter >= _downSteps){
                            dim = _lowerLimit;
                            _stepCounter=0;    
                            _state = up;
                        } else {
                            dim = _upperLimit - (_stepCounter * _dimDiff) / _downSteps;
                            _stepCounter++;
                        }
                        color24  = _colorGen.getNextColor(dim);
                        pLed->set(color24);
                    } 
            }
        }
    
    private:
        enum BreathState {stop,init,up,down};
        volatile BreathState _state;
        uint32_t   _stepTime,_upSteps,_downSteps;
        uint32_t   _stepCounter,_lastUpdate;
        uint8_t    _upperLimit,_lowerLimit;
        uint8_t    _dimDiff;
        ColorCtrl _colorGen;
};

class RgbLedRainbowAni : public RgbLedAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  dim
        -------+---------------+---------------------------
        p1:    | 0x0000 0040   |  step time in ms
        -------+---------------+---------------------------
        p2:    | N/A           |  N/A
        -------+---------------+---------------------------
        p3:    | 0x0000 FE04   |  0x00SS DDII
               |               |  S:start index for color wheel/list
               |               |  D: dim level 
               |               |  I:inc step for color wheel/list (full wheel 255)
        -------+---------------+---------------------------
        p4     | N/A           | N/A
        -------+---------------+---------------------------
        str    | ""            | color list
        -------+---------------+---------------------------
        pData: | N/A           |  length (multiple of 4),pData:
               |               |    color list       
    */    
    public:
        RgbLedRainbowAni()  : RgbLedAni("rainbow") {}
        
        void reset() {  setup(0x0040,0,0xFE01,0,"",0,NULL); }
        int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData)  {
            _state      = stop;
            _timeInc    = clamp(1,p1,100000);
            _colorGen.setup(H_BYTE(p3) /* dim */,HH_BYTE(p3) /* start */,L_BYTE(p3)/* step */,str,length,*pData);            
            _state      = init;
            return ANI_OK;
        }
 
        void loop(uint32_t time,RgbLed * pLed){
            uint32_t diff;
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state = run;
                    _lastCallTime = time;
                    pLed->set(_colorGen.getNextColor());
                    break;
                
                case run:
                    diff = time - _lastCallTime;
                    if (diff >= _timeInc){
                        _lastCallTime = time;
                        pLed->set(_colorGen.getNextColor());
                    }
                    break;
            }
        }


    private:
        enum RainbowState {stop,init,run};
        volatile RainbowState _state;
        uint32_t _timeInc,_lastCallTime;
        ColorCtrl _colorGen;
};

class RgbLedMultiFlashAni : public RgbLedAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  dim
        -------+---------------+---------------------------
        p1:    | 0x0020 0060   |  0xAAAA BBBB
               |               |  A: on Time in ms       
               |               |  B: off Time in ms       
        -------+---------------+---------------------------
        p2:    | 0x0002 01F4   |  0xAAAA BBBB
               |               |  A: count of flash group
               |               |  B: pause time between flash group      
        -------+---------------+---------------------------
        p3:    | 0x0000 FF01   |  0x00SS DDII
               |               |  S:start index for color wheel/list
               |               |  D: dim level 
               |               |  I:inc step for color wheel/list (full wheel 255)
        -------+---------------+---------------------------
        p4     | N/A           | N/A
        -------+---------------+---------------------------
        str    | color list    | color list
               | "0xFFFFFF,0x00FF0000,0x00FF00,0x0000FF"  
        -------+---------------+---------------------------
        pData: | N/A           |  length (multiple of 4),pData:
               |               |    color list       
    */    
    public:
        RgbLedMultiFlashAni()  : RgbLedAni("multi flash") {}
        
        void reset() { setup(0x00200060,0x000201F4,0xFF01,0,"0xFFFFFF,0x00FF0000,0x00FF00,0x0000FF",0,NULL);}
        int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData)  {
            _state      = stop;
            _onTime     = H_WORD(p1);
            _offTime    = L_WORD(p1);
            _flashCount = H_WORD(p2);
            _pauseTime  = L_WORD(p2);
            _colorGen.setup(H_BYTE(p3),HH_BYTE(p3),L_BYTE(p3),str,length,*pData);   
            _state      = init;
            return ANI_OK;
        }
 
        void loop(uint32_t time,RgbLed * pLed){
            uint32_t diff,color;
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state = flashOn;
                    _lastCallTime = time;
                    pLed->set(_colorGen.getNextColor());
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
                        pLed->set(_colorGen.getNextColor());
                        _state = flashOn;
                    }
                    break;

                case pause:
                    diff = time - _lastCallTime;
                    if (diff >= _pauseTime){
                        _lastCallTime = time;
                        pLed->set(_colorGen.getNextColor());
                        _count = 0;
                        _state = flashOn;
                    }
                    break;
            }
        }


    private:
        enum MultiFlashState {stop,init,flashOn,flashOff,pause};
        volatile MultiFlashState _state;
        uint32_t _lastCallTime;
        uint16_t  _onTime,_offTime;
        uint16_t  _pauseTime;
        uint16_t  _flashCount,_count;
        ColorCtrl _colorGen;

};







