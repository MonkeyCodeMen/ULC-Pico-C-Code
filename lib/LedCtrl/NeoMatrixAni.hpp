#pragma once
#include "Ani.hpp"
#include <Adafruit_NeoMatrix.h>
#include "helper.hpp"
#include "Debug.hpp"


class NeoMatrixAni:public Ani
{
	public:
		NeoMatrixAni(const char * pName) : Ani(pName) {};
		~NeoMatrixAni() = default;
		
		virtual void loop(u32_t time,Adafruit_NeoMatrix * pMatrix) {};
        virtual void reset()                            {};
        virtual void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData) {};

};

class MatrixOffAni : public NeoMatrixAni{
    public:
        MatrixOffAni():NeoMatrixAni((const char *)F("off"))      {};
        void reset() {_color = 0; _needUpdate = true;};
        void loop(u32_t time,Adafruit_NeoMatrix * pMatrix) {
            if (_needUpdate == true){
                pMatrix->fillScreen(_color);
                pMatrix->show();
                _needUpdate = false;
            }
        };
    private:
        bool    _needUpdate;
        u16_t   _color;

};

class MatrixStaticAni : public NeoMatrixAni{
     /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  static
        -------+---------------+---------------------------
        p1:    | 0x5000 00FF   |  0xDDRR GGBB  
               |               |  D: dim value
               |               |  R: red value
               |               |  G: green value
               |               |  B: blue value
        -------+---------------+---------------------------
        p2:    | N/A           |  N/A     
        -------+---------------+---------------------------
        p3:    | N/A           |  N/A
        -------+---------------+---------------------------
        p4:    |               |  N/A
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */
   public:
        MatrixStaticAni():NeoMatrixAni((const char *)F("static"))      {};
        void reset() {  setup(0x500000FF,0,0,0,0,NULL); };

        void loop(u32_t time,Adafruit_NeoMatrix * pMatrix) {
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    u16_t color565;
                    color565 = toColor565(dimRgb24ToRgb( _colorWithDim));
                    pMatrix->fillScreen(color565);
                    pMatrix->show();
                    _state = run;
                    break;
                
                case run:
                    // do nothing .. it's static
                    break;
            }
        };

        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData) {
            _state = stop;
            _colorWithDim = p1;
            _state = init;
        };
    private:
        enum StaticState {stop,init,run};
        volatile StaticState _state;
        u32_t   _colorWithDim;
        u8_t    _dim;

};

class MatrixBreathAni : public NeoMatrixAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  breath
        -------+---------------+---------------------------
        p1:    | 0x0000 00FF   |  0x00RR GGBB  
               |               |  color red,green,blues
        -------+---------------+---------------------------
        p2:    | 0x0000 0A50   |  0x0000 LLHH     
               |               |  L:dim low level   
               |               |  H:dim high level
        -------+---------------+---------------------------
        p3:    | 0x0BBB 0CCC   |  0xIIII DDDD
               |               |  I:inc time
               |               |  D:dec time
        -------+---------------+---------------------------
        p4:    |               |  N/A
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */
    public:
        MatrixBreathAni():NeoMatrixAni((const char *)F("breath"))      {};
        void reset() { setup(0x000000FF,0x00000A50,0x0BBB0CCC,0,0,NULL); };

        void loop(u32_t time,Adafruit_NeoMatrix * pMatrix){
            u32_t diff,color;
            u32_t color24;
            u16_t color565;
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _lastSwitchTime = time;
                    _dimDiff = _dimMax-_dimMin;
                    color24 = dimColor255(_color24,_dimMin);
                    color565 = toColor565(color24);
                    pMatrix->fillScreen(color565);
                    pMatrix->show();
                    _state = inc;
                    break;
                
                case inc:
                    diff = time - _lastSwitchTime;
                    if (diff >= _incTime){
                        color24 = dimColor255(_color24,_dimMax);
                        color565 = toColor565(color24);
                        pMatrix->fillScreen(color565);
                        pMatrix->show();
                        _lastSwitchTime = time;
                        _state = dec;
                    } else {
                        color24 = dimColor255(_color24,_dimMin+((_dimDiff*diff)/_incTime));
                        color565 = toColor565(color24);
                        pMatrix->fillScreen(color565);
                        pMatrix->show();
                    }
                    break;

                case dec:
                    diff = time - _lastSwitchTime;
                    if (diff >= _decTime){
                        color24 = dimColor255(_color24,_dimMin);
                        color565 = toColor565(color24);
                        pMatrix->fillScreen(color565);
                        pMatrix->show();
                        _lastSwitchTime = time;
                        _state = inc;
                    } else {
                        color24 = dimColor255(_color24,_dimMax-((_dimDiff*diff)/_decTime));
                        color565 = toColor565(color24);
                        pMatrix->fillScreen(color565);
                        pMatrix->show();
                    }
                    break;
            }
        };

        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData)  {
            _state      = stop;
            _color24    = p1 & 0x00FFFFFF;
            _dimMin     = H_BYTE(p2);
            _dimMax     = L_BYTE(p2);
            _incTime    = H_WORD(p3);
            _decTime    = L_WORD(p3);
            _state      = init;
        };

    private:
        enum BreathState {stop,init,inc,dec};
        volatile BreathState _state;
        u32_t _lastSwitchTime;
        u8_t _dimMin,_dimMax,_dimDiff;
        u16_t _incTime,_decTime;
        u32_t _color24;

};

