#pragma once
#include <NeoMatrixAni.hpp>


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

        void reset() { setup(0x020,1,0x200A,0,"",0,NULL); };
       
        virtual int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData)  {
            _state      = stop;
            _incTime    = L_WORD(p1);
            _border     = L_BYTE(p2);
            _colorGen.setup(H_BYTE(p3),HH_BYTE(p3),L_BYTE(p3),str,length,*pData);
            _state      = init;
            return ANI_OK;
        }

        void loop(uint32_t time,Adafruit_NeoMatrix * pMatrix){
            uint32_t diff;
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _lastCallTime = time;
                    pMatrix->setBrightness(matrix_default_brightness);
                    _sizeX = pMatrix->width();
                    _sizeY = pMatrix->height();
                    pMatrix->fillScreen(0);
                    _setNextColor(pMatrix);
                    _state = run;
                    break;
                
                case run:
                    diff = time - _lastCallTime;
                    if (diff >= _incTime){
                        _lastCallTime = time;
                        _setNextColor(pMatrix);
                    }
                    break;
            }
        }

 
    protected:
        enum BoxState {stop,init,run};
        volatile BoxState _state;
        enum BoxType {none,rect,circle,hor,ver};
        BoxType _type;        
        uint32_t _lastCallTime;
        uint16_t  _incTime;
        uint8_t  _border,_dim,_inc,_start;
        uint32_t _sizeX,_sizeY;
        int32_t  _startX,_startY;
        bool _circle;
        ColorCtrl _colorGen;

        void   _setNextColor(Adafruit_NeoMatrix * pMatrix){
            uint32_t startColor;
            _colorGen.restore();
            startColor = _colorGen.getNextColor();
            _colorGen.backup(); // start next time with this color 

            switch (_type){
                case none:      pMatrix->fill(0);                   break;
                case circle:    _drawCircles(pMatrix,startColor);   break;
                case rect:      _drawRect(pMatrix,startColor);      break;
                case hor:       _drawHor(pMatrix,startColor);       break;
                case ver:       _drawVer(pMatrix,startColor);       break;

            }
            
            pMatrix->show();
        };

        void _drawCircles(Adafruit_NeoMatrix * pMatrix,uint32_t startColor){
            uint32_t color24 = startColor;
            uint16_t  color565;
            uint8_t  borderLine = 0;
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
                    // get color from color wheel
                    color565 = toColor565(color24);
                    // rect with rounded corner
                    pMatrix->drawRoundRect(x,y,w,h,i,color565 );
                    // change color or repeat color 
                    borderLine++;
                    if(borderLine >= _border){
                        color24 = _colorGen.getNextColor();
                        borderLine = 0;
                    }
                }
            }
        }

        void _drawRect(Adafruit_NeoMatrix * pMatrix,uint32_t startColor){
            uint32_t color24 = startColor;
            uint16_t  color565;
            uint8_t  borderLine = 0;
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
                    // get color from color wheel
                    color565 = toColor565(color24);
                    // draw 4 lines (rect)    
                    pMatrix->drawRect(x,y,w,h,color565 );
                    // change color or repeat color 
                    borderLine++;
                    if(borderLine >= _border){
                        color24 = _colorGen.getNextColor();
                        borderLine = 0;
                    }
                }
            }
        }

        void _drawHor(Adafruit_NeoMatrix * pMatrix,uint32_t startColor){
            uint32_t color24 = startColor;
            uint16_t  color565;
            uint8_t  borderLine = 0;
            uint16_t  steps = _sizeY;
            uint16_t  lastX  = _sizeX-1;
            for(int i=0; i < _sizeY; i++){
                    color565 = toColor565(color24);
                    pMatrix->drawLine(0,i,lastX,i,color565 );
                    // change color or repeat color 
                    borderLine++;
                    if(borderLine >= _border){
                        color24 = _colorGen.getNextColor();
                        borderLine = 0;
                    }
            }
        }

        void _drawVer(Adafruit_NeoMatrix * pMatrix,uint32_t startColor){
            uint32_t color24 = startColor;
            uint16_t  color565;
            uint8_t  borderLine = 0;
            uint16_t  steps = _sizeX;
            uint16_t  lastY  = _sizeY-1;
            for(int i=0; i < _sizeX; i++){
                    color565 = toColor565(color24);
                    pMatrix->drawLine(i,0,i,lastY,color565 );
                    // change color or repeat color 
                    borderLine++;
                    if(borderLine >= _border){
                        color24 = _colorGen.getNextColor();
                        borderLine = 0;
                    }
            }
        }
};
