#pragma once
#include "Ani.hpp"
#include "RgbLed.hpp"
#include "helper.h"

class RgbLedAni:public Ani
{
	public:
		RgbLedAni(const char * pName) : Ani(pName) {}
		~RgbLedAni() = default;
		virtual void loop(uint32_t time,RgbLed * pLed) {
            Ani::loop(time);
            pLed->set(_color,_dim);       
        }
};

class RgbLedOffAni : public RgbLedAni{
    /* all LED's off , reagardless of config :-) */
    public:
        RgbLedOffAni():RgbLedAni((const char *) F("off"))           { reset();                                          }
        void reset()                                                { config(AniCfg(0xF0000000,0,0,0,""));              }
        void loop(uint32_t time,RgbLed * pLed)                      { pLed->set(RGB_LED_OFF,RGB_LED_OFF,RGB_LED_OFF);   }
};

class RgbLedOnAni : public RgbLedAni{
    /* default Blue on at 100% brightness */
    public:
        RgbLedOnAni()  : RgbLedAni((const char *) F("on"))          { reset();                                          }
        void reset()                                                { config(AniCfg(0xF00000FF,0,0,0,"0x0000 00FF"));   }
};


class RgbLedDimAni : public RgbLedAni{
    /* default green at 50% brightness */
    public:
        RgbLedDimAni()  : RgbLedAni((const char *) F("dim"))        { reset();                                          }
        void reset()                                                { config(AniCfg(0xF0000080,0,0,0,"0x0000 FF00"));   }
};

class RgbLedBreathAni : public RgbLedAni{
    /*  
        base dim level : 0x10  
        color:           blue
    	p4 : breath config: xx xx xx xx   : def    :
                            || || || ++---: 0xD0   : delta dim increase (dim between base : 0x10 and 0x10+0xD0)
                            || |+-++------: 0x020  : time t5 in 100ms: 3200ms time for dim up
                            ++-++---------: 0x010  : time t6 in 100ms: 1600ms time for dim down
    */
    public:
        RgbLedBreathAni()  : RgbLedAni((const char *) F("breath"))  { reset();                                                   }
        void reset()                                                { config(AniCfg(0xF0000010,0,0,0x010020D0,"0x0000 00FF"));   }
};


class RgbLedBlinkAni : public RgbLedAni{
/*
	p3 : flash config:  xx xx xx xx   : def   : 
						|| || || ++---:  0x01 : 1 flash per group 
						|| || ++------:  0x19 : time t1 in 10ms:  250ms  flashtime        .. time of one flash
						|| ++---------:  0x00 : time t2 in 10ms:  0ms    inter flash time (only one per group)
						++------------:  0x05 : time t3 in 100ms: 500ms  inter group time .. time between two flash groups
	p2 : color Index:   xx xx xx xx   : def   :
						|| || || ++---: 0     : start index of color list 
						|| || ++------: 1     : each trigger next color
						++-++---------: 0xFFFF FFFF : wait for trigger (from flashCtrl)
						p2:0xFFFF0100 & str:"red,blue"   switch between red and blue on trigger event  start with red (= blue is off color for blink)

    brightness:100%
    blink betweeen red and blue
*/
    public:
        RgbLedBlinkAni()  : RgbLedAni((const char *) F("blink"))    { reset();                                                                          }
        void reset()                                                { config(AniCfg(0xF00000FF,0xFFFF0100,0x05001901,0,"0x00FF 0000,0x0000 00FF"));     }
};


class RgbLedMultiFlashAni : public RgbLedAni{
/*
	p3 : flash config:  xx xx xx xx   : def   : 
						|| || || ++---:  0x05 : 5 flash per group 
						|| || ++------:  0x03 : time t1 in 10ms:  30ms  flashtime        .. time of one flash
						|| ++---------:  0x0A : time t2 in 10ms:  100ms inter flash time .. time between flashes
						++------------:  0x32 : time t3 in 100ms: 5000ms inter group time .. time between groups
    5 flash need 10 color in list  Flash1-on-color, dark-color 
*/
    public:
        RgbLedMultiFlashAni()  :RgbLedAni((const char *) F("multi flash"))  { reset();                                                                                                                     }
        void reset()                                                        { config(AniCfg(0xF00000FF,0xFFFF0100,0x05001901,0,"0x00FF0000,0, 0x0000FF00,0, 0x000000FF,0, 0x00FFFFFF,0, 0x00800080,0 "));  }
};


class RgbLedRainbowAni : public RgbLedAni{
/*
	p2 : color Index:   xx xx xx xx   : def   :
						|| || || ++---: 0     : start index of color list 
						|| || ++------: 1     : each trigger next color
						++-++---------: 0A    : time between two color steps 10ms
    brightness:100%
*/
    public:
        RgbLedRainbowAni()  : RgbLedAni((const char *) F("rainbow"))    { reset();                                          }
        void reset()                                                    { config(AniCfg(0xF00000FF,0x000A0100,0,0,""));     }
};

