#pragma once
#include "Ani.hpp"
#include <Adafruit_NeoMatrix.h>
#include "helper.hpp"
#include "Debug.hpp"
#include <ColorSelector.hpp>

#include <AnimatedGIF.h>

#include <SD.h>
#include <../../include/PinMapping.h>
#include "Mutex.hpp"
extern Mutex SPI_mutex;



class NeoMatrixAni:public Ani
{
	public:
		NeoMatrixAni(const char * pName) : Ani(pName) {};
		~NeoMatrixAni() = default;
		
		virtual void loop(u32_t time,Adafruit_NeoMatrix * pMatrix) {};
        virtual void reset()                            {};
        virtual int setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,String str,u32_t length,u8_t ** pData) {return ANI_OK;};

};

class MatrixOffAni : public NeoMatrixAni{
    public:
        MatrixOffAni():NeoMatrixAni(F_CONST("off"))      {};
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
        MatrixStaticAni():NeoMatrixAni(F_CONST("static"))      {};
        void reset() {  setup(0x500000FF,0,0,0,"",0,NULL); };

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

        int setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,String str,u32_t length,u8_t ** pData) {
            _state = stop;
            _colorWithDim = p1;
            _state = init;
            return ANI_OK;
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
        p1:    | 0x0000 0A50   |  0x0000 LLHH     
               |               |  L:dim low level   
               |               |  H:dim high level
        -------+---------------+---------------------------
        p2:    | 0x0BBB 0CCC   |  0xIIII DDDD
               |               |  I:inc time
               |               |  D:dec time
        -------+---------------+---------------------------
        p3:    | 0x0000 0001   |  0x00SS 00II
               |               |  S:start index for color wheel/list
               |               |  I:inc step for color wheel/list (full wheel 255)
        -------+---------------+---------------------------
        p4:    |               |  N/A
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        

        breath blue only:
        setup(0x00000A50,0x0BBB0CCC,0,0,"0x000000FF",0,NULL)

        breath rainbow:
        setup(0x00000A50,0x0BBB0CCC,0,1,"",0,NULL)

        breath color list:
        setup(0x00000A50,0x0BBB0CCC,0,1,"0xFFFFFF,0x00FF0000,0x00FF00,0x0000FF",0,NULL)

    */
    public:
        MatrixBreathAni():NeoMatrixAni(F_CONST("breath"))      {};
        void reset() { setup(0x00000A50,0x0BBB0CCC,1,0,"",0,NULL); };

        int setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,String str,u32_t length,u8_t ** pData)  {
            _state      = stop;
            _dimMin     = H_BYTE(p1);
            _dimMax     = L_BYTE(p1);
            _incTime    = H_WORD(p2);
            _decTime    = L_WORD(p2);
            _colorGen.setup(255,HH_BYTE(p3),L_BYTE(p3),str,length,*pData);
            _state      = init;
            return ANI_OK;
        };


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
                    color24 = _colorGen.getNextColor(_dimMin);
                    color565 = toColor565(color24);
                    pMatrix->fillScreen(color565);
                    pMatrix->show();
                    _state = inc;
                    break;
                
                case inc:
                    diff = time - _lastSwitchTime;
                    if (diff >= _incTime){
                        color24 = _colorGen.getNextColor(_dimMax);
                        color565 = toColor565(color24);
                        pMatrix->fillScreen(color565);
                        pMatrix->show();
                        _lastSwitchTime = time;
                        _state = dec;
                    } else {
                        color24 = _colorGen.getNextColor(_dimMin+((_dimDiff*diff)/_incTime));
                        color565 = toColor565(color24);
                        pMatrix->fillScreen(color565);
                        pMatrix->show();
                    }
                    break;

                case dec:
                    diff = time - _lastSwitchTime;
                    if (diff >= _decTime){
                        color24 = _colorGen.getNextColor(_dimMin);
                        color565 = toColor565(color24);
                        pMatrix->fillScreen(color565);
                        pMatrix->show();
                        _lastSwitchTime = time;
                        _state = inc;
                    } else {
                        color24 = _colorGen.getNextColor(_dimMax-((_dimDiff*diff)/_decTime));
                        color565 = toColor565(color24);
                        pMatrix->fillScreen(color565);
                        pMatrix->show();
                    }
                    break;
            }
        };

    private:
        enum BreathState {stop,init,inc,dec};
        volatile BreathState _state;
        u32_t _lastSwitchTime;
        u8_t _dimMin,_dimMax,_dimDiff;
        u16_t _incTime,_decTime;
        ColorSelector _colorGen;

};

class MatrixMultiFlashAni : public NeoMatrixAni{

    //#define COLOR_DEF_LIST F_CONST("")
    #define COLOR_DEF_LIST F_CONST("0xFFFFFFFF,0xFF0000FF,0xFF00FF00,0xFFFF0000")
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
        p3:    | 0x0000 FF10   |  0x00SS DDII
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
        MatrixMultiFlashAni():NeoMatrixAni(F_CONST("multi flash"))      {};

        void reset() { setup(0x0019004A,0x00080800,0x0000FF10,0,COLOR_DEF_LIST,0,NULL); };
       
