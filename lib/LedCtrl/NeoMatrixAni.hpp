
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
#include <Adafruit_NeoMatrix.h>
#include "helper.h"
#include "Debug.hpp"
#include <AnimatedGIF.h>
#include <SDcard.hpp>
#include "PinMapping.h"


#define matrix_default_brightness   0xA0

class NeoMatrixAni:public Ani
{
	public:
		NeoMatrixAni(const char * pName) : Ani(pName) {}
		~NeoMatrixAni() = default;
		virtual void loop(uint32_t time,Adafruit_NeoMatrix * pMatrix)   {
            Ani::loop(time);
            if (hasChanged() == true){
                pMatrix->setBrightness(_dim);
                pMatrix->fillScreen(toColor565(_color));
                pMatrix->show();
            }
        }
};

class MatrixOffAni : public NeoMatrixAni{
    public:
        MatrixOffAni():NeoMatrixAni((const char *) F("off"))    { reset();                                       }
        void reset()                                            { config(AniCfg(ANI_WR_ALL | 0x00,0,0,0,"0"));   }
};

class MatrixOnAni : public NeoMatrixAni{
    public:
        MatrixOnAni():NeoMatrixAni((const char *) F("on"))      { reset();                                       }
        void reset()                                            { config(AniCfg(ANI_WR_ALL | 0x40,0,0,0,"0x00FF FFFF"));      }
};


class MatrixDimAni : public NeoMatrixAni{
    public:
        MatrixDimAni():NeoMatrixAni((const char *) F("dim"))    { reset();                                       }
        void reset()                                            { config(AniCfg(ANI_WR_ALL | 0x40,0,0,0,"0x0000 FF00"));       }
};


class MatrixBreathAni : public NeoMatrixAni{
    public:
        MatrixBreathAni():NeoMatrixAni((const char *) F("breath"))  { reset();                                       }
        void reset()                                                { config(AniCfg(ANI_WR_ALL | 0x10,0,0,0x00102050,"0x00FF 0000"));  }
};

class MatrixBlinkAni : public NeoMatrixAni{
    public:
        MatrixBlinkAni():NeoMatrixAni((const char *) F("blink"))    { reset();                                       }
        void reset()                                                { config(AniCfg(ANI_WR_ALL | 0x40,0x00FF0100,0x05001901,0,"0x00FF 0000,0x0000 00FF"));    }
};

class MatrixMultiFlashAni : public NeoMatrixAni{
    public:
        MatrixMultiFlashAni():NeoMatrixAni((const char *) F("multi flash")) { reset();   }
        void reset()                                                        {  config(AniCfg(ANI_WR_ALL | 0x80,0x00FF0100,0x1E0A0305,0,"0,0x00FF0000,0, 0x0000FF00,0, 0x000000FF,0, 0x00FFFFFF,0, 0x00800080"));   }
};

class MatrixRainbowAni : public NeoMatrixAni{
    public:
        MatrixRainbowAni():NeoMatrixAni((const char *) F("rainbow"))        { reset();         }
        void reset()                                                        { config(AniCfg(ANI_WR_ALL | 0x40,0x000A0100,0,0,""));     }
};


