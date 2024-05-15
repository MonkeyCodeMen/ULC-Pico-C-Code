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
						p2:0  & str:"0x00FF FFFF"  ==> constant white
						p2:0  & str:"0x0000 00FF"  ==> constant blue
						p2:0xFFFF0100 & str:"0x00FF 0000,0x0000 00FF"   switch between red and blue on trigger event  start with red
					

	p3 : flash config:  xx xx xx xx   :
						|| || || ++---: flashes per group 
						|| || ||      : 0:  flash modul switched off
						|| || ++------: time t1 in 10ms:    flashtime        .. time of one flash
						|| ++---------: time t2 in 10ms: 	inter flash time .. time between two flashes
						++------------: time t3 in 100ms: 	inter group time .. time between two flash groups
									  :                     0xFFxx xxxx wait for trigger 
									  .. standard  colors : flash white   , pause black
									     can be overwritten by str color list   first color flash (if provided)
										 										2nd color   pause (if provided)

						0xXXXX XX00 flash module switched off


	p4 : breath config: xx xx xx xx   :
						|| || || ++---: delta dim increase (clipped at 255) for high level (p1 = low level) 
						|| || ||      : 0: switch breath module off
						|| |+-++------: time t5 in 100ms: time for dim up
						++-++---------: time t6 in 100ms: time for dim down

	str: string parameter : examples:
					color list: "0x00FF 0000,0x0000 FF00,0x0000 00FF"
					file list:  "3*rainbow.gif,2*alien.gif"    .. play 3 times rainbow.gif, 2 times alien.gif
		
*/


class AniCfg{
	public:
		AniCfg (uint32_t p1=0,uint32_t p2=0,uint32_t p3=0,uint32_t p4=0,String str=""): _p1(p1),_p2(p2),_p3(p3),_p4(p4),_str(str) {}
		~AniCfg() = default;

		uint32_t _p1,_p2,_p3,_p4;
		String	 _str;
};


class DimCtrl{
	public:
		DimCtrl():_dim(0),_speed(0)		{ config(0); 							}
		~DimCtrl() = default;

        void config(uint32_t p=0);
		uint32_t doDim(uint32_t color)	{ return dimColor255(color,_dim);		}
		uint8_t  getDim()				{ return _dim;							}
		uint16_t getSpeed()				{ return _speed;						}
		void loop(uint32_t now)			{ /* nothing to do*/ 				   	}

	private:
		uint8_t _dim;
		uint16_t _speed;
};


class ColorCtrl{
    public:
        ColorCtrl() 									{  config(0,"");    }       // setup with default values 
        ~ColorCtrl() = default;
        void loop(uint32_t now);
        uint32_t getColor()     						{ return _currentColor;   								}
		void trigger()									{ _triggerActive = true; 								}
		uint32_t getConfigP()							{ return _configParam;									}
        void config(uint32_t p=0,String colorList="")	{ _config(L_BYTE(p),H_BYTE(p),H_BYTE(p),HHH_BYTE(p),colorList);}
		void switchToTriggerMode();


    private:
		enum ColorState 	{stop,initTime,waitTrigger,waitTime};
		volatile ColorState _state;
        bool                _useColorWheel;
		volatile bool		_triggerActive;
		uint16_t            _eventCounter,_eventTarget;
        uint32_t            _nextLoopTime,_timeStep;
		uint8_t		        _currentColorIndex;
        uint32_t    	    _currentColor,_secondColor;
        uint8_t             _colorIndexMax;
		SimpleList<int32_t> _colorList;
        int8_t              _incStep;
		uint32_t            _configParam;
		String				_configStr;

		void _checkForUpdate();
        void _config(uint8_t startIndex,uint8_t incStep,uint8_t time, uint8_t divider, String colorList);

};


class FlashCtrl{
	public:
		FlashCtrl()													{ config(0);											}		
		~FlashCtrl() = default;

		void config(uint8_t p=0);
		bool loop(uint32_t now);		// if return == true : call trigger of colorCtrl
		void trigger()												{ _triggerActive = true; 								}
		uint32_t selectColor(uint32_t colorOn,uint32_t colorOff)	{ return (_state == flashOn) ? colorOn : colorOff;		}
	
	private:
		enum FlashState 	{stop,init,flashOn,flashOff,pause,waitTrigger};
		volatile FlashState _state;
		bool 				_waitForTrigger;
		volatile bool		_triggerActive;
        int32_t             _t1,_t2,_t3;
        uint32_t            _nextLoopTime;
		uint8_t				_groupCount,_groupCounter;
};



class BreathCtrl{
	public:
		BreathCtrl()	{ config(0);	}
		~BreathCtrl() = default;

		void config(uint32_t p=0);
		uint8_t modifyDimFactor(uint8_t dim)	{ return (uint8_t) clamp((uint32_t)0,(uint32_t) ((uint8_t)dim + (uint16_t)_dimDelta),(uint32_t)255);		}
		void loop(uint32_t now);

	private:
		enum BreathState 	{stop,init,up,down};
		volatile BreathState _state;
		uint8_t _dimDelta,_target;
		uint32_t _t5,_t6,_lastTurn;
};



class Ani{
	public:
		Ani(const char * pName)  						{ _pName = pName;  reset();		}
		~Ani() = default;
        const char *   getName()						{ return _pName;				}
		virtual AniCfg getConfig()						{ return _cfg; 					}
		virtual void   reset();
        virtual int    config(AniCfg cfg);
		virtual void   trigger() 						{ _flashCtrl.trigger();			}
		virtual void   loop(uint32_t now);
		static const char * getErrorText(int error);

		virtual uint8_t  getDim()                      	{ return _dim;					}
		virtual uint32_t getColor()						{ return _color;				}
		virtual uint16_t getSpeed()						{ return _dimCtrl.getSpeed();	}

		virtual bool     hasChanged();

	protected:
		const char* 	_pName;
		AniCfg			_cfg;

		DimCtrl			_dimCtrl;
		ColorCtrl 		_colorCtrl;
		FlashCtrl		_flashCtrl;
		BreathCtrl		_breathCtrl;

		uint8_t			_dim,_dimLast;
		uint32_t		_color,_colorLast;

};
