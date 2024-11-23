
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
#include <NeoMatrixAni.hpp>

/*
	basic layout for configuration:

    the frame width is been given by event divider of color control
    if you want a rainbow color box with 3 lines thickness, then set the divider to 2 
    



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


		
*/

class MatrixBoxAni : public NeoMatrixAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  rect or circle (depends on constructor)
        -------+---------------+---------------------------
        p1:    | 0x0000 0020   |  0x0000 SSSS 
               |               |  S:step time in ms
        -------+---------------+---------------------------
        p2:    | 0x0000 0001   |  0x0000 00TT
               |               |  T: rect/circle line width
        -------+---------------+---------------------------
        p3:    | 0x0000 800A   |  0x00SS DDII
               |               |  S:start index for color wheel/list
               |               |  D:dim value for color
               |               |  I:inc step for color wheel/list (full wheel 255)
        -------+---------------+---------------------------
        p4:    | N/A           |  N/A
        -------+---------------+---------------------------
        str:   | COLOR_DEF_LIST|  if not empty this color list will be taken instead of rainbow colors
        -------+---------------+---------------------------
        pData: | N/A           |  bin color List .. n of uint32 (4bytes) L-Byte first 
    */

   

    public:
        MatrixBoxAni(const char * type):NeoMatrixAni(type){ 
            if      (strcmp(type,"rect") == 0)      _type = rect;
            else if (strcmp(type,"circle") == 0)    _type = circle;
            else if (strcmp(type,"hor") == 0)       _type = hor;
            else if (strcmp(type,"ver") == 0)       _type = ver;
            else _type = none;
        }

        void reset() { config(AniCfg(ANI_WR_ALL | 0x40,0x02200A00,0,0,""));   /* config(0x020,1,0x200A,0,""); */   }

        virtual int config(AniCfg cfg)  {
            _state      = stop;
            Ani::config(cfg);
            _state      = init;
            return ANI_OK;
        }

        virtual void loop(uint32_t time,Adafruit_NeoMatrix * pMatrix){
            uint32_t diff;
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    pMatrix->setBrightness(_dimCtrl.getDim());
                    _sizeX = pMatrix->width();
                    _sizeY = pMatrix->height();
                    _border= clamp(1,_dimCtrl.getSpeed(),100);
                    pMatrix->fillScreen(0);
                    _lastStartColor=0;
                    _lastDimValue = _dimCtrl.getDim();
                    _state = run;
                    break;
                
                case run:
                    Ani::loop(time);
                    if (hasChanged() == true){
                        ColorCtrl colorCtrlCopy = _colorCtrl;
                        colorCtrlCopy.switchToTriggerMode();
                        colorCtrlCopy.loop(0);
                        _update(pMatrix,colorCtrlCopy);
                    }
                    break;
            }
        }

 
    protected:
        enum BoxState {stop,init,run};
        volatile BoxState _state;
        enum BoxType {none,rect,circle,hor,ver};
        BoxType _type;        
        uint8_t  _border;
        uint32_t _sizeX,_sizeY;
        int32_t  _startX,_startY;
        uint32_t _lastStartColor;
        uint8_t  _lastDimValue;




        void   _update(Adafruit_NeoMatrix * pMatrix,ColorCtrl colorCtrl){
            switch (_type){
                case circle:    _drawCircles(pMatrix,colorCtrl);    break;
                case rect:      _drawRect(pMatrix,colorCtrl);       break;
                case hor:       _drawHor(pMatrix,colorCtrl);        break;
                case ver:       _drawVer(pMatrix,colorCtrl);        break;
                
                case none:      
                default:        pMatrix->fill(0);       

            }
            pMatrix->show();
        };

        void _drawCircles(Adafruit_NeoMatrix * pMatrix,ColorCtrl colorCtrl){
            uint32_t color24 = colorCtrl.getColor();
            uint16_t  color565;
            uint16_t  steps,x,y;
            int32_t  h,w;
            steps = _sizeX/2;
            if (_sizeY/2 < steps){
                steps = _sizeY/2;
            }
            for(int i=0; i < steps; i++){
                x=0+i;
                y=0+i;
                h = _sizeY - (2*i);
                w = _sizeX - (2*i);

                if ((h>0) && (w>0)){
                    color565 = toColor565(color24);                 
                    pMatrix->drawRoundRect(x,y,w,h,i,color565 );    // rect with rounded corner
                    colorCtrl.trigger();                            // change color or repeat color 
                    colorCtrl.loop(i+2);
                    color24 = colorCtrl.getColor();
                }
            }
        }

        void _drawRect(Adafruit_NeoMatrix * pMatrix,ColorCtrl colorCtrl){
            uint32_t color24 = colorCtrl.getColor();
            uint16_t  color565;
            uint16_t  steps,x,y;
            int32_t  h,w;
            steps = _sizeX/2;
            if (_sizeY/2 < steps){
                steps = _sizeY/2;
            }
            for(int i=0; i < steps; i++){
                x=0+i;
                y=0+i;
                h = _sizeY - (2*i);
                w = _sizeX - (2*i);

                if ((h>0) && (w>0)){
                    color565 = toColor565(color24);                 
                    pMatrix->drawRect(x,y,w,h,color565 );           // draw 4 lines (rect)    
                    colorCtrl.trigger();                            // change color or repeat color 
                    colorCtrl.loop(i+2);
                    color24 = colorCtrl.getColor();
                }
            }
        }

        void _drawHor(Adafruit_NeoMatrix * pMatrix,ColorCtrl colorCtrl){
            uint32_t color24 = colorCtrl.getColor();
            uint16_t  color565;
            uint16_t  steps = _sizeY;
            uint16_t  lastX  = _sizeX-1;
            for(int i=0; i < _sizeY; i++){
                    color565 = toColor565(color24);                 
                    pMatrix->drawLine(0,i,lastX,i,color565 );
                    colorCtrl.trigger();                            // change color or repeat color 
                    colorCtrl.loop(i+2);
                    color24 = colorCtrl.getColor();
            }
        }

        void _drawVer(Adafruit_NeoMatrix * pMatrix,ColorCtrl colorCtrl){
            uint32_t color24 = colorCtrl.getColor();
            uint16_t  color565;
            uint16_t  steps = _sizeX;
            uint16_t  lastY  = _sizeY-1;
            for(int i=0; i < _sizeX; i++){
                    color565 = toColor565(color24);
                    pMatrix->drawLine(i,0,i,lastY,color565 );
                    colorCtrl.trigger();                            // change color or repeat color 
                    colorCtrl.loop(i+2);
                    color24 = colorCtrl.getColor();
            }
        }
};
