#pragma once
#include "Ani.hpp"
#include "RgbLed.hpp"
#include "helper.hpp"

class RgbLedAni:public Ani
{
	public:
		RgbLedAni(String name) : Ani(name) {};
		~RgbLedAni() = default;
		
		virtual void loop(RgbLed * pLed) {};

    // base class
        //String getName()		{return _name;};
		//virtual void reset()  {};
        //virtual void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData) {};
        // if nothing to do for this member functions you can stay with the default implentation from base class

};

class RgbLedOffAni : public RgbLedAni{
    public:
        RgbLedOffAni():RgbLedAni(String("off"))     {};
        void loop(u32_t time,RgbLed * pLed)                    {pLed->set(RGB_LED_OFF,RGB_LED_OFF,RGB_LED_OFF);};
};

class RgbLedOnAni : public RgbLedAni{
    public:
        RgbLedOnAni()  : RgbLedAni(String("on"))    {};
        void reset()                                {_value = RgbLed::pack(RGB_LED_MAX,RGB_LED_MAX,RGB_LED_MAX);};
        void loop(RgbLed * pLed)                    {pLed->set(_value);};
        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData) 
                                                    {_value = 0x00FFFFFF & p1;};
	private:
		u32_t _value;
};

class RgbLedBlinkAni : public RgbLedAni{
    public:
        RgbLedBlinkAni()  : RgbLedAni(String("blink")) {};
        
        void reset() {
            _state=init;
            _onTime1_ms =250;
            _onTime2_ms = 250; 
            u8_t onValue = 25;
            _value1 = RgbLed::pack(onValue,onValue,onValue);
            _value2 = RgbLed::pack(RGB_LED_OFF,RGB_LED_OFF,RGB_LED_OFF);
        };
 
        void loop(RgbLed * pLed){
            u32_t diff;
            switch (_state){
                case stop:
                    // do nothing parameters are loopd by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state = state1;
                    pLed->set(_value1);   
                    _lastSwitchTime = millis();
                    break;
                
                case state1:
                    diff = millis()-_lastSwitchTime;
                    if (diff > _onTime1_ms){
                        _state = state2;
                        pLed->set(_value2);   
                        _lastSwitchTime = millis();
                    }
                    break;
                
                case state2:
                    diff = millis()-_lastSwitchTime;
                    if (diff > _onTime2_ms){
                        _state = state1;
                        pLed->set(_value1);   
                        _lastSwitchTime = millis();
                    }
                    break;
            }
        };

        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData)  {
            _state = stop; 
			_value1 = p1 & 0x00FFFFFF;
			_value2 = p2 & 0x00FFFFFF;

            _onTime1_ms = p3;
            _onTime2_ms = p4;
            _state = init;

        };

    private:
        enum BlinkState {stop,init,state1,state2};
        volatile BlinkState _state;
        u32_t _value1,_value2;
        int _onTime1_ms,_onTime2_ms;
        u32_t _lastSwitchTime;
};

class RgbLedBreathAni : public RgbLedAni{
    public:
        RgbLedBreathAni()  : RgbLedAni(String("breath")) {};
        
        void reset() {
            _state=stop;
            _incPerMs = 10;
            _decPerMs = 10; 
            _targetR = 0;
            _targetG = 0;
            _targetB = 255;
            _upperLimit = 20000;
            _lowerLimit = 1000;
            _dim = 0;
            _state=init;
        };
 
        void loop(RgbLed * pLed){
            u32_t diff,time;
            time = millis();
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state = inc;
                    _lastCallTime = time;
                    pLed->set(_targetR,_targetG,_targetB,_dim);
                    break;
                
                case inc:
                    diff = time-_lastCallTime;
                    _lastCallTime = time;
                    _dim += diff * _incPerMs;
                    if (_dim >= _upperLimit){
                        _dim = _upperLimit;
                        _state = dec;
                    }
                    pLed->set(_targetR,_targetG,_targetB,_dim);
                    break;
                
                case dec:
                    diff = time-_lastCallTime;
                    _lastCallTime = time;
                    _dim -= diff * _decPerMs;
                    if (_dim <= _lowerLimit){
                        _dim = _lowerLimit;
                        _state = inc;
                    }
                    pLed->set(_targetR,_targetG,_targetB,_dim);
                    break;
            }
        };

        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData)  {
            _state = stop; 
            _targetR = RgbLed::unpackR(p1);
            _targetG = RgbLed::unpackG(p1);
            _targetB = RgbLed::unpackB(p1);
            _incPerMs = p2;
            _decPerMs = p3;
            _lowerLimit = (p4 & 0xFFFF0000) >> 16;
            _upperLimit = p4 & 0x0000FFFF;
            _upperLimit =  clamp(1,_upperLimit,RGB_DIM_ACCURACY);
            _lowerLimit = clamp (0,_lowerLimit, _upperLimit);
            _dim = 0;
            _state = init;

        };

    private:
        enum BreathState {stop,init,inc,dec};
        volatile BreathState _state;
        u32_t _incPerMs,_decPerMs;
        u8_t _targetR,_targetG,_targetB;
        u32_t _lastCallTime;
        s32_t _dim;
        u32_t _upperLimit,_lowerLimit;
};

class RgbLedRainbowAni : public RgbLedAni{
    public:
        RgbLedRainbowAni()  : RgbLedAni(String("rainbow")) {};
        
        void reset() {
            _state      = stop;
            _dim        = RGB_DIM_ACCURACY/2;
            _wheelIndex = 0;
            _wheelInc   = 1;
            _timeInc    = 100;
            _state      = init;
        };
 
        void loop(RgbLed * pLed){
            u32_t diff,time,color;
            time = millis();
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state = run;
                    _lastCallTime = time;
                    _wheelIndex = 0;
                    color = get888ColorWheel(_wheelIndex);
                    pLed->set(color,_dim);
                    break;
                
                case run:
                    diff = time - _lastCallTime;
                    if (diff >= _timeInc){
                        _lastCallTime = time;
                        _wheelIndex += _wheelInc;
                    }
                    color = get888ColorWheel(_wheelIndex);
                    pLed->set(color,_dim);
                    break;
            }
        };

        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData)  {
            _state      = stop;
            _dim        = clamp(0,p1,RGB_DIM_ACCURACY);
            _wheelInc   = p2 & 0xFF;
            _wheelIndex = (p2 & 0xFF00) >> 8;
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
        RgbLedMultiFlashAni()  : RgbLedAni(String("multi flash")) {};
        
        void reset() {
            // Blaulicht Doppelblitz: 500ms, ~25ms An, ~75ms Aus, ~25ms An (Aus Diagramm oben abgelesen)
            _state      = stop;
            _dim        = RGB_DIM_ACCURACY/2;
            _color      = 0x000000FF;
            _count      = 0;
            _flashCount = 2;
            _onTime     = 25;
            _offTime    = 75;
            _pauseTime  = 500;
            _state      = init;
        };
 
        void loop(RgbLed * pLed){
            u32_t diff,time,color;
            time = millis();
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
            _dim        = clamp(0,p1,RGB_DIM_ACCURACY);
            _color      = p2 & 0x00FFFFFF;
            _onTime     = (p3 & 0xFFFF0000) >> 16;
            _offTime    = p3 & 0x0000FFFF;
            _flashCount = (p4 & 0xFFFF0000) >> 16;
            _pauseTime  = p4 & 0x0000FFFF;
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







