
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
    for the gif ani module only diming is been used in same way like for the orther ani modules
    the color is been only used for background/clear purpose 

    nevertheless the timer of all config functionalities run independenly from the GIF frame animation

    so you could combine a breath dim effect and overlay this to any gif animation 

    
	dimCfg :         	xx xx xx xx   :
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

						dimCfg: 0x8000 0030   set to dim level 0x30
						dimCfg: 0x8000 1000 inc by 10 ==> results in 0x30 + 0x10 = 0x40 dim level 

	colorCfg :          xx xx xx xx   :
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
						colorCfg:0  & str:"0x00FF FFFF"  ==> constant white
						colorCfg:0  & str:"0x0000 00FF"  ==> constant blue
						colorCfg:0xFFFF0100 & str:"0x00FF 0000,0x0000 00FF"   switch between red and blue on trigger event  start with red
					

	flashCfg :          xx xx xx xx   :
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


	breathCfg :         xx xx xx xx   :
						|| || || ++---: delta dim increase (clipped at 255) for high level (dimCfg = low level) 
						|| || ||      : 0: switch breath module off
						|| || ++------: time t1 in 100ms: time for dim up
						|| ++---------: time t2 in 100ms: time for dim down
						++------------: reserved

	str: string parameter : color list ~&~ file list
					example: "0x0000 0000 ~&~ 3*rainbow.gif,2*alien.gif"
                    default: "0x0000 0000 ~&~ 0*start.gif"      0 ==> endless
*/
 


class MatrixGifFileAni : public NeoMatrixAni{
    struct AniGifNode{
        int32_t count;
        String  name;
    };

    public:
        MatrixGifFileAni():
            NeoMatrixAni("gif"),
            _state(stop),_lastFrame(0),_wait(0),
            _loopCount(0),_loopIndex(0),_fileCount(0),_fileIndex(0) 
        {
            _gif.begin(GIF_PALETTE_RGB888 );
        }
        
        void reset() { 
            config(AniCfg( ANI_WR_ALL | 0x40,0,0,0,"0x0000 0000 ~&~ 0*GIF/START.GIF" )); 
        }

        int  config(AniCfg cfg){
            StringList *    pList;
            StringList *    pEntryList;
            String          entryStr,countStr,nameStr;
            AniGifNode      entry;
            int             res;
            _state      = stop;

            pList = new StringList(cfg.str.c_str(),"~&~");
            String colorList = pList->getNextListEntry();
            String fileList  = pList->getNextListEntry();
            delete pList;

            // do base class config 
            cfg.str = colorList;
            Ani::config(cfg);       

            // do individual config (file list)
    	    _fileList.clear();
            pList = new StringList(fileList.c_str(),',');
            while(pList->isEndReached() == false){
                entryStr = pList->getNextListEntry();

                pEntryList = new StringList(entryStr.c_str(),"*");
                countStr = pEntryList->getNextListEntry();
                nameStr  = pEntryList->getNextListEntry();
                delete pEntryList;

                entry.count = convertStrToInt(countStr);
                entry.name  = nameStr;
                res = _checkFile(entry.name);
                if (res != ANI_OK){
                    LOG("file not found: " + entry.name);
                } else {
                    LOG("file: " + entry.name + " add to list and will be played " + String(entry.count) + " times");
                    _fileList.add(entry);
                }
            }
            _fileCount = _fileList.size();
            if (_fileCount == 0){
                LOG(" no file in list found");
                return ANI_ERROR_FILE_NOT_FOUND;
            }
            LOG("GIF list contains now "+String(_fileCount)+ " entrie(s)");
            _state  = init;
            return ANI_OK;
        }

        void loop(uint32_t time,Adafruit_NeoMatrix * pMatrix){
            AniGifNode entry;
            u16_t color;
            u8_t dim;
            int nr;

            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    Ani::loop(time);
                    pMatrix->fillScreen(toColor565(getColor()));
                    pMatrix->setBrightness(getDim());

                    _fileIndex = 0;
                    _loopIndex = 0;

                    entry = _fileList.get(_fileIndex);
                    _loopCount = entry.count;
                    _gif.close();
                    _gif.open((const char *)entry.name.c_str(),_GIFOpenFile, _GIFCloseFile, _GIFReadFile, _GIFSeekFile, _GIFDraw);

                    _lastFrame = time;
                    _wait = 1;

                    _state = run;
                    break;
                
                case run:
                    Ani::loop(time);
                    if (time-_lastFrame >= _wait){
                        _lastFrame = time;
                        if (hasChanged()){
                            pMatrix->setBrightness(getDim());
                        }
                        int res = _gif.playFrame(false,&_wait,pMatrix);
                        if(res == 0){
                            // last gif frame has been played 
                            if (_loopCount > 0){
                                // it is not and endless loop ==> select next one (same frame or next file)
                                _loopIndex++;
                                if (_loopIndex >= _loopCount){
                                    _loopIndex = 0;
                                    _fileIndex = wrapAround(0,_fileIndex+1,_fileCount-1);
                                }
                            }
                            entry = _fileList.get(_fileIndex);
                            _loopCount = entry.count;
                            //_gif.reset();  // does not work for me ???
                            _gif.close();
                            _gif.open((const char *)entry.name.c_str(),_GIFOpenFile, _GIFCloseFile, _GIFReadFile, _GIFSeekFile, _GIFDraw);
                            pMatrix->fillScreen(toColor565(getColor()));
                        } else if (res == -1){
                            _frameError();
                        }
                    }
                    break;
            }
        }

    protected:
        enum GifState {stop,init,run};
        volatile GifState   _state;

        AnimatedGIF         _gif; // static instance of the class uses 22.5K of RAM
        uint32_t            _lastFrame;
        int                 _wait,_loopCount,_loopIndex,_fileCount,_fileIndex;                

        SimpleList<AniGifNode>  _fileList;


        int _checkFile(String fileName){
            SDFile file;
            if (globalSDcard0.exists(fileName.c_str())){
                file = globalSDcard0.open(fileName.c_str(),FILE_READ);
                if (file.isDirectory()) {
                    return ANI_ERROR_FILE_NOT_FOUND;
                }
                // was just test if file exits .. file open will be done later by _gif object
                file.close();      
                return ANI_OK;
            } 
            return ANI_ERROR_FILE_NOT_FOUND;
        }

        void _frameError(){
            String errorMsg=" error in _gif.playFrame: ";
            switch(_gif.getLastError()){
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

