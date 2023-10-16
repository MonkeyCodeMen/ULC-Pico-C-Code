#include "LedCtrl.hpp"
#include "Debug.hpp"

LedCtrl::LedCtrl(Led * pLed) : Ctrl(){
    // create LED object
    ASSERT(pLed != NULL,"pLed must not be NULL");
    _pLed = pLed;

    // fill ani List
    _addAni(new LedOffAni());
    _addAni(new LedOnAni());
    _addAni(new LedDimAni());
    _addAni(new LedBlinkAni());
    _addAni(new LedMultiFlashAni());

    // setup first
    setup(0);
}

LedCtrl::~LedCtrl(){
    if (_pLed != NULL)   
        delete(_pLed);
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