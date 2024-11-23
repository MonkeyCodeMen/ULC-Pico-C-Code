
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
#include <Arduino.h>
#include "StringList.hpp"

/*
    typesafe macros for max and min   
    arduino Framework uses MAX and MIN 
      defined like : MAX(a, b) ((a)>(b)?(a):(b))

#define MAX(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a > _b ? _a : _b; \
})

#define MIN(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a < _b ? _a : _b; \
})


*/ 

/* 
fast and simple convert function 

ingnores whitespaces (leading, ending, seperating)
0x or 0X will be interpreted as HEX number 
negative numbers (leading '-') will be evaluated as signed int 32 and casted before return to uint32_t (you must recast it to s32 )

for some ideas how to use see test module: helperTest.cpp
*/
uint8_t  convertCharToInt(const char c);            // returns 255 if not in ['0' .. '9'] and ['a'..'f'] and ['A'..'F']
uint8_t  convertDezCharToInt(const char c);         // returns 255 if not in ['0' .. '9']
uint32_t convertStrToInt(const char * str);
uint32_t convertStrToInt(String str);

// limit range of a value
uint32_t clampUint32(uint32_t lowEnd,uint32_t value,uint32_t highEnd);
int32_t clamp(int32_t lowEnd,int32_t value,int32_t highEnd);

int32_t wrapAround(int32_t lowEnd,int32_t value,int32_t highEnd);

// color 565 format 
uint16_t toColor565(uint8_t r,uint8_t g,uint8_t b);
uint16_t toColor565(uint8_t r,uint8_t g,uint8_t b,uint8_t dim);
uint16_t toColor565(uint32_t c);
uint16_t toColor565Dim(uint32_t c);


// color dimming based on 255 base
inline uint8_t  dimColorChannel255(uint8_t c,uint8_t dim) {    return ((uint32_t)c*dim)/255; };
uint32_t color24Bit(uint8_t r,uint8_t g,uint8_t b);
uint32_t dimColor255(uint32_t color,uint8_t dim );
uint32_t dimColor255(uint8_t r,uint8_t g,uint8_t b,uint8_t dim );
uint32_t dimRgb24ToRgb(uint32_t value);

// color wheel / rainbow
uint32_t getColorWheel24Bit(uint8_t pos);
uint16_t getColorWheel565(uint8_t pos);



union uint32_t_byteAcess {
    uint32_t uint32;
    struct {
        uint8_t L; // least significant byte
        uint8_t H;
        uint8_t HH;
        uint8_t HHH; // most significant byte
    } byte;
};


union uint16_t_byteAcess {
    uint16_t uint16;
    struct {
        uint8_t L; // least significant byte
        uint8_t H;
    } byte;
};

#define HHH_BYTE(val)       (*(((uint8_t*)&val)+3))
#define HH_BYTE(val)        (*(((uint8_t*)&val)+2))
#define H_BYTE(val)         (*(((uint8_t*)&val)+1))
#define L_BYTE(val)         (*(((uint8_t*)&val)+0))

#define H_WORD(val)         (*(((uint16_t*)&val)+1))   
#define L_WORD(val)         (*(((uint16_t*)&val)+0))   


int findStringInArray(String * pFirst,int count,String target);

String removeLeadingCharacters(String input, char charToRemove);
String removeLeadingCharacters(String input, const String& charsToRemove);
String removeTrailingCharacters(String input, char charToRemove);
String removeTrailingCharacters(String input, const String& charsToRemove);
