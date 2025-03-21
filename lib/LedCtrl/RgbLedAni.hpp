
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
        void reset()                                                { config(AniCfg(ANI_WR_ALL | 0,0,0,0,"0"));         }
};

class RgbLedOnAni : public RgbLedAni{
    /* default Blue on at 100% brightness */
    public:
        RgbLedOnAni()  : RgbLedAni((const char *) F("on"))          { reset();                                          }
        void reset()                                                { config(AniCfg(ANI_WR_ALL | 0xFF,0,0,0,"0x0000 00FF"));   }
};


class RgbLedDimAni : public RgbLedAni{
    /* default green at 50% brightness */
    public:
        RgbLedDimAni()  : RgbLedAni((const char *) F("dim"))        { reset();                                          }
        void reset()                                                { config(AniCfg(ANI_WR_ALL | 0x080,0,0,0,"0x0000 FF00"));   }
};

class RgbLedBreathAni : public RgbLedAni{
    public:
        RgbLedBreathAni()  : RgbLedAni((const char *) F("breath"))  { reset();                                                   }
        void reset()                                                { config(AniCfg(ANI_WR_ALL | 0x10,0,0,0x001020D0,"0x00FF 0000"));   }
};


class RgbLedBlinkAni : public RgbLedAni{
    public:
        RgbLedBlinkAni()  : RgbLedAni((const char *) F("blink"))    { reset();                                                                          }
        void reset()                                                { config(AniCfg(ANI_WR_ALL | 0x80,0x00FF0100,0x05001901,0,"0x00FF 0000,0x0000 00FF"));     }
};


class RgbLedMultiFlashAni : public RgbLedAni{
    public:
        RgbLedMultiFlashAni()  :RgbLedAni((const char *) F("multi flash"))  { reset();                                                                                                                     }
        void reset()                                                        { config(AniCfg(ANI_WR_ALL | 0xFF,0x00FF0100,0x1E0A0305,0,"0,0x00FF0000,0, 0x0000FF00,0, 0x000000FF,0, 0x00FFFFFF,0, 0x00800080"));  }
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
        void reset()                                                    { config(AniCfg(ANI_WR_ALL | 0xFF,0x000A0100,0,0,""));     }
};