class MatrixRainbowFlashAni : public NeoMatrixAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  rainbow flash
        -------+---------------+---------------------------
        p1:    | 0x0019 004A   |  0xIIII DDDD
               |               |  I:on time
               |               |  D:off time
        -------+---------------+---------------------------
        p2:    | 0x0008 0800   |  0xCCCC PPPP     
               |               |  C:flash count per sequence   
               |               |  P:pause time   
        -------+---------------+---------------------------
        p3:    | 0x0000 5008   |  0x0000 DDII
               |               |  I:inc step for color wheel (full wheel 255)
               |               |  D:dim value for color
        -------+---------------+---------------------------
        p4:    | N/A           |  N/A
        -------+---------------+---------------------------
        pData: | N/A           |  N/A 
    */
    #define COLOR_LIST_LENGTH 16
    public:
        MatrixRainbowFlashAni():NeoMatrixAni((const char *)F("rainbow flash"))      {};

        void reset() { setup(0x0019004A,0x00080800,0x00005008,0,0,NULL); };
       
        virtual void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData)  {
            _state      = stop;
            _onTime     = H_WORD(p1);
            _offTime    = L_WORD(p1);
            _flashCount = H_WORD(p2);
            _pauseTime  = L_WORD(p2);
            _incStep    = L_BYTE(p3);
            _dim        = H_BYTE(p3);
            _state      = init;
        };

        void loop(u32_t time,Adafruit_NeoMatrix * pMatrix){
            u32_t diff;
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _state = flashOn;
                    _lastCallTime = time;
                    _colorIndex=0;
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
        virtual void   _setNextColor(Adafruit_NeoMatrix * pMatrix){
            u16_t color565;
            u32_t color24;
            u8_t  dim;
            dim   = _dim;
            color24 = getColorWheel24Bit(_colorIndex);
            color24 = dimColor255(color24,dim);
            color565 = toColor565(color24);
            _colorIndex += _incStep;
            pMatrix->fillScreen(color565);
            pMatrix->show();
        };

        void _setMatrixOff(Adafruit_NeoMatrix * pMatrix){
            pMatrix->fillScreen(0);
            pMatrix->show();
        }

        enum MultiFlashState {stop,init,flashOn,flashOff,pause};
        volatile MultiFlashState _state;
        u32_t _lastCallTime;
        u8_t  _incStep;
        u16_t _onTime,_offTime;
        u16_t _pauseTime;
        u16_t _flashCount,_count;
        u8_t  _colorIndex,_dim;

};

