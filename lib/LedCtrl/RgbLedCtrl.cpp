#include "RgbLedCtrl.hpp"
#include "Debug.hpp"

RgbLedCtrl::RgbLedCtrl(RgbLed * pRgbLed) : Ctrl()
{
    // create LED object
    ASSERT(pRgbLed != NULL,"pRgbLed object could not be NULL");
    _pRgbLed = pRgbLed;

    _addAni(new RgbLedOffAni());
    _addAni(new RgbLedOnAni());
    _addAni(new RgbLedBlinkAni());
    _addAni(new RgbLedBreathAni());
    _addAni(new RgbLedRainbowAni());
    _addAni(new RgbLedMultiFlashAni());

    
    // select first
    setup(0);
}


RgbLedCtrl::~RgbLedCtrl()
{
    if (_pRgbLed != NULL)   delete(_pRgbLed);
}


void RgbLedCtrl::loop(u32_t time){
    ASSERT(_pCurrentAni != NULL,"");
    ASSERT(_pRgbLed != NULL,"");
 
    if (_mutexSetup.isLocked()==true){
        return;  // do not wait 
    }
    _mutexSetup.lock();
    ((RgbLedAni*)_pCurrentAni)->loop(time,_pRgbLed);
    _mutexSetup.unlock();
}