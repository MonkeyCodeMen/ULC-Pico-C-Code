
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
#include <helper.h>
#include <SimpleList.hpp>


#define ANI_OK								0
#define ANI_ERROR_GENERAL 					1
#define ANI_ERROR_PROGRAM_DOES_NOT_EXIST	2
#define ANI_ERROR_OUT_OF_RANGE     			3
#define ANI_ERROR_INTERNAL      			4
#define ANI_ERROR_PAR_MISSING				5
#define ANI_ERROR_FILE_NOT_FOUND			6
#define ANI_ERROR_OUT_OF_MEMORY				7

#define ANI_WR_ALL      0xF0000000
#define ANI_WR_DIM		0x80000000
#define ANI_WR_COLOR	0x40000000
#define ANI_WR_FLASH  	0x20000000
#define ANI_DARTH_VADER 0x10000000

/*
	Ani is a base class for led animations

	it contains base fucntionality for :
		+ dim handling 
		+ color handling (incl. color list and color wheel)
		+ flash and multi flash 
		+ breath animation 
	
	the configuration is been done over 4 32-bit configuration paramter (one for each feature) plus a string () for color lists etc

	the idea is that other concret classes derive from this base class and utilize the pretested color switch, dimming etc.
	by doing this not only the functoinality is pretested, although the configuration interface is the same for a RGB LED and and standard LED channel and a NEO pixel amtrix
	so all of this objects could be dimmed up or down with the same command / function call / menu action / hotekey event / com frame

	the Ani class has three main interfaces:
	+ constrcutor
	+ loop   (including set trigger and hasChanged)
	+ config 

	1) the derived class/objects must atke care the the loop function is been called
	2) can check over hasChanged()  if there is a new color or dim rate available
	3) can request the current dim rate and color to perform their updates of their LED's
	
	a connection between the color list and the flash functionality is implemented to change color on flashes

	for examples I would like to refer to the unity tests: AniTest.cpp in this project

	Note:
	the ani class is constructed for loop functionality 
	so a config call will never influence directly the dim or color values 
	after a config you need to call at least one time the loop interface to update the values

*/


/*
	the basic configuration is been done over 4 parameter (sets)
	for : dimming general, color, flash / multiflashes and breath behavior

	all of this controls can be combined (even if some combinations does not make sense at all)

	the configuration is very much like a register definition .. sorry i came from embedded
	so for easy access I define unions / structs for various ways of accessing the invidual parameters 

	for some examples see here the unity module tests too (AniTest.cpp)

	dimCfg :         	xx xx xx xx   :
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

						dimCfg: 0x8000 0030   set to dim level 0x30
						dimCfg: 0x8000 1000 inc by 10 ==> results in 0x30 + 0x10 = 0x40 dim level 

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
						|| || || ++---: flashes per group 
						|| || ||      : 0:  flash modul switched off
						|| || ++------: time t1 in 10ms:    flashtime        .. time of one flash
						|| ++---------: time t2 in 10ms: 	inter flash time .. time between two flashes
						++------------: time t3 in 100ms: 	inter group time .. time between two flash groups
									  :                     0xFFxx xxxx wait for trigger 
						0xXXXX XX00 flash module switched off


	breathCfg :         xx xx xx xx   :
						|| || || ++---: delta dim increase (clipped at 255) for high level (dimCfg = low level) 
						|| || ||      : 0: switch breath module off
						|| || ++------: time t1 in 100ms: time for dim up
						|| ++---------: time t2 in 100ms: time for dim down
						++------------: reserved

	str: string parameter : examples:
					color list: "0x00FF 0000,0x0000 FF00,0x0000 00FF"
					file list:  "3*rainbow.gif,2*alien.gif"    .. play 3 times rainbow.gif, 2 times alien.gif
		
*/