        virtual int setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,String str,u32_t length,u8_t ** pData)  {
            _state      = stop;
            _onTime     = H_WORD(p1);
            _offTime    = L_WORD(p1);
            _flashCount = H_WORD(p2);
            _pauseTime  = L_WORD(p2);
            _colorGen.setup(H_BYTE(p3),HH_BYTE(p3),L_BYTE(p3),str,length,*pData);
            _state      = init;
            return ANI_OK;
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
        u32_t _lastCallTime;
        u16_t _onTime,_offTime;
        u16_t _pauseTime;
        u16_t _flashCount,_count;
        ColorSelector _colorGen;


        void   _setNextColor(Adafruit_NeoMatrix * pMatrix){
            u16_t color565;
            color565 = toColor565(_colorGen.getNextColor());
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
        p3:    | 0x0000 500A   |  0x00SS DDII
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
        MatrixBoxAni(bool circle = false):NeoMatrixAni(circle ? F_CONST("circle"):F_CONST("rect") ){ 
            _circle = circle;
        };

        void reset() { setup(0x020,1,0x300A,0,"",0,NULL); };
       
        virtual int setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,String str,u32_t length,u8_t ** pData)  {
            _state      = stop;
            _incTime    = L_WORD(p1);
            _border     = L_BYTE(p2);
            _colorGen.setup(H_BYTE(p3),HH_BYTE(p3),L_BYTE(p3),str,length,*pData);
            _state      = init;
            return ANI_OK;
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
        u32_t _lastCallTime;
        u16_t _incTime;
        u8_t  _border,_dim,_inc,_start;
        u32_t _sizeX,_sizeY;
        s32_t _startX,_startY;
        bool _circle;
        ColorSelector _colorGen;

        void   _setNextColor(Adafruit_NeoMatrix * pMatrix){
            u16_t color565;
            u32_t color24;
            u8_t  borderLine = 0;
            u16_t steps,x,y;
            s32_t h,w;
            

            steps = _sizeX/2;
            if (_sizeY/2 < steps){
                steps = _sizeY/2;
            }

            _colorGen.restore();
            color24 = _colorGen.getNextColor();
            _colorGen.backup(); // start next time with this color 
            for(int i=0; i < steps; i++){
                x=0+i;
                y=0+i;
                h = _sizeY - (2*i);
                w = _sizeX - (2*i);

                if ((h>0) && (w>0)){
                    // get color from color wheel
                    color565 = toColor565(color24);
                    
                    if (_circle==true){
                        // rect with rounded corner
                        pMatrix->drawRoundRect(x,y,w,h,i,color565 );
                    }   else {
                        // draw 4 lines (rect)    
                        pMatrix->drawRect(x,y,w,h,color565 );
                    }   

                    // change color or repeat color 
                    borderLine++;
                    if(borderLine >= _border){
                        color24 = _colorGen.getNextColor();
                        borderLine = 0;
                    }
                }
            }
            pMatrix->show();
        };
};
 




class MatrixGifFileAni : public NeoMatrixAni{
    /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  gif file
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
        MatrixGifFileAni():NeoMatrixAni(F_CONST("gif"))      {_gif.begin(GIF_PALETTE_RGB888);_pBuffer = NULL;};
        void reset() { setup(0x80,0,0,0,"TEST.GIF",0,NULL); };

        void loop(u32_t time,Adafruit_NeoMatrix * pMatrix){
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    pMatrix->fillScreen(0);
                    pMatrix->setBrightness(_brightness);
                    _gif.open(_pBuffer,_size,_GIFDraw);
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
                                    break;
                                }
                            }
                            _gif.open(_pBuffer,_size,_GIFDraw);
                        }
                    }
                    break;
            }
        };

        int setup(u32_t p1,u32_t p2,u32_t p3,u32_t p4,String str,u32_t length,u8_t ** pData)  {
            _state      = stop;
            _gif.close();
            if (_pBuffer != NULL) {
                delete _pBuffer;
                _pBuffer = NULL;
            }
            _fileName   = str;  
            _brightness = L_BYTE(p1);
            _repeat     = H_WORD(p1);
            if (length != 0){
                _pBuffer = *pData; 
                // from now on this object is owner of the buffer
                // and take care about free the memory if no longer used
                *pData = NULL;
                _state  = init;
                return ANI_OK;
            } 
            int res = _loadGifFromSD();
            if (res == ANI_OK){
                _state  = init;
            } 
            return res;
        };

    private:
        enum GifState {stop,init,run};
        volatile GifState   _state;

        AnimatedGIF         _gif; // static instance of the class uses 22.5K of RAM
        u8_t *              _pBuffer = NULL;
        int                 _size = 0;
        u32_t               _lastFrame;
        int                 _wait,_repeat,_count;                
        u8_t                _brightness;
        String              _fileName;

        int _loadGifFromSD(){
            SDClass card;
            if (_pBuffer != NULL) LOG(F_CONST("buffer should here be empty!!!"));
            SPI_mutex.lock();
            if (!card.begin(PIN_SPI0_CS_SD)) {
                LOG(F_CONST("SD card initialization failed!"));
                SPI_mutex.unlock();
                return ANI_ERROR_FILE_NOT_FOUND;
            }
            if (card.exists(_fileName.c_str())){
                SDFile file = card.open(_fileName.c_str(),FILE_READ);
                if (file.isDirectory()) {
                    SPI_mutex.unlock();
                    return ANI_ERROR_FILE_NOT_FOUND;
                }
                
                _size = file.size();
                _pBuffer = new u8_t[_size];
                if (_pBuffer == NULL)  {
                    SPI_mutex.unlock();
                    return ANI_ERROR_INTERNAL;
                } 

                file.readBytes(_pBuffer,_size);
                file.close();
                SPI_mutex.unlock();
                return ANI_OK;
            } 
            SPI_mutex.unlock();
            return ANI_ERROR_FILE_NOT_FOUND;
        }

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

