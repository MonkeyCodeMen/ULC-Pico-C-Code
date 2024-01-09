#pragma once
#include "Ani.hpp"
#include <Adafruit_NeoMatrix.h>
#include "helper.h"
#include "Debug.hpp"
#include <ColorSelector.hpp>
#include <AnimatedGIF.h>
#include <SD.h>
#include "PinMapping.h"


#define matrix_default_brightness   0xA0

class NeoMatrixAni:public Ani
{
	public:
		NeoMatrixAni(const char * pName) : Ani(pName) {};
		~NeoMatrixAni() = default;
		
		virtual void loop(uint32_t time,Adafruit_NeoMatrix * pMatrix) {};
        virtual void reset()                            {};
        virtual int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData) {return ANI_OK;};

};

class MatrixOffAni : public NeoMatrixAni{
    public:
        MatrixOffAni():NeoMatrixAni(F_CHAR("off"))      {};
        void reset() {_color = 0; _needUpdate = true;};
        void loop(uint32_t time,Adafruit_NeoMatrix * pMatrix) {
            if (_needUpdate == true){
                pMatrix->fillScreen(_color);
                pMatrix->show();
                _needUpdate = false;
            }
        };
    private:
        bool    _needUpdate;
        uint16_t    _color;

};

class MatrixStaticAni : public NeoMatrixAni{
     /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  static
        -------+---------------+---------------------------
        p1:    | 0x0000 00FF   |  0x00RR GGBB  
               |               |  R: red value
               |               |  G: green value
               |               |  B: blue value
        -------+---------------+---------------------------
        p2:    | N/A           |  N/A     
        -------+---------------+---------------------------
        p3:    | N/A           |  N/A
        -------+---------------+---------------------------
        p4:    | N/A           |  N/A
        -------+---------------+---------------------------
        str:   | N/A           |  N/A
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */
   public:
        MatrixStaticAni():NeoMatrixAni(F_CHAR("static"))      {};
        void reset() {  setup(0xFF,0,0,0,"",0,NULL); };

        int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData) {
            _state = stop;
            _color = p1&0xFFFFFF;
            _state = init;
            return ANI_OK;
        };

        void loop(uint32_t time,Adafruit_NeoMatrix * pMatrix) {
            uint16_t  color565; 
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    pMatrix->setBrightness(matrix_default_brightness);
                    color565 = toColor565(_color);
                    pMatrix->fillScreen(color565);
                    pMatrix->show();
                    _state = run;
                    break;
                
                case run:
                    // do nothing .. it's static
                    break;
            }
        };

    private:
        enum StaticState {stop,init,run};
        volatile StaticState _state;
        uint32_t   _color;
        uint8_t    _dim;

};

class MatrixRainbowAni : public NeoMatrixAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  dim
        -------+---------------+---------------------------
        p1:    | 0x0000 000A   |  step time in ms
        -------+---------------+---------------------------
        p2:    | N/A           |  N/A
        -------+---------------+---------------------------
        p3:    | 0x0000 5001   |  0x00SS DDII
               |               |  S:start index for color wheel/list
               |               |  D: dim level 
               |               |  I:inc step for color wheel/list (full wheel 255)
        -------+---------------+---------------------------
        p4     | N/A           | N/A
        -------+---------------+---------------------------
        str    | ""            | color list
        -------+---------------+---------------------------
        pData: | N/A           |  length (multiple of 4),pData:
               |               |    color list       
    */    
    public:
        MatrixRainbowAni()  : NeoMatrixAni(F_CHAR("rainbow")) {};
        
        void reset() {  setup(0x0040,0,0x5001,0,"",0,NULL); };
        int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData)  {
            _state      = stop;
            _timeInc    = clamp(1,p1,100000);
            _colorGen.setup(H_BYTE(p3),HH_BYTE(p3),L_BYTE(p3),str,length,*pData);            
            _state      = init;
            return ANI_OK;
        };
 
        void loop(uint32_t time,Adafruit_NeoMatrix * pMatrix){
            uint32_t diff;
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state = run;
                    _lastCallTime = time;
                    pMatrix->fillScreen(toColor565(_colorGen.getNextColor()));
                    pMatrix->show();
                    break;
                
                case run:
                    diff = time - _lastCallTime;
                    if (diff >= _timeInc){
                        _lastCallTime = time;
                        pMatrix->fillScreen(toColor565(_colorGen.getNextColor()));
                        pMatrix->show();
                    }
                    break;
            }
        };


    private:
        enum RainbowState {stop,init,run};
        volatile RainbowState _state;
        uint32_t _timeInc,_lastCallTime;
        ColorSelector _colorGen;
};

