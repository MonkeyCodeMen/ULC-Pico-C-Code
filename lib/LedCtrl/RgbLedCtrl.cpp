#include "RgbLedCtrl.hpp"
#include "Debug.hpp"

RgbLedCtrl::RgbLedCtrl(int pinR,int pinG,int pinB) : Ctrl()
{
    // create LED object
    _pRgbLed = new RgbLed(pinR,pinG,pinB);

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


void RgbLedCtrl::loop(){
    ASSERT(_pCurrentAni != NULL,"");
    ASSERT(_pRgbLed != NULL,"");
 
    if (_mutexSetup.isLocked()==true){
        return;  // do not wait 
    }
    _mutexSetup.lock();
    ((RgbLedAni*)_pCurrentAni)->loop(_pRgbLed);
    _mutexSetup.unlock();
}