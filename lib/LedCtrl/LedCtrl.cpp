#include "LedCtrl.hpp"
#include "../Debug/Debug.hpp"

LedCtrl::LedCtrl(int pin) : Ctrl(){
    // create LED object
    _pLed = new Led(pin);

    // fill ani List
    addAni(new LedOffAni());
    addAni(new LedOnAni());
    addAni(new LedDimAni());
    addAni(new LedBlinkAni());

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
 
    takeSemaBlocking();
    ((LedAni*)_pCurrentAni)->loop(_pLed);
    freeSema();
}