class MatrixBreathAni : public NeoMatrixAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  breath
        -------+---------------+---------------------------
        p1:    | 0x000A 2020   |  0xSSSS UUDD
               |               |  S: step time in ms
               |               |  U: nr steps for dim up
               |               |  D: nr steps for dim down
        -------+---------------+---------------------------
        p2:    | 0x0000 FF10   |  0x0000 UULL
               |               |  U: upper dim limit
               |               |  B: lower dim limit
        -------+---------------+---------------------------
        p3:    | 0x0000 0001   |  0x00SS 00II
               |               |  S:start index for color wheel/list
               |               |  I:inc step for color wheel/list (full wheel 255)
        -------+---------------+---------------------------
        p4     | N/A           | N/A
        -------+---------------+---------------------------
        str    | ""            | color list
        -------+---------------+---------------------------
        pData: | N/A           |  length (multiple of 4),pData:
               |               |    color list       


        breath blue only:
        setup(0x00204040,0xFF10,0,0,"0x000000FF",0,NULL)

        breath rainbow:
        setup(0x00204040,0x0000FF10,1,0,"",0,NULLL)

        breath color list:
        setup(0x02000404,0x0000FF10,1,0,"0xFFFFFF,0x00FF0000,0x00FF00,0x0000FF",0,NULL)

    */

    public:
        MatrixBreathAni():NeoMatrixAni(F_CHAR("breath"))      {};
        
        void reset() {  setup(0x00204040,0x0000FF10,1,0,"",0,NULL); };
        int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData)  {
            _state = stop; 
            _stepTime  = H_WORD(p1);
            _upSteps   = H_BYTE(p1);
            _downSteps = L_BYTE(p1);
            _upperLimit = H_BYTE(p2);
            _lowerLimit = L_BYTE(p2);
            _colorGen.setup(255,HH_BYTE(p3),L_BYTE(p3),str,length,*pData);

            // do some basic checks/correction of parameter set
            if (_lowerLimit > _upperLimit){
                uint8_t temp = _upperLimit;
                _upperLimit = _lowerLimit;
                _lowerLimit = temp;
            }
            if (_upSteps == 0)      _upSteps = 1;
            if (_downSteps == 0)    _downSteps = 1;
            if (_stepTime == 0)     _stepTime = 1;

            _state = init;
            return ANI_OK;
        };
 
        void loop(uint32_t time,Adafruit_NeoMatrix * pMatrix){
            uint32_t diff,color24;
            uint8_t dim;
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state       = up;
                    _stepCounter = 0;
                    _lastUpdate  = time;
                    _dimDiff     = _upperLimit-_lowerLimit;
                    dim          = _lowerLimit;
                    color24      = _colorGen.getNextColor(dim);
                    pMatrix->fillScreen(toColor565(color24));
                    pMatrix->show();
                    break;
                
                case up:
                    diff = time-_lastUpdate;
                    if (diff >= _stepTime){
                        _lastUpdate = time;
                        if (_stepCounter >= _upSteps){
                            dim = _upperLimit;
                            _stepCounter=0;    
                            _state = down;
                        } else {
                            dim = _lowerLimit + (_stepCounter * _dimDiff) / _upSteps;
                            _stepCounter++;
                        }
                        color24      = _colorGen.getNextColor(dim);
                        pMatrix->fillScreen(toColor565(color24));
                        pMatrix->show();
                    } 
                    break;
                
                case down:
                    diff = time-_lastUpdate;
                    if (diff >= _stepTime){
                        _lastUpdate = time;
                        if (_stepCounter >= _downSteps){
                            dim = _lowerLimit;
                            _stepCounter=0;    
                            _state = up;
                        } else {
                            dim = _upperLimit - (_stepCounter * _dimDiff) / _downSteps;
                            _stepCounter++;
                        }
                        color24      = _colorGen.getNextColor(dim);
                        pMatrix->fillScreen(toColor565(color24));
                        pMatrix->show();
                    } 
            }
        };
    private:
        enum BreathState {stop,init,up,down};
        volatile BreathState _state;
        uint32_t   _stepTime,_upSteps,_downSteps;
        uint32_t   _stepCounter,_lastUpdate;
        uint8_t    _upperLimit,_lowerLimit;
        uint8_t    _dimDiff;
        ColorSelector _colorGen;

};

