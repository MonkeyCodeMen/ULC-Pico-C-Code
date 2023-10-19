#include "NeoStripeCtrl.hpp"
#include "Debug.hpp"
#include "helper.hpp"


NeoStripeCtrl::NeoStripeCtrl(WS2812FX * pNeoStripe) : Ctrl()
{
    LOG(F("NeoStripeCtrl::NeoStripeCtrl setup ws2812fx"));
    ASSERT(pNeoStripe != NULL,"pNeoStripe must not be NULL");
    _pNeoStripe = pNeoStripe;
    _pNeoStripe->init();
    _count = _pNeoStripe->getModeCount() - MAX_CUSTOM_MODES + 1;
    _aniNameList = "0:off,";   // add off as number 0
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
}

void NeoStripeCtrl::setStdParameter(){
    _pNeoStripe->setColor(BLUE);
    _pNeoStripe->setBrightness(255);
    _pNeoStripe->setSpeed(1000);
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
        return F_CONST("off");
    } 
    return (const char *)_pNeoStripe->getModeName(_current-1);
}

void NeoStripeCtrl::loop(u32_t time){
    if (_mutexSetup.isLocked()==true){
        return;  // do not wait 
    }
    _mutexSetup.lock();
    ASSERT(_pNeoStripe != NULL,"");
    _pNeoStripe->service();;
    _mutexSetup.unlock();
}

void NeoStripeCtrl::setup(int nr){
    if (nr >= _count)        return;
    if (nr < 0 )             return;

    _mutexSetup.lock();
    if (nr == 0){
        setOff();
    } else {
        _pNeoStripe->setMode(nr-1);
        setStdParameter();
    }
    _mutexSetup.unlock();
}

void NeoStripeCtrl::setup(const char *pName){
    _mutexSetup.lock();
    // find matching entry or set all to NULL
    if (strcmp(pName , "off") == 0){
        setOff();
        _current = 0;
        _mutexSetup.unlock();
    }
    for(int i=1;i < _count;i++){
        if (strcmp(pName, (const char *)_pNeoStripe->getModeName(i-1)) == 0){
            _pNeoStripe->setMode(i-1);
            setStdParameter();
            _current = i;
            _mutexSetup.unlock();
            return;            
        }
    }
    _mutexSetup.unlock();
}



void NeoStripeCtrl::setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData){
    ASSERT(_pCurrentAni != NULL,"");
    if (_pCurrentAni == NULL)   return;

    _mutexSetup.lock();
    _pNeoStripe->setColor(p1);
    _pNeoStripe->setBrightness(p2 & 0xFF);
    _pNeoStripe->setSpeed(p3);
    _mutexSetup.unlock();
}