union dimCtrl_u {
    uint32_t uint32;
	struct {
        uint8_t L; // least significant byte
        uint8_t H;
        uint8_t HH;
        uint8_t HHH; // most significant byte
    } byte;
    struct {
		uint8_t 	setValue;
		int8_t  	incValue;
		unsigned    speed:12;
		unsigned    WR_breath:1;
		unsigned	WR_flash:1;
		unsigned    WR_color:1;
		unsigned    WR_dim:1;
    } reg;
};
typedef union dimCtrl_u dimCtrl_t;

union colorCtrl_u {
    uint32_t uint32;
	struct {
        uint8_t L; // least significant byte
        uint8_t H;
        uint8_t HH;
        uint8_t HHH; // most significant byte
    } byte;
    struct {
		uint8_t 	startIndex;
		int8_t  	incValue;
		uint8_t     t2_ms;
		uint8_t     eventCounter;
    } reg;
};
typedef union colorCtrl_u colorCtrl_t;

union flashCtrl_u {
    uint32_t uint32;
	struct {
        uint8_t L; // least significant byte
        uint8_t H;
        uint8_t HH;
        uint8_t HHH; // most significant byte
    } byte;
    struct {
		uint8_t 	flashPerGroup;
		int8_t  	t1_10ms;
		uint8_t     t2_10ms;
		uint8_t     t3_100ms;
    } reg;
};
typedef union flashCtrl_u flashCtrl_t;

union breathCtrl_u {
    uint32_t uint32;
    struct {
		uint8_t 	delta;
		uint8_t     t1_100ms;
		uint8_t     t2_100ms;
		uint8_t     reserved;
    } reg;
};
typedef union breathCtrl_u breathCtrl_t;



class AniCfg{
	public:
		AniCfg (uint32_t dimCfg=0,uint32_t colorCfg=0,uint32_t flashCfg=0,uint32_t breathCfg=0,String str=""): 
			dimCfg{.uint32=dimCfg},
			colorCfg{.uint32=colorCfg},
			flashCfg{.uint32=flashCfg},
			breathCfg{.uint32=breathCfg},
			str(str) 
			{}
		AniCfg(const AniCfg & src) {*this = src;}		// copy constructor
		~AniCfg() = default;

		virtual void clear(){
			dimCfg.uint32    = 0;
			colorCfg.uint32  = 0;
			flashCfg.uint32  = 0; 
			breathCfg.uint32 = 0;
			str              = "";
		}

		virtual void operator=(const AniCfg & src){
			dimCfg    = src.dimCfg;
			colorCfg  = src.colorCfg;
			flashCfg  = src.flashCfg;
			breathCfg = src.breathCfg;
			str		  = src.str;
		}
		
		dimCtrl_t 	 	dimCfg;
		colorCtrl_t  	colorCfg;
		flashCtrl_t  	flashCfg;
		breathCtrl_t 	breathCfg;
		String	 		str;
};


class DimCtrl{
	public:
		DimCtrl() 						{ config(); 							}
		~DimCtrl() = default;

        virtual void config(uint32_t p=0)		{dimCtrl_t cfg; cfg.uint32=p; config(cfg);	}
		virtual void config(dimCtrl_t cfg); 
		virtual uint32_t doDim(uint32_t color)	{ return dimColor255(color,_dim);		}
		virtual uint8_t  getDim()				{ return _dim;							}
		virtual uint16_t getSpeed()				{ return _speed;						}
		virtual void loop(uint32_t now)			{ /* nothing to do*/ 				   	}

	private:
		uint8_t _dim;
		uint16_t _speed;
};

class ColorCtrl{
	public:
		#define COLOR_WHEEL		0x01
		#define COLOR_LIST		0x02
		#define LOOP_STOP		0x10
		#define LOOP_TRIGGER	0x20
		#define LOOP_TIME   	0x30
		enum ColorState 		{stop,initTime,waitTrigger,waitTime};

        ColorCtrl() 											{  config();    									}
        ~ColorCtrl() = default;

