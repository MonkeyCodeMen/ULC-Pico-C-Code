
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


#include <Display.hpp>
#include <Debug.hpp>
#include <PinMapping.h>

Display display;



void Display::begin(){
    init();
    setRotation(3);
    fillScreen(TFT_BLACK);
    pinMode(PIN_TFT_LED, OUTPUT);
    analogWrite(PIN_TFT_LED,TFT_DIM);

    #ifdef WITH_DSIPLAY_STARTUP_TEST
        setup_t cfg;
        getSetup(cfg);
        String out="";
        out+="\nTFT_eSPI version:  "+cfg.version;
        out+="\nTFT_eSPI setup:    "+cfg.setup_info;
        out+="\nTFT_eSPI id:       "+String(cfg.setup_id);
        out+="\nTFT_eSPI pin_mosi: "+String(cfg.pin_tft_mosi);
        out+="\nTFT_eSPI pin_miso: "+String(cfg.pin_tft_miso);
        out+="\nTFT_eSPI pin_clk:  "+String(cfg.pin_tft_clk);
        out+="\nTFT_eSPI pin_cs:   "+String(cfg.pin_tft_cs);
        out+="\nTFT_eSPI pin_dc:   "+String(cfg.pin_tft_dc);
        out+="\nTFT_eSPI spi port: "+String(cfg.port);
        out+="\nTFT_eSPI f spi:    "+String(cfg.tft_spi_freq);
        out+="\nTFT_eSPI f rd:     "+String(cfg.tft_rd_freq);
        out+="\nTFT_eSPI f touch:  "+String(cfg.tch_spi_freq);
        out+="\nTFT_eSPI width:    "+String(cfg.tft_width);
        out+="\nTFT_eSPI height:   "+String(cfg.tft_height);
        LOG(out.c_str());
        fillScreen(TFT_RED);
        delay(200);
        fillScreen(TFT_GREEN);
        delay(200);
        fillScreen(TFT_BLUE);
        delay(200);
        fillScreen(TFT_BLACK);
    #endif
}