class MatrixMultiFlashAni : public MatrixRainbowFlashAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  multi flash
        -------+---------------+---------------------------
        p1:    | 0x0019 004A   |  0xIIII DDDD
               |               |  I:on time
               |               |  D:off time
        -------+---------------+---------------------------
        p2:    | 0x0008 0800   |  0xCCCC PPPP     
               |               |  C:flash count per sequence   
               |               |  P:pause time   
        -------+---------------+---------------------------
        p3:    | 0x0000 00001  |  0x0000 00II
               |               |  I:inc step for color table
        -------+---------------+---------------------------
        p4:    | N/A           |  N/A
        -------+---------------+---------------------------
        pData: | COLOR_DEF_LIST|  length(0): ASCII  List 
               |               |  0xDDRRGGBB,0xDDRRGGBB,...  (max COLOR_LIST_LENGTH)

    */
    #define COLOR_LIST_LENGTH 16
    #define COLOR_DEF_LIST F("0xFFFFFFFF,0xFF0000FF,0xFF00FF00,0xFFFF0000")
    public:
        MatrixMultiFlashAni()      {
            MatrixRainbowFlashAni();
            NeoMatrixAni((const char *)F("multi flash"));
            for(int i=0;i < COLOR_LIST_LENGTH;i++){
                _colorList[i]=0;
            }
        };

        void reset() { setup(0x0019004A,0x00080800,1,0,0,(u8_t*)COLOR_DEF_LIST); };      
       
        void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData)  {
            MatrixRainbowFlashAni::setup(p1,p2,p3,p4,length,pData);
            _state      = stop;
            if (pData != 0){
                if (length==0){
                    _decodeAsciiColorList((const char *) pData);
                }else{
                    _decodeBinColorList(pData,length);
                }
            }
            _state      = init;
        };

    private:
        void   _setNextColor(Adafruit_NeoMatrix * pMatrix){
            u16_t color565;
            u32_t color24;
            u8_t  dim;
            color24 = dimRgb24ToRgb(_colorList[_colorIndex]);
            color565 = toColor565(color24);
            _colorIndex += _incStep;
            _colorIndex %= _colorCount;
            pMatrix->fillScreen(color565);
            pMatrix->show();
        };

        void _decodeAsciiColorList(const char * listText){
            StringList list(listText,',');
            _colorCount = 0;
            for(int i=0;i < COLOR_LIST_LENGTH;i++){
                _colorList[i]=0;
            }
            while(list.isEndReached() == false){
                _colorList[_colorCount] = convertStrToInt(list.getNextListEntry());
                _colorCount++;
            }
        };

        void _decodeBinColorList(u8_t * pData,u32_t length){
            _colorCount = 0;
            for(int i=0;i < COLOR_LIST_LENGTH;i++){
                _colorList[i]=0;
            }
            if (length % 4 != 0){
                LOG(F("MatrixMultiFlashAni::_decodeBinColorList  invalid length (%4 = 0!!) list skipped"));
                return;
            }

            _colorCount = length/4;
            if (_colorCount > COLOR_LIST_LENGTH){
                _colorCount = COLOR_LIST_LENGTH;
                LOG(F("MatrixMultiFlashAni::_decodeBinColorList  list to long, list truncated"));
            }

            u32_t * p=(u32_t*)pData;
            for(int i=0;i < _colorCount;i++){
                _colorList[i] = *p;
                p++;
            }
        };

        u16_t _colorList[COLOR_LIST_LENGTH];
        u8_t  _colorIndex,_colorCount;

};
  
