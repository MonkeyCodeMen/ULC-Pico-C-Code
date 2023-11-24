#include "NeoStripeCtrl.hpp"
#include "Debug.hpp"
#include "helper.hpp"


NeoStripeCtrl::NeoStripeCtrl(WS2812FX * pNeoStripe) : Ctrl()
{
    LOG(F_CONST("NeoStripeCtrl::NeoStripeCtrl setup ws2812fx"));
    ASSERT(pNeoStripe != NULL,"pNeoStripe must not be NULL");
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
    _pNeoStripe->init();
    _pNeoStripe->setMode(0);
    _current = 1;
    setStdParameter();
    _pNeoStripe->start();
}


void NeoStripeCtrl::setStdParameter(){
    _pNeoStripe->setBrightness(0xFF);
    _pNeoStripe->setSpeed(1000);
    _pNeoStripe->setColor(0x0000FF);
}

void NeoStripeCtrl::setOff(){
    _pNeoStripe->setMode(FX_MODE_STATIC);
    _pNeoStripe->setColor(0);
    _pNeoStripe->setBrightness(0);
    _pNeoStripe->setSpeed(1000);
    _current = 0;
}

const char * NeoStripeCtrl::getName(){
    if (_current == 0){
        return F_CONST("Off");
    } 
    return (const char *)_pNeoStripe->getModeName(_current-1);
}

void NeoStripeCtrl::loop(u32_t time){
    if (_pNeoStripe == NULL)            return;
    if (_mutexSetup.isLocked()==true)   return;  // do not wait 

    _mutexSetup.lock();
    _pNeoStripe->service();;
    _mutexSetup.unlock();
}

int NeoStripeCtrl::setup(int nr){
    if (nr >= _count)        return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
    if (nr < 0 )             return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;

    _mutexSetup.lock();
    if (nr == 0){
        setOff();
    } else {
        _pNeoStripe->setMode(nr-1);
        setStdParameter();
    }
    _mutexSetup.unlock();
    return ANI_OK;
}

int NeoStripeCtrl::setup(const char *pName){
    _mutexSetup.lock();
    // find matching entry or set all to NULL
    if (strcmp(pName , "Off") == 0){
        setOff();
        _current = 0;
        _mutexSetup.unlock();
        return ANI_OK;
    }
    for(int i=1;i < _count;i++){
        if (strcmp(pName, (const char *)_pNeoStripe->getModeName(i-1)) == 0){
            _pNeoStripe->setMode(i-1);
            setStdParameter();
            _current = i;
            _mutexSetup.unlock();
            return ANI_OK;            
        }
    }
    LOG(F_CONST("NeoStripeCtrl::setup could not find mode"));
    _mutexSetup.unlock();
    return ANI_ERROR_PROGRAM_DOES_NOT_EXIST;
}



int NeoStripeCtrl::setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,String str,u32_t length,u8_t ** pData){
    _mutexSetup.lock();
    _pNeoStripe->setColor(p1);
    _pNeoStripe->setBrightness(p2 & 0xFF);
    _pNeoStripe->setSpeed(p3);
    _mutexSetup.unlock();
    return ANI_OK;
}
