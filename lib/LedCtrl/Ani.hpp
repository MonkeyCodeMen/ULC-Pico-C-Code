#pragma once
#include <Arduino.h>
#include <helper.h>
#include <SimpleList.hpp>
#include <ColorSelector.hpp>
extern class DimCtrl;
extern class FlashCtrl;
extern class BreathCtrl;

#define ANI_OK								0
#define ANI_ERROR_GENERAL 					1
#define ANI_ERROR_PROGRAM_DOES_NOT_EXIST	2
#define ANI_ERROR_OUT_OF_RANGE     			3
#define ANI_ERROR_INTERNAL      			4
#define ANI_ERROR_PAR_MISSING				5
#define ANI_ERROR_FILE_NOT_FOUND			6
#define ANI_ERROR_OUT_OF_MEMORY				7


class AniCfg{
	public:
		AniCfg (uint32_t p1=0,uint32_t p2=0,uint32_t p3=0,uint32_t p4=0,String str=""): _p1(p1),_p2(p2),_p3(p3),_p4(p4),_str(str) {}
		~AniCfg() = default;

		uint32_t _p1,_p2,_p3,_p4;
		String	 _str;
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
						|| || || ++---: dim increase for high level (p1 = low level)
						|| |+-++------: time t5 in ms: time for dim up
						++-++---------: time t6 in ms: time for dim down

	str: string parameter : examples:
					color list: "0x00FF 0000,0x0000 FF00,0x0000 00FF"
					file list:  "3*rainbow.gif,2*alien.gif"    .. play 3 times rainbow.gif, 2 times alien.gif


	
		
*/


class Ani
{
	public:
		Ani(const char * pName)  						{ _pName = pName;  reset();		}
		~Ani() = default;
        const char *   getName()						{ return _pName;				}
		virtual AniCfg getConfig()						{ return _cfg; 					}
		virtual void   reset()  						{ config(AniCfg());				}
        virtual int    config(AniCfg cfg) { 
			_cfg = cfg; 
			uint8_t ctrlByte = HHH_BYTE(cfg._p1);
			#define DIM_MASK	0x80
			#define COLOR_MASK	0x40
			#define FLASH_MASK  0x20
			#define DARTH_VADER 0x10

			if (ctrlByte & DIM_MASK)	_dimCtrl.config(cfg._p1);
			if (ctrlByte & COLOR_MASK)	_colorCtrl.config(cfg._p2,cfg._str);
			if (ctrlByte & FLASH_MASK)	_flashCtrl.config(cfg._p3);
			if (ctrlByte & DARTH_VADER)	_breathCtrl.config(cfg._p4);
			return ANI_OK;    
		}
		virtual void   trigger() 						{								}
		virtual void   loop(uint32_t now) {	
			_dimCtrl.loop(now);
			_colorCtrl.loop(now);							
			_flashCtrl.loop(now);
			_breathCtrl.loop(now);
		}

		static const char * getErrorText(int error)		{
			switch (error){
				case ANI_OK:							return "OK";
				case ANI_ERROR_GENERAL:					return "general error";
				case ANI_ERROR_PROGRAM_DOES_NOT_EXIST:	return "program does not exist";
				case ANI_ERROR_OUT_OF_RANGE:			return "parameter out of range";
				case ANI_ERROR_INTERNAL:				return "internal error";
				case ANI_ERROR_PAR_MISSING:				return "parameter missing";
				case ANI_ERROR_FILE_NOT_FOUND:			return "file not found";
				case ANI_ERROR_OUT_OF_MEMORY:			return "out of memory";
			}
			return "unknow error code";
		}


	protected:
		const char* 	_pName;
		AniCfg			_cfg;
		uint8_t			_dim;
		DimCtrl			_dimCtrl;
		ColorSelector 	_colorCtrl;
		FlashCtrl		_flashCtrl;
		BreathCtrl		_breathCtrl;






};
