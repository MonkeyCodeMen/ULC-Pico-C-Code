#pragma once
#include "Ani.hpp"
#include <Adafruit_NeoMatrix.h>
#include "helper.h"
#include "Debug.hpp"
#include <ColorSelector.hpp>
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
            if (needsUpdate() == true){
                pMatrix->setBrightness(_dim);
                pMatrix->fillScreen(toColor565(_color));
                pMatrix->show();
            }
        }
};

class MatrixOffAni : public NeoMatrixAni{
    public:
        MatrixOffAni():NeoMatrixAni((const char *) F("off"))    { }
        void reset() { config(AniCfg(0xF0000000,0,0,0,""));       }
};

class MatrixOnAni : public NeoMatrixAni{
    public:
        MatrixOnAni():NeoMatrixAni((const char *) F("on"))    { }
        void reset() { config(AniCfg(0xF00000FF,0,0,0,"0x0000 00FF"));       }
};


class MatrixDimAni : public NeoMatrixAni{
    public:
        MatrixDimAni():NeoMatrixAni((const char *) F("dim"))    { }
        void reset() { config(AniCfg(0xF0000080,0,0,0,"0x0000 FF00"));       }
};


class MatrixBreathAni : public NeoMatrixAni{
    public:
        MatrixBreathAni():NeoMatrixAni((const char *) F("breath"))    { }
        void reset() { config(AniCfg(0xF0000010,0,0,0x010020D0,"0x0000 00FF"));      }
};

class MatrixBlinkAni : public NeoMatrixAni{
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
        MatrixBlinkAni():NeoMatrixAni((const char *) F("blink"))    { }
        void reset() { config(AniCfg(0xF00000FF,0xFFFF0100,0x05001901,0,"0x00FF 0000,0x0000 00FF"));      }
};



class MatrixMultiFlashAni : public NeoMatrixAni{
/*
	p3 : flash config:  xx xx xx xx   : def   : 
						|| || || ++---:  0x05 : 5 flash per group 
						|| || ++------:  0x03 : time t1 in 10ms:  30ms  flashtime        .. time of one flash
						|| ++---------:  0x0A : time t2 in 10ms:  100ms inter flash time .. time between flashes
						++------------:  0x32 : time t3 in 100ms: 5000ms inter group time .. time between groups
    5 flash need 10 color in list  Flash1-on-color, dark-color 
*/  
    public:
        MatrixMultiFlashAni():NeoMatrixAni((const char *) F("multi flash"))    { }
        void reset() { config(AniCfg(0xF00000FF,0xFFFF0100,0x05001901,0,"0x00FF0000,0, 0x0000FF00,0, 0x000000FF,0, 0x00FFFFFF,0, 0x00800080,0 "));      }
};

class MatrixRainbowAni : public NeoMatrixAni{
/*
	p2 : color Index:   xx xx xx xx   : def   :
						|| || || ++---: 0     : start index of color list 
						|| || ++------: 1     : each trigger next color
						++-++---------: 0A    : time between two color steps 10ms
    brightness:25%
*/ 
    public:
        MatrixRainbowAni():NeoMatrixAni((const char *) F("rainbow"))    { }
        void reset() { config(AniCfg(0xF0000040,0x000A0100,0,0,""));       }
};