class MatrixMultiFlashAni : public NeoMatrixAni{

    /*  
         ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  dim
        -------+---------------+---------------------------
        p1:    | 0x0020 0060   |  0xAAAA BBBB
               |               |  A: on Time in ms       
               |               |  B: off Time in ms       
        -------+---------------+---------------------------
        p2:    | 0x0002 01F4   |  0xAAAA BBBB
               |               |  A: count of flash group
               |               |  B: pause time between flash group      
        -------+---------------+---------------------------
        p3:    | 0x0000 FF01   |  0x00SS DDII
               |               |  S:start index for color wheel/list
               |               |  D: dim level 
               |               |  I:inc step for color wheel/list (full wheel 255)
        -------+---------------+---------------------------
        p4     | N/A           | N/A
        -------+---------------+---------------------------
        str    | color list    | color list
               | "0xFFFFFF,0x00FF0000,0x00FF00,0x0000FF"  
        -------+---------------+---------------------------
        pData: | N/A           |  length (multiple of 4),pData:
               |               |    color list      

    */
    public:
        MatrixMultiFlashAni():NeoMatrixAni(F_CHAR("multi flash"))      {};

        void reset() { setup(0x00200060,0x000201F4,0xFF01,0,"0xFFFFFF,0x00FF0000,0x00FF00,0x0000FF",0,NULL); };
       
        virtual int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData)  {
            _state      = stop;
            _onTime     = H_WORD(p1);
            _offTime    = L_WORD(p1);
            _flashCount = H_WORD(p2);
            _pauseTime  = L_WORD(p2);
            _colorGen.setup(H_BYTE(p3),HH_BYTE(p3),L_BYTE(p3),str,length,*pData);
            _state      = init;
            return ANI_OK;
        };

        void loop(uint32_t time,Adafruit_NeoMatrix * pMatrix){
            uint32_t diff;
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state = flashOn;
                    _lastCallTime = time;
                    pMatrix->setBrightness(matrix_default_brightness);
                    pMatrix->fillScreen(0);
                    _setNextColor(pMatrix);
                    break;
                
                case flashOn:
                    diff = time - _lastCallTime;
                    if (diff >= _onTime){
                        _lastCallTime = time;
                        _setMatrixOff(pMatrix);
                        _count++;
                        if (_count >= _flashCount){
                            _state = pause;
                        } else {
                            _state = flashOff;
                        }
                    }
                    break;

                case flashOff:
                    diff = time - _lastCallTime;
                    if (diff >= _offTime){
                        _lastCallTime = time;
                        _setNextColor(pMatrix);
                        _state = flashOn;
                    }
                    break;

                case pause:
                    diff = time - _lastCallTime;
                    if (diff >= _pauseTime){
                        _lastCallTime = time;
                        _setNextColor(pMatrix);
                        _count = 0;
                        _state = flashOn;
                    }
                    break;
            }
        };

 
    protected:
        enum MultiFlashState {stop,init,flashOn,flashOff,pause};
        volatile MultiFlashState _state;
        uint32_t _lastCallTime;
        uint16_t  _onTime,_offTime;
        uint16_t  _pauseTime;
        uint16_t  _flashCount,_count;
        ColorSelector _colorGen;


        void   _setNextColor(Adafruit_NeoMatrix * pMatrix){
            uint32_t color24  = _colorGen.getNextColor();
            uint16_t  color565 = toColor565(color24);
            pMatrix->fillScreen(color565);
            pMatrix->show();
        };

        void _setMatrixOff(Adafruit_NeoMatrix * pMatrix){
            pMatrix->fillScreen(0);
            pMatrix->show();
        }
};

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
        };

        void reset() { setup(0x020,1,0x800A,0,"",0,NULL); };
       
        virtual int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData)  {
            _state      = stop;
            _incTime    = L_WORD(p1);
            _border     = L_BYTE(p2);
            _colorGen.setup(H_BYTE(p3),HH_BYTE(p3),L_BYTE(p3),str,length,*pData);
            _state      = init;
            return ANI_OK;
        };

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
        };

 
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
        ColorSelector _colorGen;

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
        };

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
        };

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
        };

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
        };



};
 
