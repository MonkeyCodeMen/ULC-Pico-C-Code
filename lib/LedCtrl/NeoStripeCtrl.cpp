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
    setStdParameter();
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
    if (_mutexSetup.isLocked()==true)   return;  // do not wait 

    _mutexSetup.lock();
    _pNeoStripe->service();;
    _mutexSetup.free();
}

int NeoStripeCtrl::select(int nr){
    if (nr >= _count)        return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
    if (nr < 0 )             return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;

    _mutexSetup.lock();
    if (nr == 0){
        setOff();
    } else {
        _pNeoStripe->setMode(nr-1);
        setStdParameter();
    }
    _current = nr;
    _mutexSetup.free();
    return ANI_OK;
}

int NeoStripeCtrl::select(const char *pName){
    _mutexSetup.lock();
    // find matching entry or set all to NULL
    if (strcmp(pName , "Off") == 0){
        setOff();
        _current = 0;
        _mutexSetup.free();
        return ANI_OK;
    }
    for(int i=1;i < _count;i++){
        if (strcmp(pName, (const char *)_pNeoStripe->getModeName(i-1)) == 0){
            _pNeoStripe->setMode(i-1);
            setStdParameter();
            _current = i;
            _mutexSetup.free();
            return ANI_OK;            
        }
    }
    LOG(F("NeoStripeCtrl::setup could not find mode"));
    _mutexSetup.free();
    return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
}


int NeoStripeCtrl::config(AniCfg cfg){
    _mutexSetup.lock();
        // functionality of base class Ani is not used 
        // we just use the parameter decoding, to harmonize the interface of the different objects a little bit
        // for example dim up / down for all modules now work with consistent parameter set
        _aniInterface.config(cfg);
        _pNeoStripe->setBrightness(_aniInterface.getDim());
        _pNeoStripe->setColor(_aniInterface.getColor());
        _pNeoStripe->setSpeed(_aniInterface.getSpeed());
    _mutexSetup.free();
    return ANI_OK;
}


void NeoStripeCtrl::setStdParameter(){
    config(AniCfg(0x800000FF,0,0,0,"0x0000 00FF"));
}

void NeoStripeCtrl::setOff(){
    _pNeoStripe->setMode(FX_MODE_STATIC);
    config(AniCfg(0));
    _current = 0;
}
