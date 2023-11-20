#include "LedCtrl.hpp"
#include "Debug.hpp"

LedCtrl::LedCtrl(Led * pLed) : Ctrl(){
    // create LED object
    ASSERT(pLed != NULL,"pLed must not be NULL");
    _pLed = pLed;

    // fill ani List
    addAni(new LedOffAni());
    addAni(new LedOnAni());
    addAni(new LedDimAni());
    addAni(new LedBreathAni());
    addAni(new LedBlinkAni());
    addAni(new LedMultiFlashAni());

    // setup first
    setup(0);
}

LedCtrl::~LedCtrl(){

}


void LedCtrl::loop(u32_t time){
    ASSERT(_pCurrentAni != NULL,"");
    ASSERT(_pLed != NULL,"");

    if (_mutexSetup.isLocked()==true){
        return;  // do not wait 
    }

    _mutexSetup.lock();
    ((LedAni*)_pCurrentAni)->loop(time,_pLed);
    _mutexSetup.unlock();
}