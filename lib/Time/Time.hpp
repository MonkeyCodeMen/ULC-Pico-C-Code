#pragma once

#include <Arduino.h>
#include <Wire.h>

#include <Mutex.hpp>
#include <Debug.hpp>

//#include <RtcDS1307.h>
//#include <RTClib.h>



class Time{
    public:
        Time():_pBus(NULL),_pMutex(NULL) {}
        ~Time() { if (_dummyMutex == true) delete _pMutex; }
        
        void begin(uint32_t now,TwoWire *pBus,Mutex * pMutex=NULL){
                if (pBus == NULL){
                    LOG(F("invalid bus"));
                    _pBus = NULL;
                }
                _pBus = pBus;
                if (pMutex == NULL){
                    _pMutex = new Mutex();
                    _dummyMutex = true;
                } else {
                    _pMutex = pMutex;
                }
        }


        void loop(uint32_t now);

    protected:
        TwoWire *           _pBus;
        uint32_t            _startTime;
        Mutex  *            _pMutex;
        bool                _dummyMutex;

        RtcDS1307<TwoWire>  *_pRtc;

};