        virtual void loop(uint32_t now);
        virtual uint32_t getColor()     						{ return _currentColor;   							}
		virtual void trigger()									{ _triggerActive = true; 							}
        virtual void config(uint32_t p=0,String str="0")		{ colorCtrl_t cfg; cfg.uint32=p; config(cfg,str);	}		// default static color with value 0
        virtual void config(colorCtrl_t cfg,String str);
		virtual void switchToTriggerMode();

		virtual uint8_t getMode()								{ return _mode;				}
		virtual uint8_t getMaxIndex()							{ return _colorIndexMax;	}
		virtual ColorState getState()							{ return _state;			}
		virtual uint32_t getNextLoopTime()						{ return _nextLoopTime;		}

		virtual void operator=(ColorCtrl & src);


    private:
		volatile ColorState _state;
		uint8_t           	_mode;

        bool                _useColorWheel;
		volatile bool		_triggerActive;
		uint16_t            _eventCounter,_eventTarget;
        uint32_t            _nextLoopTime,_timeStep;
		uint8_t		        _currentColorIndex;
        uint32_t    	    _currentColor;
        uint8_t             _colorIndexMax;
		SimpleList<int32_t> _colorList;
        int8_t              _incStep;

		virtual void _checkForUpdate();

};


class FlashCtrl{
	public:
		enum FlashState 						{stop,init,flashOn,flashOff,pause,waitTrigger};
		
		FlashCtrl()								{ config();										}		
		~FlashCtrl() = default;

        virtual void config(uint32_t p=0)		{ flashCtrl_t cfg; cfg.uint32=p; config(cfg);	}
		virtual void config(flashCtrl_t cfg); 
		virtual bool loop(uint32_t now);		// if return == true : call trigger of colorCtrl
		virtual void trigger()					{ _triggerActive = true; 						}
		virtual FlashState getState()			{ return _state;								}

	private:
		volatile FlashState _state;
		bool 				_waitForTrigger;
		volatile bool		_triggerActive;
        int32_t             _t1,_t2,_t3;
        uint32_t            _nextLoopTime;
		uint8_t				_groupCount,_groupCounter;
};



class BreathCtrl{
	public:
		enum BreathState 	{stop,init,up,down};

		BreathCtrl()									{ config(0);									}
		~BreathCtrl() = default;

        virtual void config(uint32_t p=0)				{ breathCtrl_t cfg; cfg.uint32=p; config(cfg);	}
		virtual void config(breathCtrl_t cfg); 
		virtual uint8_t modifyDimFactor(uint8_t dim)	{ return clamp(0,dim + _dimDelta,255);			}
		virtual void loop(uint32_t now);
		virtual BreathState getState()					{ return _state;								}
	
	private:
		volatile BreathState _state;
		uint8_t _dimDelta,_target;
		uint32_t _t1,_t2,_lastTurn;
};



class Ani{
	public:
		Ani(const char * pName)  						{ _pName = pName;  reset();		}
		~Ani() = default;
        const char *   getName()						{ return _pName;				}
//		virtual AniCfg getConfig()						{ return _cfg; 					}
		virtual void   reset();
        virtual int    config(AniCfg cfg);
		virtual void   triggerFlash() 					{ _flashCtrl.trigger();			}
		virtual void   triggerColor() 					{ _colorCtrl.trigger();			}
		virtual void   loop(uint32_t now);
		virtual String dump();

		static const char * getErrorText(int error);

		virtual uint8_t  getDim()                      	{ return _dim;					}
		virtual uint32_t getColor()						{ return _color;				}
		virtual uint16_t getSpeed()						{ return _dimCtrl.getSpeed();	}

		virtual bool     hasChanged();


	protected:
		const char* 	_pName;
//		AniCfg			_cfg;		// TODO .. brauchen wir das ????

		DimCtrl			_dimCtrl;
		ColorCtrl 		_colorCtrl;
		FlashCtrl		_flashCtrl;
		BreathCtrl		_breathCtrl;

		uint8_t			_dim,_dimLast;
		uint32_t		_color,_colorLast;
		bool            _newConfig;
};
