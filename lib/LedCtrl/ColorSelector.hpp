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

class ColorSelector{
    #define COLOR_LIST_LENGTH 32
    public:
        ColorSelector() {  config(0,"");    };       // setup with default values 
        ~ColorSelector() = default;


        void config(uint32_t p=0,String colorList=""){
            config(L_BYTE(p),H_BYTE(p),HH_BYTE(p),colorList);
        }

        void config(uint8_t startIndex,uint8_t incStep,uint8_t time, String colorList){
            _run = (incStep == 0) ? false:true;
            _colorList.clear();
            _incStep   = (int8_t) incStep;
            _timeStep  = time;
            _firstLoop = true;
            _currentColorIndex = startIndex;
            _currentColor = 0x00FFFFFF; 
            _colorIndexMax = 255;
            _useColorWheel = true;

            if (colorList.length() > 0){
                StringList list(colorList.c_str(),',');
                while(list.isEndReached() == false){
                    _colorList.add(convertStrToInt(list.getNextListEntry()));
                }
                _colorIndexMax = _colorList.size();
                if (_colorIndexMax > 0){
                    _currentColor = _colorList.get(0);
                    _useColorWheel = false;
                }
            }
        }

        void loop(uint32_t now){
            if (_run == true){
                // check for first loop cycle
                if (_firstLoop == true){
                    _firstLoop = false;
                    _nextLoopTime = now+_timeStep;
                } else if (now >= _nextLoopTime){
                    // time for update
                    _nextLoopTime += _timeStep;
                    
                    // update Index
                    _currentColorIndex += _incStep;
                    if (_currentColorIndex > _colorIndexMax){
                        _currentColorIndex = 0;
                    }
                    
                    // now calc color based on index
                    if (_useColorWheel == true){
                        _currentColor = getColorWheel24Bit(_currentColorIndex);
                    } else {
                        _currentColor = _colorList.get(_currentColorIndex);
                    }
                }

            }
        }

        uint32_t getColor()     {   return _currentColor;   }

    private:
        bool                _run;
		uint8_t		        _currentColorIndex;
        uint32_t    	    _currentColor;
        uint8_t             _colorIndexMax;
		SimpleList<int32_t> _colorList;
        int8_t              _incStep;
        int8_t              _timeStep;
        bool                _firstLoop;
        bool                _useColorWheel;
        uint32_t            _nextLoopTime;

};