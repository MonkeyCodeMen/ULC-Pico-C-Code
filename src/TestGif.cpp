#include <Arduino.h>
#include <MainConfig.h>
#include <PinMapping.h>
#include <SPI.h>

#include <Debug.hpp>
#include <helper.hpp>

#include <globalObjects.hpp>
#include <LoopStats.hpp>

#include <AnimatedGIF.h>
#include <SD.h>




// Normal color values are too bright on the LED array, so reduce the values
#define BRIGHT_SHIFT 2

AnimatedGIF gif; // static instance of the class uses 22.5K of RAM




//
// GIF decoder callback function
// called once per line as the image is decoded
//
void GIFDraw(GIFDRAW *pDraw)
{
uint8_t r, g, b, *s, *p, *pPal = (uint8_t *)pDraw->pPalette;
int x, y = pDraw->iY + pDraw->y;

    
    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
      p = &pPal[pDraw->ucBackground * 3];
      r = p[0] >> BRIGHT_SHIFT; g = p[1] >> BRIGHT_SHIFT; b = p[2] >> BRIGHT_SHIFT;
      for (x=0; x<pDraw->iWidth; x++)
      {
        if (s[x] == pDraw->ucTransparent) {
           neoMatrix1.drawPixel(x,y,toColor565(r,g,b));
        }
      }
      pDraw->ucHasTransparency = 0;
    }
    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
      const uint8_t ucTransparent = pDraw->ucTransparent;
      for (x=0; x<pDraw->iWidth; x++)
      {
        if (s[x] != ucTransparent) {
           p = &pPal[s[x] * 3];
           neoMatrix1.drawPixel(x,y,toColor565(p[0]>>BRIGHT_SHIFT, p[1]>>BRIGHT_SHIFT, p[2]>>BRIGHT_SHIFT));
        }
      }
    }
    else // no transparency, just copy them all
    {
      for (x=0; x<pDraw->iWidth; x++)
      {
           p = &pPal[s[x] * 3];
           neoMatrix1.drawPixel(x,y,toColor565(p[0]>>BRIGHT_SHIFT, p[1]>>BRIGHT_SHIFT, p[2]>>BRIGHT_SHIFT));
      }
    }
    if (pDraw->y == pDraw->iHeight-1) {// last line has been decoded, display the image
        neoMatrix1.show();
    }
} /* GIFDraw() */

void gifSetup() {
  gif.begin(GIF_PALETTE_RGB888); // request 24-bit palette
} /* setup() */


void gifLoop() {
    #define MAX_GIF_COUNTER     13
    #define MAX_REPEAT          5
    static u8_t fileCounter     = 0;
    static bool loadNextFile    = true;
    static u8_t repeatCounter   = 0;
    String out;
    static String name;
    static GIFINFO gi;
    bool res;
    static u32_t lastFrame;
    u32_t now = millis();
    static int wait=0;
    static u8_t * pBuffer = NULL;
    static int size = 0;

    if (loadNextFile == true){
        bool res;
        name = "8X8-"+String(fileCounter+1)+".GIF";
        if (SD.exists(name.c_str())){
            if (pBuffer != NULL){
                delete pBuffer;
            }
            SDFile file = SD.open(name.c_str(),FILE_READ);
            if (file.isDirectory()) {
                out = "file is directory: ";
                out += name;
                LOG(out.c_str());
                STOP();
            }
            size = file.size();
            out = "before load file "+name;
            LOG_MEM(out.c_str());
            pBuffer = new u8_t[size];
            out = "after load file "+name;
            LOG_MEM(out.c_str());
            if (pBuffer == NULL){
                LOG(F_CONST("could not create GIF buffer"));
                STOP();
            }
            file.readBytes(pBuffer,size);
            file.close();
            loadNextFile = false;
            wait = 0;
            lastFrame = 0;
            res = gif.openFLASH(pBuffer,size,GIFDraw);
            if (res == false){
                LOG(F_CONST("could not load GIF from memory"));
                STOP();
            }
        } else {
            out = "file does not exist: ";
            out += name;
            LOG(out.c_str());
        }
    } else {
        if (now-lastFrame >= wait){
            lastFrame = now;
            res = gif.playFrame(false,&wait);
            if(res==false){
                repeatCounter++;
                if (repeatCounter > MAX_REPEAT){
                    gif.close();
                    fileCounter = (fileCounter + 1) % MAX_GIF_COUNTER;
                    repeatCounter = 0;
                    loadNextFile = true;
                } else {
                    gif.close();
                    gif.openFLASH(pBuffer,size,GIFDraw);
                }
                
            }
        }
    }
}