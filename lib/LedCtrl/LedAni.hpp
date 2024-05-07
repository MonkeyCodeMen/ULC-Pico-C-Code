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
            // _dimCtrl.loop(time);  does nothing anyway
			// _colorCtrl.loop(time); we have no colors on single die leds
            _flashCtrl.loop(time);
			_breathCtrl.loop(time);

            if (_flashCtrl.selectColor(1,0) == 1){
                uint8_t dim = _dimCtrl.getDim();
                dim = _breathCtrl.modifyDimFactor(dim);
                pLed->set(dim);
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
        LedOnAni()  : LedAni((const char *) F("on"))            { reset();                  }
        void reset()                                            { config(AniCfg(0xF000000FF,0,0,0,"0x00FF FFFF"));       }
};



/*

	basic layout for configuration:

	p1 : dim value:		xx xx xx xx   :
						|| || || ++---: set dim value 0-255 (0 = off)
						|| || ++------: == 0   : no inc / dec 
						|| ||         : <  128 : inc step
						|| ||         : >= 128 : dec step
						|+-++---------: NeoStripe speed parameter
						+-------------: update CTRL bit field
						              : 7654
									  : |||+--- 1: update breath config
									  : ||+---- 1: update flash config
									  : |+----- 1: update color config
									  : +------ 1: update dim config
									
									  0:  no update 
									  8:  update dim
									  9:  update dim & breath
									  A:  update dim & flash
									  B:  update dim & flash & breath
									  .....

						p1: 0x8000 0030   set to dim level 0x30
						p1: 0x8000 1000 inc by 10 ==> results in 0x30 + 0x10 = 0x40 dim level 

	p2 : color Index:   xx xx xx xx   :
						|| || || ++---: start index of color list 
						|| || ||      :     color list is provided in cfg.str
						|| || ||      :     if cfg.str is empty color are taken from color wheel (0..255)
						|| || ++------: == 0   : no inc / dec 
						|| ||         : <  128 : inc step
						|| ||         : >= 128 : dec step  (-1 = 255; -2 = 254)
						|| ++---------: time t2 in ms: time between two color steps 
						||            : 0xxxFF xxxx wait for trigger 
						++------------: reserved 

						standard color white, an be overwritten by color list or color wheel
						p2:0  & str:""             ==> constant white
						p2:0  & str:"0x0000 00FF"  ==> constant blue
					

	p3 : flash config:  xx xx xx xx   :
						|| || || ++---: flashes per group (t1 = flash on time)
						|| || ||      : 0:  flash modul switched off
						|| || ++------: time t1 in ms: 		flashtime        .. time of one flash
						|| ++---------: time t2 in ms: 		inter flash time .. time between two flashes
						++------------: time t3 in 100ms: 	inter group time .. time between two flash groups
									  :                     0xFFxx xxxx wait for trigger 
									  .. standard  colors : flash white   , pause black
									     can be overwritten by str color list   first color flash (if provided)
										 										2nd color   pause (if provided)

						0xXXXX XX00 flash module switched off


	p4 : breath config: xx xx xx xx   :
						|| || || ++---: delta dim increase (clipped at 255) for high level (p1 = low level) 
						|| || ||      : 0: switch breath module off
						|| |+-++------: time t5 in ms: time for dim up
						++-++---------: time t6 in ms: time for dim down

	str: string parameter : examples:
					color list: "0x00FF 0000,0x0000 FF00,0x0000 00FF"
					file list:  "3*rainbow.gif,2*alien.gif"    .. play 3 times rainbow.gif, 2 times alien.gif


	
		
*/