class MatrixRunningRectAni : public NeoMatrixAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  running rect
        -------+---------------+---------------------------
        p1:    | 0x01F4 5008   |  0xSSSS DDII
               |               |  S:step time in ms
               |               |  I:inc step for color wheel (full wheel 255)
               |               |  D:dim value for color
        -------+---------------+---------------------------
        p2:    | 0x0000 0001   |  0x0000 00TT
               |               |  T: rect border width
        -------+---------------+---------------------------
        p3:    | N/A           |  N/A
        -------+---------------+---------------------------
        p4:    | N/A           |  N/A
        -------+---------------+---------------------------
        pData: | N/A           |  N/A 

        currently only with rainbow colors
        ToDo: color from color list 
    */
    #define COLOR_LIST_LENGTH 16
    public:
        MatrixRunningRectAni():NeoMatrixAni((const char *)F("running rect"))      {};

        void reset() { setup(0x01005040,0,0,0,0,NULL); };
       
        virtual void setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,u32_t length,u8_t * pData)  {
            _state      = stop;
            _incTime    = H_WORD(p1);
            _incStep    = L_BYTE(p1);
            _dim        = H_BYTE(p1);
            _border     = L_BYTE(p2);
            LOG(F_CONST("RGB matrix running rect selected"));
            _state      = init;
        };

        void loop(u32_t time,Adafruit_NeoMatrix * pMatrix){
            u32_t diff;
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    _lastCallTime = time;
                    _colorIndex = 0;
                    _sizeX = pMatrix->width();
                    _sizeY = pMatrix->height();
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
        virtual void   _setNextColor(Adafruit_NeoMatrix * pMatrix){
            u16_t color565;
            u32_t color24;
            u8_t  borderLine = 0;
            u8_t  colorIndex = _colorIndex;
            u16_t steps,x,y;
            s32_t h,w;
            

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
                    color24 = getColorWheel24Bit(colorIndex);
                    color24 = dimColor255(color24,_dim);
                    color565 = toColor565(color24);
                    
                    // draw 4 lines (rect)
                    pMatrix->drawRect(x,y,w,h,color565 );

                    // change color or repeat color 
                    borderLine++;
                    if(borderLine >= _border){
                        colorIndex += _incStep;
                        borderLine = 0;
                    }
                }
            }
            pMatrix->show();

            // next time start one color step later 
            _colorIndex += _incStep;
        };

        enum RunningRectState {stop,init,run};
        volatile RunningRectState _state;
        u32_t _lastCallTime;
        u16_t _incTime;
        u8_t  _dim,_incStep,_colorIndex,_border;
        u32_t _sizeX,_sizeY;
        s32_t _startX,_startY;

};
 
class MatrixRunningCircleAni : public MatrixRunningRectAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  running circle
        -------+---------------+---------------------------
        p1:    | 0x000A 5008   |  0xSSSS DDII
               |               |  S:step time in ms
               |               |  I:inc step for color wheel (full wheel 255)
               |               |  D:dim value for color
        -------+---------------+---------------------------
        p2:    | 0x0000 0001   |  0x0000 00TT
               |               |  T: circle border width
        -------+---------------+---------------------------
        p3:    | N/A           |  N/A
        -------+---------------+---------------------------
        p4:    | N/A           |  N/A
        -------+---------------+---------------------------
        pData: | N/A           |  N/A 

        currently only with rainbow colors
        ToDo: color from color list 
    */
    #define COLOR_LIST_LENGTH 16
    public:
        MatrixRunningCircleAni() {
            MatrixRunningRectAni();
            NeoMatrixAni((const char *)F("running circle"));
        };


 
    protected:
        virtual void   _setNextColor(Adafruit_NeoMatrix * pMatrix){
            u16_t color565;
            u32_t color24;
            u8_t  borderLine = 0;
            u8_t  colorIndex = _colorIndex;
            u16_t steps,x,y;
            s32_t h,w;
            

            steps = _sizeX/2;
            if (_sizeY/2 < steps){
                steps = _sizeY/2;
            }

            for(int i=0; i < steps; i++){
                if ((h>0) && (w>0)){
                    // get color from color wheel
                    color24 = getColorWheel24Bit(colorIndex);
                    color24 = dimColor255(color24,_dim);
                    color565 = toColor565(color24);
                    
                    // rect with rounded corner
                    pMatrix->drawRect(i,i,w,h,color565 );

                    // change color or repeat color 
                    borderLine++;
                    if(borderLine >= _border){
                        colorIndex += _incStep;
                        borderLine = 0;
                    }
                }
            }
            pMatrix->show();

            // next time start one color step later 
            _colorIndex += _incStep;
        };


};  
  
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  breath
        -------+---------------+---------------------------
        p1:    |               |  N/A
        -------+---------------+---------------------------
        p2:    | 0x0000 00FF   |  0x00RR GGBB  
               |               |  color red,green,blues
        -------+---------------+---------------------------
        p3:    | 0x0000 0A50   |  0x0000 LLHH     
               |               |  L:dim low level   
               |               |  H:dim high level
        -------+---------------+---------------------------
        p4:    | 0x0BBB 0CCC   |  0xIIII DDDD
               |               |  I:inc time
               |               |  D:dec time
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */
