
/*
 * MIT License
 * 
 * Copyright (c) 2024 MonkeyCodeMen@GitHub
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * provided to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#pragma once
#include <Arduino.h>
#include "Led.hpp"
#include "Ani.hpp"
#include "helper.h"

class LedAni : public Ani{
    public:
        LedAni(const char * pName) : Ani(pName) {}
        ~LedAni() = default;
        virtual void loop(uint32_t time,Led * pLed)  { 
            Ani::loop(time);
            if(hasChanged() == true){
                if ((_dim > 0) && (_color > 0)) {
                    pLed->set(_dim);
                } else {
                    pLed->set(LED_OFF);
                }
            }
        }
};

class LedOffAni : public LedAni{
    public:
        LedOffAni()  : LedAni((const char *) F("off"))          {                                               }
        void reset()                                            { config(AniCfg(ANI_WR_ALL | 0x00,0,0,0,"0"));   }
};


class LedOnAni : public LedAni{
    public:
        LedOnAni()  : LedAni((const char *) F("on"))            { reset();                                      }
        void reset()                                            { config(AniCfg(ANI_WR_ALL | 0xFF,0,0,0,"1"));   }
};


class LedDimAni : public LedAni{
    public:
        LedDimAni()  : LedAni((const char *) F("dim"))          { reset();                                      }
        void reset()                                            { config(AniCfg(ANI_WR_ALL | 0x80,0,0,0,"1"));   }
};

class LedBlinkAni : public LedAni{
/*
   	colorCfg :          xx xx xx xx   :
						|| || || ++---: 0 start index of color list   
						|| || ||      :     color list is provided in cfg.str
						|| || ||      :     if cfg.str is empty color are taken from color wheel (0..255)
						|| || ++------: 1  
						|| ||         : <  128 : inc step
						|| ||         : >= 128 : dec step  (-1 = 255; -2 = 254)
						|| ++---------: time t2 in ms: time between two color steps 
						||            : 0xxxFF xxxx wait for trigger 
						++------------: event divider 0..255 = 1..256 (2 ==> 3 trigger or 3 time event until color change)			

						standard color white, an be overwritten by color list or color wheel
						colorCfg:0  & str:"0x00FF FFFF"  ==> constant white
						colorCfg:0  & str:"0x0000 00FF"  ==> constant blue
						colorCfg:0xFFFF0100 & str:"0x00FF 0000,0x0000 00FF"   switch between red and blue on trigger event  start with red

    dim value: 0x80
*/
    public:
        LedBlinkAni() :LedAni((const char *) F("blink"))    {  reset();                                                     }
        void reset()                                        {  config(AniCfg(ANI_WR_ALL | 0x80,0x00FF0100,0x05001901,0,"0,1"));    }
};

class LedMultiFlashAni : public LedAni{
/*
	colorCfg :          xx xx xx xx   :
						|| || || ++---: start index of color list   (0..255)
						|| || ||      :     color list is provided in cfg.str
						|| || ||      :     if cfg.str is empty color are taken from color wheel (0..255)
						|| || ++------: == 0   : no inc / dec  = static
						|| ||         : <  128 : inc step
						|| ||         : >= 128 : dec step  (-1 = 255; -2 = 254)
						|| ++---------: time t2 in ms: time between two color steps 
						||            : 0xxxFF xxxx wait for trigger 
						++------------: event divider 0..255 = 1..256 (2 ==> 3 trigger or 3 time event until color change)			

						standard color white, an be overwritten by color list or color wheel
						colorCfg:0  & str:"0x00FF FFFF"  ==> constant white
						colorCfg:0  & str:"0x0000 00FF"  ==> constant blue
						colorCfg:0xFFFF0100 & str:"0x00FF 0000,0x0000 00FF"   switch between red and blue on trigger event  start with red
					

	flashCfg :          xx xx xx xx   :
						|| || || ++---: 5 flashes per group 
						|| || ++------: time t1 in 10ms:  30ms  flashtime        .. time of one flash
						|| ++---------: time t2 in 10ms:  160ms inter flash time .. time between two flashes
						++------------: time t3 in 100ms: 3000ms	inter group time .. time between two flash groups

						0xXXXX XX00 flash module switched off

*/
    public:
        LedMultiFlashAni() :LedAni((const char *) F("multi flash"))     { reset();                                                     }
        void reset()                                                    { config(AniCfg(ANI_WR_ALL | 0xFF,0x00FF0100,0x1E0A0305,0,"0,1"));    }
};

class LedBreathAni : public LedAni{
    /*  


    breathCfg :         xx xx xx xx   :
						|| || || ++---: 0xD0  :  delta dim increase (clipped at 255) for high level (dimCfg = low level) 
						|| || ||      : 0: switch breath module off
						|| || ++------: 0x20 : time t1 in 100ms: 3200ms time for dim up
						|| ++---------: 0x10 : time t2 in 100ms: 1600ms time for dim up
						++------------: reserved

    */
    public:
        LedBreathAni():LedAni((const char *) F("breath"))               { reset();                                                   }
        void reset()                                                    { config(AniCfg(ANI_WR_ALL | 0x10,0,0,0x001020D0,"1"));   }
};

