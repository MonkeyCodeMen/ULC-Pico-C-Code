#pragma once
#include <Arduino.h>
#include "Led.hpp"
#include "Ani.hpp"
#include "helper.h"

class LedAni : public Ani
{
    
    public:
        LedAni(const char * pName) : Ani(pName) {}
        ~LedAni() = default;
        virtual void loop(uint32_t time,Led * pLed)  { 
            Ani::loop(time);
            if (_color > 0){
                pLed->set(_dim);
            } else {
                pLed->set(LED_OFF);
            }
        }
};

class LedOffAni : public LedAni{
    public:
        LedOffAni()  : LedAni((const char *) F("off"))          {                           }
        void reset()                                            {                           }
        void loop(uint32_t time,Led * pLed)                     { pLed->set(LED_OFF);       }
};


class LedOnAni : public LedAni{
    public:
        LedOnAni()  : LedAni((const char *) F("on"))            { reset();                                 }
        void reset()                                            { config(AniCfg(0xF00000FF,0,0,0,"1,0"));  }
};


class LedDimAni : public LedAni{
    public:
        LedDimAni()  : LedAni((const char *) F("dim"))          { reset();                                 }
        void reset()                                            { config(AniCfg(0xF0000080,0,0,0,"1,0"));  }
};

class LedBlinkAni : public LedAni{
/*
	p3 : flash config:  xx xx xx xx   : def   : 
						|| || || ++---:  0x01 : 1 flash per group 
						|| || ++------:  0x19 : time t1 in 10ms:  250ms  flashtime        .. time of one flash
						|| ++---------:  0x00 : time t2 in 10ms:  0ms    inter flash time (only one per group)
						++------------:  0x05 : time t3 in 100ms: 500ms  inter group time .. time between two flash groups

	p2 : color Index:   xx xx xx xx   :
						|| || || ++---: def   : start index of color list 
						|| || ||      : 0     
						|| || ++------: 1     : each trigger next color
						++-++---------: 0xFFFF FFFF : wait for trigger (from flashCtrl)
						p2:0xFFFF0100 & str:"1,0"   switch between red and blue on trigger event  start with red

    dim value: 0x80
*/
    public:
        LedBlinkAni() :LedAni((const char *) F("blink"))    {  reset();                                                     }
        void reset()                                        {  config(AniCfg(0xF0000080,0xFFFF0100,0x05001901,0,"1,0"));    }
};

class LedMultiFlashAni : public LedAni{
/*
	p3 : flash config:  xx xx xx xx   : def   : 
						|| || || ++---:  0x05 : 5 flash per group 
						|| || ++------:  0x03 : time t1 in 10ms:  30ms  flashtime        .. time of one flash
						|| ++---------:  0x0A : time t2 in 10ms:  100ms inter flash time .. time between flashes
						++------------:  0x32 : time t3 in 100ms: 5000ms inter group time .. time between groups

	p2 : color Index:   xx xx xx xx   :
						|| || || ++---: def   : start index of color list 
						|| || ||      : 0     
						|| || ++------: 1     : each trigger next color
						++-++---------: 0xFFFF FFFF : wait for trigger (from flashCtrl)
						p2:0xFFFF0100 & str:"1,0"   switch between red and blue on trigger event  start with red

*/
    public:
        LedMultiFlashAni() :LedAni((const char *) F("multi flash"))     { reset();                                                     }
        void reset()                                                    { config(AniCfg(0xF00000FF,0xFFFF0100,0x320A0305,0,"1,0"));    }
};

class LedBreathAni : public LedAni{
    /*  
        base dim level : 0x10
    	p4 : breath config: xx xx xx xx   : def    :
                            || || || ++---: 0xD0   : delta dim increase (dim between base : 0x10 and 0x10+0xD0)
                            || |+-++------: 0x020  : time t5 in 100ms: 3200ms time for dim up
                            ++-++---------: 0x010  : time t6 in 100ms: 1600ms time for dim down
    */
    public:
        LedBreathAni():LedAni((const char *) F("breath"))               { reset();                                                   }
        void reset()                                                    { config(AniCfg(0xF0000010,0,0,0x010020D0,"0x00FF FFFF"));   }
};

