#pragma once
#include <NeoMatrixAni.hpp>

 
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
        MatrixGifFileAni():NeoMatrixAni("gif")      {
            _gif.begin(GIF_PALETTE_RGB888 );
        };
        
        void reset() { config(ANI_WR_ALL | 0x40,0,0,0,"START.GIF"); }

        void loop(uint32_t time,Adafruit_NeoMatrix * pMatrix){
            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    pMatrix->fillScreen(0);
                    pMatrix->setBrightness(_brightness);
                    _gif.close();
                    _gif.open((const char *)_fileName.c_str(),_GIFOpenFile, _GIFCloseFile, _GIFReadFile, _GIFSeekFile, _GIFDraw);
                    _count = 0;
                    _state = run;
                    break;
                
                case run:
                    if (time-_lastFrame >= _wait){
                        _lastFrame = time;
                        int res = _gif.playFrame(false,&_wait,pMatrix);
                        if(res == 0){
                            _gif.close();
                            if (_repeat > 0){
                                _count++;
                                if (_count >= _repeat){
                                    _state = stop;
                                    break;
                                }
                            }
                            //_gif.reset();  does not work for me ???
                            _gif.open((const char *)_fileName.c_str(),_GIFOpenFile, _GIFCloseFile, _GIFReadFile, _GIFSeekFile, _GIFDraw);
                        } else if (res == -1){
                            String errorMsg=" error in _gif.playFrame: ";
                            int nr = _gif.getLastError();
                            switch(nr){
                                case GIF_SUCCESS:               errorMsg+="GIF_SUCCESS";            break;
                                case GIF_DECODE_ERROR:          errorMsg+="GIF_DECODE_ERROR";       break;
                                case GIF_TOO_WIDE:              errorMsg+="GIF_TOO_WIDE";           break;
                                case GIF_INVALID_PARAMETER:     errorMsg+="GIF_INVALID_PARAMETER";  break;
                                case GIF_UNSUPPORTED_FEATURE:   errorMsg+="GIF_UNSUPPORTED_FEATURE";break;
                                case GIF_FILE_NOT_OPEN:         errorMsg+="GIF_FILE_NOT_OPEN";      break;
                                case GIF_EARLY_EOF:             errorMsg+="GIF_EARLY_EOF";          break;
                                case GIF_EMPTY_FRAME:           errorMsg+="GIF_EMPTY_FRAME";        break;
                                case GIF_BAD_FILE:              errorMsg+="GIF_BAD_FILE";           break;
                                case GIF_ERROR_MEMORY:          errorMsg+="GIF_ERROR_MEMORY";       break;
                            }
                            debug.log(errorMsg);
                        }
                    }
                    break;
            }
        }

        int setup(uint32_t p1,uint32_t p2,uint32_t p3,uint32_t p4,String str,uint32_t length,uint8_t ** pData)  {
            _state      = stop;
            //_gif.close(); do _gif handling only in main loop (reentrance)
            _fileName   = str;  
            _brightness = L_BYTE(p1);
            _repeat     = H_WORD(p1);
            int res = _checkFile();
            if (res == ANI_OK){
                _state  = init;
            } 
            return res;
        }

    private:
        enum GifState {stop,init,run};
        volatile GifState   _state;

        AnimatedGIF         _gif; // static instance of the class uses 22.5K of RAM
        uint32_t            _lastFrame;
        int                 _wait,_repeat,_count;                
        uint8_t             _brightness;
        String              _fileName;


        int _checkFile(){
            SDFile file;
            if (globalSDcard0.exists(_fileName.c_str())){
                file = globalSDcard0.open(_fileName.c_str(),FILE_READ);
                if (file.isDirectory()) {
                    return ANI_ERROR_FILE_NOT_FOUND;
                }
                // was just test if file exits .. file open will be done later by _gif object
                file.close();      
                return ANI_OK;
            } 
            return ANI_ERROR_FILE_NOT_FOUND;
        }



        static void * _GIFOpenFile(const char *fname, int32_t *pSize)
        {
            SDFile * pFile = new SDFile();
            *pFile = globalSDcard0.open(fname,FILE_READ);

            if (*pFile)
            {
                *pSize = pFile->size();
                return (void*)pFile;
            }
            return NULL;
        } 

        static void _GIFCloseFile(void *pHandle)
        {
            SDFile *pFile = static_cast<SDFile *>(pHandle);
            if (pFile != NULL){
                pFile->close();
                delete pFile;
            }
                
        } 

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
            iBytesRead = (int32_t)pFile->read(pBuf, iBytesRead);
            pGifFile->iPos = pFile->position();
            return iBytesRead;
        } 

        static int32_t _GIFSeekFile(GIFFILE *pGifFile, int32_t iPosition)
        { 
            SDFile *pFile = static_cast<SDFile *>(pGifFile->fHandle);
            pFile->seek(iPosition);
            pGifFile->iPos = (int32_t)pFile->position();
           return pGifFile->iPos;
        } 

        static void _GIFDraw(GIFDRAW *pDraw)
        {
            // GIF decoder callback function
            // called once per line as the image is decoded
            // mainly copyed from LIB example like the other file acccess methods too
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
        } 
};

