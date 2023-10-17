#pragma once
#include <Arduino.h>
#include "StringList.hpp"

/* 
fast and simple convert function 

ingnores whitespaces (leading, ending, seperating)
0x or 0X will be interpreted as HEX number 
negative numbers (leading '-') will be evaluated as signed int 32 and casted before return to u32_t (you must recast it to s32_t )

for some ideas how to use see test module: helperTest.cpp
*/
u32_t convertStrToInt(const char * str);
u32_t convertStrToInt(String str);

// limit range of a value
u32_t clamp(u32_t lowEnd,u32_t value,u32_t highEnd);


// color 565 format 
u16_t toColor565(u8_t r,u8_t g,u8_t b);
u16_t toColor565(u8_t r,u8_t g,u8_t b,u8_t dim);
u16_t toColor565(u32_t c);
u16_t toColor565Dim(u32_t c);


// color dimming based on 255 base
inline u8_t  dimColorChannel255(u8_t c,u8_t dim) {    return ((u32_t)c*dim)/255; };
u32_t dimColor255(u32_t color,u8_t dim );
u32_t dimColor255(u8_t r,u8_t g,u8_t b,u8_t dim );
u32_t dimRgb24ToRgb(u32_t value);

// color wheel / rainbow
u32_t getColorWheel24Bit(u8_t pos);
u16_t getColorWheel565(u8_t pos);



union u32_byteAcess {
    u32_t ival;
    struct {
        u8_t L; // least significant byte
        u8_t H;
        u8_t HH;
        u8_t HHH; // most significant byte
    } bval;
};


union u16_byteAcess {
    u16_t ival;
    struct {
        u8_t L; // least significant byte
        u8_t H;
    } bval;
};

#define HHH_BYTE(val)       (*(((u8_t*)&val)+3))
#define HH_BYTE(val)        (*(((u8_t*)&val)+2))
#define H_BYTE(val)         (*(((u8_t*)&val)+1))
#define L_BYTE(val)         (*(((u8_t*)&val)+0))

#define H_WORD(val)         (*(((u16_t*)&val)+1))   
#define L_WORD(val)         (*(((u16_t*)&val)+0))   

//extern String emptyString;