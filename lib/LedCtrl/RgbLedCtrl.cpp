#include "RgbLedCtrl.hpp"
#include "../Debug/Debug.hpp"

RgbLedCtrl::RgbLedCtrl(int pinR,int pinG,int pinB) : Ctrl()
{
    // create LED object
    _pRgbLed = new RgbLed(pinR,pinG,pinB);

    addAni(new RgbLedOffAni());
    addAni(new RgbLedOnAni());
    addAni(new RgbLedBlinkAni());
    addAni(new RgbLedBreathAni());
    
    // setup first
    setup(0);
}


RgbLedCtrl::~RgbLedCtrl()
{
    if (_pRgbLed != NULL)   delete(_pRgbLed);
}


void RgbLedCtrl::loop(){
    ASSERT(_pCurrentAni != NULL,"");
    ASSERT(_pRgbLed != NULL,"");
 
    takeSemaBlocking();
    ((RgbLedAni*)_pCurrentAni)->loop(_pRgbLed);
    freeSema();
}