class MatrixGifFileAni : public NeoMatrixAni{
    /*  

        ToDO neue GIF class die jedes Bild einzeln lädt (Wegen Speicher verbrauch !!!)

        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  gif
        -------+---------------+---------------------------
        p1:    | 0x0000 0080   |  0xCCCC 00DD     
               |               |  D:brightness 0..255
               |               |  C:repeat count, 0 = endless   
        -------+---------------+---------------------------
        p2:    |               |  N/A
        -------+---------------+---------------------------
        p3:    |               |  N/A
        -------+---------------+---------------------------
        p4:    |               |  N/A
        -------+---------------+---------------------------
        str:   | "TEST.GIF"    |  filename of gif file on SD card
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */
    public:
        MatrixGifFileAni():NeoMatrixAni(F_CHAR("gif"))      {
            _gif.begin(GIF_PALETTE_RGB888 );
        };
        
        void reset() { setup(0x80,0,0,0,"TEST.GIF",0,NULL); };

        void loop(uint32_t time,Adafruit_NeoMatrix * pMatrix){
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    pMatrix->fillScreen(0);
                    pMatrix->setBrightness(_brightness);
                    _gif.open((const char *)_fileName.c_str(),_GIFOpenFile, _GIFCloseFile, _GIFReadFile, _GIFSeekFile, _GIFDraw);
                    _count = 0;
                    _state = run;
                    break;
                
                case run:
                    if (time-_lastFrame >= _wait){
                        _lastFrame = time;
                        bool res = _gif.playFrame(false,&_wait,pMatrix);
                        if(res==false){
                            _gif.close();
                            if (_repeat > 0){
                                _count++;
                                if (_count >= _repeat){
                                    _state = stop;
                                    //_gif.close();
                                    break;
                                }
                            }
                            //_gif.reset();
                            _gif.open((const char *)_fileName.c_str(),_GIFOpenFile, _GIFCloseFile, _GIFReadFile, _GIFSeekFile, _GIFDraw);
                        }
                    }
                    break;
            }
        };

        int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData)  {
            _state      = stop;
            _gif.close();
            _fileName   = str;  
            _brightness = L_BYTE(p1);
            _repeat     = H_WORD(p1);
            int res = _checkFile();
            if (res == ANI_OK){
                _state  = init;
            } 
            return res;
        };

    private:
        enum GifState {stop,init,run};
        volatile GifState   _state;

        AnimatedGIF         _gif; // static instance of the class uses 22.5K of RAM
        uint32_t               _lastFrame;
        int                 _wait,_repeat,_count;                
        uint8_t                _brightness;
        String              _fileName;


        int _checkFile(){
            #ifdef WITH_SD_CARD
                SDFile file;
                globalSPI0_mutex.lock();
                if (globalSDcard0.exists(_fileName.c_str())){
                    file = globalSDcard0.open(_fileName.c_str(),FILE_READ);
                    if (file.isDirectory()) {
                        globalSPI0_mutex.unlock();
                        return ANI_ERROR_FILE_NOT_FOUND;
                    }
                    // was just fpr test _gif will open file later
                    file.close();      
                    globalSPI0_mutex.unlock();
                    return ANI_OK;
                } 
                globalSPI0_mutex.unlock();
            #endif
            return ANI_ERROR_FILE_NOT_FOUND;
        }



        static void * _GIFOpenFile(const char *fname, int32_t *pSize)
        {
            #ifdef WITH_SD_CARD
                SDFile * pFile = new SDFile();
                globalSPI0_mutex.lock();
                *pFile = globalSDcard0.open(fname,FILE_READ);

                if (*pFile)
                {
                    *pSize = pFile->size();
                    globalSPI0_mutex.unlock();
                    return (void*)pFile;
                }
                globalSPI0_mutex.unlock();
            #endif
            return NULL;
        } /* GIFOpenFile() */

        static void _GIFCloseFile(void *pHandle)
        {
            SDFile *pFile = static_cast<SDFile *>(pHandle);
            if (pFile != NULL){
                globalSPI0_mutex.lock();
                pFile->close();
                delete pFile;
                globalSPI0_mutex.free();
            }
                
        } /* GIFCloseFile() */

        static int32_t _GIFReadFile(GIFFILE *pGifFile, uint8_t *pBuf, int32_t iLen)
        {
            int32_t iBytesRead;
            iBytesRead = iLen;
            SDFile *pFile = static_cast<SDFile *>(pGifFile->fHandle);
            // Note: If you read a file all the way to the last byte, seek() stops working
            if ((pGifFile->iSize - pGifFile->iPos) < iLen)
                iBytesRead = pGifFile->iSize - pGifFile->iPos - 1; // <-- ugly work-around
            if (iBytesRead <= 0)
                return 0;
            globalSPI0_mutex.lock();
                iBytesRead = (int32_t)pFile->read(pBuf, iBytesRead);
                pGifFile->iPos = pFile->position();
            globalSPI0_mutex.free();
            return iBytesRead;
        } /* GIFReadFile() */

        static int32_t _GIFSeekFile(GIFFILE *pGifFile, int32_t iPosition)
        { 
            SDFile *pFile = static_cast<SDFile *>(pGifFile->fHandle);
            globalSPI0_mutex.lock();
                pFile->seek(iPosition);
                pGifFile->iPos = (int32_t)pFile->position();
            globalSPI0_mutex.free();
           return pGifFile->iPos;
        } /* GIFSeekFile() */

        static void _GIFDraw(GIFDRAW *pDraw)
        {
            // GIF decoder callback function
            // called once per line as the image is decoded
            uint8_t r, g, b, *s, *p, *pPal = (uint8_t *)pDraw->pPalette;
            int x, y = pDraw->iY + pDraw->y;
            Adafruit_NeoMatrix * pMatrix = (Adafruit_NeoMatrix *)pDraw->pUser;
            
            s = pDraw->pPixels;
            if (pDraw->ucDisposalMethod == 2) {
                // restore to background color
                p = &pPal[pDraw->ucBackground * 3];
                r = p[0]; g = p[1]; b = p[2];
                for (x=0; x<pDraw->iWidth; x++) {
                    if (s[x] == pDraw->ucTransparent) {
                        pMatrix->drawPixel(x,y,toColor565(r,g,b));
                    }
                }
                pDraw->ucHasTransparency = 0;
            }
            // Apply the new pixels to the main image
            if (pDraw->ucHasTransparency) {
                // if transparency used
                const uint8_t ucTransparent = pDraw->ucTransparent;
                for (x=0; x<pDraw->iWidth; x++)  {
                    if (s[x] != ucTransparent) {
                        p = &pPal[s[x] * 3];
                        pMatrix->drawPixel(x,y,toColor565(p[0], p[1], p[2]));
                    }
                }
            } else  {
                // no transparency, just copy them all
                for (x=0; x<pDraw->iWidth; x++)  {
                    p = &pPal[s[x] * 3];
                    pMatrix->drawPixel(x,y,toColor565(p[0], p[1], p[2]));
                }
            }
            if (pDraw->y == pDraw->iHeight-1) {
                // last line has been decoded, display the image
                pMatrix->show();
            }
        } /* GIFDraw() */
};

