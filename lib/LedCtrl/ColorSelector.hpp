#pragma once
#include <Arduino.h>
#include <Debug.hpp>
#include "helper.h"
#include <SimpleList.hpp>


/*

	p2 : color Index:   xx xx xx xx   :
						|| || || ++---: start index of color list 
						|| || ||      :     color list is provided in cfg.str
						|| || ||      :     if cfg.str is empty color are taken from color wheel (0..255)
						|| || ++------: == 0   : no inc / dec 
						|| ||         : <  128 : inc step
						|| ||         : >= 128 : dec step   (-1 = 255, -2 = 254)
						|| ++---------: time t2 in ms: time between two color steps 
						||            : 0xxxFF xxxx wait for trigger 
						++------------: reserved

						standard color white, an be overwritten by color list or color wheel
						p2:0  & str:""             ==> constant white
						p2:0  & str:"0x0000 00FF"  ==> constant blue
					
		
*/

