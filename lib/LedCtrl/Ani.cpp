#include <Ani.hpp>


///////////////////////////////////////////////////
// DimCtrl

void DimCtrl::config(uint32_t p=0){
    uint8_t soll = L_BYTE(p);
    int8_t  delta  = H_BYTE(p);

    if (delta == 0){
        _dim = soll;
    } else {
        int16_t temp = _dim;
        temp += delta;
        _dim = (uint8_t) clamp(0,temp,255);
    }
    _speed = (p >> 16) & 0x0FFF;
}


///////////////////////////////////////////////////
// ColorCtrl

void ColorCtrl::config(uint8_t startIndex,uint8_t incStep,uint8_t time, String colorList){
    _run = false;
    _colorList.clear();
    _incStep   = (int8_t) incStep;
    _timeStep  = time;
    _firstLoop = true;
    _currentColorIndex = startIndex;
    _currentColor = 0x00FFFFFF; 
    _secondColor  = 0x0;
    _colorIndexMax = 255;
    _useColorWheel = true;

    if (colorList.length() > 0){
        StringList list(colorList.c_str(),',');
        while(list.isEndReached() == false){
            _colorList.add(convertStrToInt(list.getNextListEntry()));
        }
        _colorIndexMax = _colorList.size();
        if (_colorIndexMax > 0){
            _currentColor = _colorList.get(0);
            _useColorWheel = false;
            if (_colorIndexMax > 1){
                _secondColor = _colorList.get(1);
            }
        }
    }
    _run = (incStep == 0) ? false:true;
}

void ColorCtrl::loop(uint32_t now){
    if (_run == true){
        // check for first loop cycle
        if (_firstLoop == true){
            _firstLoop = false;
            _nextLoopTime = now+_timeStep;
        } else if (now >= _nextLoopTime){
            // time for update
            _nextLoopTime += _timeStep;
            
            // update Index
            _currentColorIndex += _incStep;
            _currentColorIndex %= _colorIndexMax;
            
            // now calc color based on index
            if (_useColorWheel == true){
                _currentColor = getColorWheel24Bit(_currentColorIndex);
            } else {
                _currentColor = _colorList.get(_currentColorIndex);
            }
        }
    }
}

///////////////////////////////////////////////////
// FlashCtrl

void FlashCtrl::config(uint8_t p=0){
    _state        = stop;
    _triggerActive= false;
    _groupCount   = L_BYTE(p);
    _groupCounter = 0;
    _t1           = H_BYTE(p);
    _t2           = HH_BYTE(p);
    _t3 		  = HHH_BYTE(p);
    if (_t3 == 0xFF){
        _waitForTrigger = true;
    } else {
        _waitForTrigger = false;
        _t3 = _t3*100;
    }
    if (_groupCount > 0){
        _state = init;
    }
}

void FlashCtrl::loop(uint32_t now){
    switch (_state){
        case stop:
            // do nothing parameters are loocked by other thread
            // the setup will move forward to state init
            break;

        case init:
            _state = flashOn;
            _nextLoopTime = now+_t1;
            break;
        
        case flashOn:
            if (now >= _nextLoopTime){
                _groupCounter++;
                if (_groupCounter >= _groupCount){
                    if (_waitForTrigger == true){
                        _state = waitTrigger;
                    } else {
                        _state = pause;
                        _nextLoopTime += _t3;
                    }
                } else {
                    _state = flashOff;
                    _nextLoopTime += _t2;
                }
            }
            break;

        case flashOff:
            if (now >= _nextLoopTime){
                _state = flashOn;
                _nextLoopTime += _t1;
            }
            break;

        case pause:
            if (now >= _nextLoopTime){
                _state = flashOn;
                _groupCounter = 0;
                _nextLoopTime += _t1;
            }
            break;

        case waitTrigger:
            if (_triggerActive == true){
                _state = flashOn;
                _groupCounter = 0;
                now += _t1;
                _triggerActive = false;
            }
            break;
    }
}


///////////////////////////////////////////////////
// BreathCtrl

void BreathCtrl::config(uint32_t p=0){
    _state = stop;
    _dimDelta = 0;
    _target = L_BYTE(p);
    _t5 = (p >> 8)  & 0x0FFF;
    _t6 = (p >> 20) & 0x0FFF;

    if (_dimDelta > 0) { 
        _state = init;
    }
}

void BreathCtrl::loop(uint32_t now){
    uint32_t elapsed,value;
    switch(_state){
        case stop:
            break;

        case init:
            _dimDelta = 0;
            _lastTurn = now;
            _state = up;
            break;

        case up:
            elapsed = now-_lastTurn;
            if (elapsed >= _t5){
                _state = down;
                _lastTurn = now;
                elapsed = clamp(0,elapsed,_t5);
            }
            value = (_target * elapsed) / _t5;
            _dimDelta = clamp(0,value,_target);
            break;

        case down:
            elapsed = now-_lastTurn;
            if (elapsed >= _t6){
                _state = up;
                _lastTurn = now;
                elapsed = clamp(0,elapsed,_t6);
            }
            value = (_target * elapsed) / _t6;
            _dimDelta = clamp(0,_target - value,_target);
            break;
    }
}

///////////////////////////////////////////////////
// Ani Base class


int Ani::config(AniCfg cfg) { 
    _cfg = cfg; 
    uint8_t ctrlByte = HHH_BYTE(cfg._p1);
    if (ctrlByte & DIM_MASK)	_dimCtrl.config(cfg._p1);
    if (ctrlByte & COLOR_MASK)	_colorCtrl.config(cfg._p2,cfg._str);
    if (ctrlByte & FLASH_MASK)	_flashCtrl.config(cfg._p3);
    if (ctrlByte & DARTH_VADER)	_breathCtrl.config(cfg._p4);
    return ANI_OK;    
}

void Ani::loop(uint32_t now) {	
    _dimCtrl.loop(now);
    _colorCtrl.loop(now);							
    _flashCtrl.loop(now);
    _breathCtrl.loop(now);
}

const char * Ani::getErrorText(int error)		{
    switch (error){
        case ANI_OK:							return "OK";
        case ANI_ERROR_GENERAL:					return "general error";
        case ANI_ERROR_PROGRAM_DOES_NOT_EXIST:	return "program does not exist";
        case ANI_ERROR_OUT_OF_RANGE:			return "parameter out of range";
        case ANI_ERROR_INTERNAL:				return "internal error";
        case ANI_ERROR_PAR_MISSING:				return "parameter missing";
        case ANI_ERROR_FILE_NOT_FOUND:			return "file not found";
        case ANI_ERROR_OUT_OF_MEMORY:			return "out of memory";
    }
    return "unknow error code";
}

