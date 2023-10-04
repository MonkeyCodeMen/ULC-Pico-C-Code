#include "LedCtrl.hpp"
#include "Debug.hpp"

LedCtrl::LedCtrl(int pin) : Ctrl(){
    // create LED object
    _pLed = new Led(pin);

    // fill ani List
    _addAni(new LedOffAni());
    _addAni(new LedOnAni());
    _addAni(new LedDimAni());
    _addAni(new LedBlinkAni());

    // setup first
    setup(0);
}

LedCtrl::~LedCtrl(){
    if (_pLed != NULL)   
        delete(_pLed);
 }


void LedCtrl::loop(){
    ASSERT(_pCurrentAni != NULL,"");
    ASSERT(_pLed != NULL,"");

    if (_mutexSetup.isLocked()==true){
        return;  // do not wait 
    }

    _mutexSetup.lock();
    ((LedAni*)_pCurrentAni)->loop(_pLed);
    _mutexSetup.unlock();
}