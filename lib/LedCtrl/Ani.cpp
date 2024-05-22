#include <Ani.hpp>


///////////////////////////////////////////////////
// Ani Base class


void Ani::reset() {	
    config(AniCfg(ANI_WR_DIM|ANI_WR_COLOR|ANI_WR_FLASH|ANI_DARTH_VADER,0,0,0,"0"));	

    _color      = _colorCtrl.getColor();
    _colorLast  = _color;
    _dim        = _breathCtrl.modifyDimFactor(_dimCtrl.getDim());
    _dimLast    = _dim;
    
}


int Ani::config(AniCfg cfg) { 
    if (cfg.dimCfg.reg.WR_dim    == 1)	_dimCtrl.config(cfg.dimCfg);
    if (cfg.dimCfg.reg.WR_color  == 1)	_colorCtrl.config(cfg.colorCfg,cfg.str);
    if (cfg.dimCfg.reg.WR_flash  == 1)	_flashCtrl.config(cfg.flashCfg);
    if (cfg.dimCfg.reg.WR_breath == 1)	_breathCtrl.config(cfg.breathCfg);
    return ANI_OK;    
}

void Ani::loop(uint32_t now) {	
    _dimCtrl.loop(now);
    if (_flashCtrl.loop(now) == true) {
        _colorCtrl.trigger();
    }
    _colorCtrl.loop(now);							
    _breathCtrl.loop(now);

    _color = _colorCtrl.getColor();
    _dim = _breathCtrl.modifyDimFactor(_dimCtrl.getDim());
}



bool Ani::hasChanged(){
    if ((_color != _colorLast) || (_dim != _dimLast)){
        _colorLast = _color;
        _dimLast = _dim;
        return true;
    }
    return false;
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



///////////////////////////////////////////////////
// DimCtrl

void DimCtrl::config(dimCtrl_t cfg)
{
    if (cfg.reg.incValue == 0){
        _dim = cfg.reg.setValue;
    } else {
        int32_t temp = _dim;
        temp += cfg.reg.incValue;
        _dim = clamp(0,temp,255);
    }
    _speed = cfg.reg.speed;
}


///////////////////////////////////////////////////
// ColorCtrl

void ColorCtrl::config(colorCtrl_t cfg,String str){
    _state = stop;
    _incStep   = cfg.reg.incValue;
    _timeStep  = cfg.reg.t2_ms;
    _triggerActive = false;
    _eventCounter = 0;
    _eventTarget = cfg.reg.eventCounter+1;

    // setup color list
    _colorList.clear();         // clear old list
    if (str.length() > 0){
        StringList list(str.c_str(),',');
        while(list.isEndReached() == false){
            _colorList.add(convertStrToInt(list.getNextListEntry()));
        }
    } 
    _useColorWheel = (_colorList.size() == 0) ? true:false;
    
    // select color start condition
    if (_useColorWheel == true){
        // select color from wheel
        _colorIndexMax = 255;
        _currentColorIndex = cfg.reg.startIndex;
        _currentColor = getColorWheel24Bit(_currentColorIndex);    
        _mode = COLOR_WHEEL;
    } else {
        // select color from list
        _colorIndexMax = _colorList.size()-1;
        _currentColorIndex = clamp(0,cfg.reg.startIndex,_colorIndexMax);
        _currentColor = _colorList.get(_currentColorIndex);    
        _mode = COLOR_LIST;
    }

    if (_incStep == 0){
        // static color mode
        // stay in state stop
        _mode |= LOOP_STOP;
        // _state = stop;
    } else {
        // dynamic color
        // wait for trigger or time
        if (_timeStep == 0xFF){
            _mode |= LOOP_TRIGGER;
            _state = waitTrigger;
        } else {
            _mode |= LOOP_TIME;
            _state = initTime;
        }
    }
}

void ColorCtrl::loop(uint32_t now){
    switch(_state){
        case stop:
            break;
        
        case initTime:
            _nextLoopTime = now+_timeStep;
            _state = waitTime;
            break;

        case waitTime:
            if (now >= _nextLoopTime){
                // time for update
                _eventCounter++;
                _nextLoopTime += _timeStep;
                _checkForUpdate();
            }
            break;

        case waitTrigger:
            if (_triggerActive == true){
                _eventCounter++;
                _triggerActive = false;
                _checkForUpdate();
            }
            break;
        
        default:
            LOG("ColorCtrl::loop  invalid state");
            _state = stop;
    }
}

void ColorCtrl::switchToTriggerMode(){
    _state = stop;
    _eventCounter = 0;
    _triggerActive = false;
    _nextLoopTime = 0;
    _timeStep = 0xFF;
    _state = waitTrigger;
}

void ColorCtrl::_checkForUpdate(){
    if (_eventCounter >= _eventTarget){
        _eventCounter = 0;

        // calc new Index
        _currentColorIndex = wrapAround(0,_currentColorIndex+_incStep,_colorIndexMax);

        // now calc color based on index
        if (_useColorWheel == true){
            _currentColor = getColorWheel24Bit(_currentColorIndex);
        } else {
            _currentColor = _colorList.get(_currentColorIndex);
        }
    }
}

///////////////////////////////////////////////////
// FlashCtrl

void FlashCtrl::config(flashCtrl_t cfg){
    _state        = stop;
    _triggerActive= false;
    _groupCount   = cfg.reg.flashPerGroup;
    _groupCounter = 0;
    _t1           = cfg.reg.t1_10ms*10;
    _t2           = cfg.reg.t2_10ms*10;
    if (cfg.reg.t3_100ms == 0xFF){
        _waitForTrigger = true;
        _t3 = 0;
    } else {
        _waitForTrigger = false;
        _t3 = cfg.reg.t3_100ms*100;
    }
    if (_groupCount > 0){
        _state = init;
    }
}

bool FlashCtrl::loop(uint32_t now){
    switch (_state){
        case stop:
            // do nothing parameters are loocked by other thread
            // the setup will move forward to state init
            break;

        case init:
            if (_waitForTrigger == true){
                _state = waitTrigger;
            } else  {
                _state = pause;
                _nextLoopTime = now+_t3;
            }
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
                return true;
            }
            break;

        case flashOff:
            if (now >= _nextLoopTime){
                _state = flashOn;
                _nextLoopTime += _t1;
                return true;
            }
            break;

        case pause:
            if (now >= _nextLoopTime){
                _state = flashOn;
                _groupCounter = 0;
                _nextLoopTime += _t1;
                return true;
            }
            break;

        case waitTrigger:
            if (_triggerActive == true){
                _state = flashOn;
                _groupCounter = 0;
                _nextLoopTime = now + _t1;
                _triggerActive = false;
                return true;
            }
            break;
    }
    return false;
}


///////////////////////////////////////////////////
// BreathCtrl

void BreathCtrl::config(breathCtrl_t cfg){
    _state = stop;
    _dimDelta = 0;
    _target = cfg.reg.delta;
    _t1 = cfg.reg.t1_100ms * 100;
    _t2 = cfg.reg.t2_100ms * 100;

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
            if (elapsed >= _t1){
                _state = down;
                _lastTurn = now;
                elapsed = clampUint32(0,elapsed,_t1);
            }
            value = (_target * elapsed) / _t1;
            _dimDelta = clampUint32(0,value,_target);
            break;

        case down:
            elapsed = now-_lastTurn;
            if (elapsed >= _t2){
                _state = up;
                _lastTurn = now;
                elapsed = clampUint32(0,elapsed,_t2);
            }
            value = (_target * elapsed) / _t2;
            _dimDelta = clampUint32(0,_target - value,_target);
            break;
    }
}

