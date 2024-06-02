#include "NeoStripeCtrl.hpp"
#include "Debug.hpp"
#include "helper.h"


NeoStripeCtrl::NeoStripeCtrl() : Ctrl(),_current(0),_pNeoStripe(NULL),_count(0),_aniInterface("NeoStripeConfigInterface"){

} 

void NeoStripeCtrl::begin(WS2812FX * pNeoStripe){
    LOG(F("NeoStripeCtrl::NeoStripeCtrl setup ws2812fx"));
    ASSERT(pNeoStripe != NULL,F("pNeoStripe must not be NULL"));
    _pNeoStripe = pNeoStripe;
    _pNeoStripe->init();
    _count = _pNeoStripe->getModeCount() - MAX_CUSTOM_MODES + 1;
    _aniNameList = "0:Off,";   // add off as number 0
    for(int i=1; i < _count; i++){
        _aniNameList += String(i);
        _aniNameList += ':';
        _aniNameList += _pNeoStripe->getModeName(i-1);
        if (i < _count-1){
            _aniNameList += ",";
        }
    }
    _pNeoStripe->setMode(0);
    _current = 1;
    _cfgCounter = 0;
    _aniInterface.config(AniCfg(ANI_WR_DIM | ANI_WR_COLOR | 0xFF,0,0,0,"0x0000 00FF"));
    _pNeoStripe->start();

}


const char * NeoStripeCtrl::getName(){
    if (_current == 0){
        return "Off";
    } 
    return (const char *)_pNeoStripe->getModeName(_current-1);
}

void NeoStripeCtrl::loop(uint32_t time){
    if (_pNeoStripe == NULL)            return;
    if (_aniInterface.hasChanged() == true){
        _cfgCounter++;
        _aniInterface.loop(_cfgCounter); // make new values available
        _pNeoStripe->setBrightness(_aniInterface.getDim());
        _pNeoStripe->setColor(_aniInterface.getColor());
        _pNeoStripe->setSpeed(_aniInterface.getSpeed());
    }
    _pNeoStripe->service();
}

int NeoStripeCtrl::select(int nr){
    if (nr >= _count)        return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
    if (nr < 0 )             return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;

    if (nr == 0){
        _pNeoStripe->setMode(FX_MODE_STATIC);
        _aniInterface.config(AniCfg(ANI_WR_DIM | ANI_WR_COLOR | 0,0,0,0,"0"));
        _current = 0;
    } else {
        _pNeoStripe->setMode(nr-1);
        _aniInterface.config(AniCfg(ANI_WR_DIM | ANI_WR_COLOR | 0xFF,0,0,0,"0x0000 00FF"));
    }
    _current = nr;
    return ANI_OK;
}

int NeoStripeCtrl::select(const char *pName){
    // find matching entry or set all to NULL
    if (strcmp(pName , "Off") == 0){
        return select(0);
    }
    for(int i=1;i < _count;i++){
        if (strcmp(pName, (const char *)_pNeoStripe->getModeName(i-1)) == 0){
            return select(i);            
        }
    }
    LOG(F("NeoStripeCtrl::setup could not find mode"));
    return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
}


int NeoStripeCtrl::config(AniCfg cfg){
    // functionality of base class Ani is not used 
    // we just use the parameter decoding, to harmonize the interface of the different objects a little bit
    // for example dim up / down for all modules now work with consistent parameter set
    _aniInterface.config(cfg);
    return ANI_OK;
}

