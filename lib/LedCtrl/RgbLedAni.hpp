#pragma once
#include "Ani.hpp"
#include "RgbLed.hpp"
class RgbLedAni:public Ani
{
	public:
		RgbLedAni(String name) : Ani(name) {};
		~RgbLedAni() = default;
		
		virtual void loop(RgbLed * pLed) {};
};


class RgbLedOffAni : public RgbLedAni{
    public:
        RgbLedOffAni()  : RgbLedAni(String("off"))  {};
        void reset()                                {};
        void loop(RgbLed * pLed)                  {pLed->set(RGB_LED_OFF,RGB_LED_OFF,RGB_LED_OFF);};
        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData) {};
};

class RgbLedOnAni : public RgbLedAni{
    public:
        RgbLedOnAni()  : RgbLedAni(String("on"))  {_value = RgbLed::pack(RGB_LED_MAX,RGB_LED_MAX,RGB_LED_MAX);};
        void reset()                                {};
        void loop(RgbLed * pLed)                  {pLed->set(_value);};
        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData) {_value = 0x00FFFFFF & p1;};
	private:
		u32_t _value;
};


class RgbLedBlinkAni : public RgbLedAni{
    public:
        RgbLedBlinkAni()  : RgbLedAni(String("blink")) {   
            _state=init;
            _onTime1_ms =250;
            _onTime2_ms = 250; 
            u8_t onValue = 25;
            _value1 = RgbLed::pack(onValue,onValue,onValue);
            _value2 = RgbLed::pack(RGB_LED_OFF,RGB_LED_OFF,RGB_LED_OFF);
        };
        
        void reset() {
            _state=init;
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
    #define BREATH_ACCURACY ((u32_t)(100*1000))

    public:
        RgbLedBreathAni()  : RgbLedAni(String("breath")) {   
            _state=init;
            _incPerMs = 10;
            _decPerMs = 10; 
            _targetR = 0;
            _targetG = 0;
            _targetB = 255;
            _upperLimit = 20000;
            _lowerLimit = 1000;
            _dim = 0;
        };
        
        void reset() {
            _state=init;
            _dim = 0;
        };
 
        void loop(RgbLed * pLed){
            u32_t diff,time;
            time = millis();
            switch (_state){
                case stop:
                    // do nothing parameters are loopd by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state = inc;
                    _lastCallTime = time;
                    set(pLed);
                    break;
                
                case inc:
                    diff = time-_lastCallTime;
                    _lastCallTime = time;
                    _dim += diff * _incPerMs;
                    if (_dim >= _upperLimit){
                        _dim = _upperLimit;
                        _state = dec;
                    }
                    set(pLed);
                    break;
                
                case dec:
                    diff = time-_lastCallTime;
                    _lastCallTime = time;
                    _dim -= diff * _decPerMs;
                    if (_dim <= _lowerLimit){
                        _dim = _lowerLimit;
                        _state = inc;
                    }
                    set(pLed);
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
            if (_upperLimit > BREATH_ACCURACY)  _upperLimit = BREATH_ACCURACY;
            if (_lowerLimit > _upperLimit)      _lowerLimit = 0;
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

        void set(RgbLed * pLed){
            u8_t r,g,b;
            r = (u8_t)(((u32_t)_targetR*_dim)/BREATH_ACCURACY);
            g = (u8_t)(((u32_t)_targetG*_dim)/BREATH_ACCURACY);
            b = (u8_t)(((u32_t)_targetB*_dim)/BREATH_ACCURACY);
            pLed->set(r,g,b);
        };
};
