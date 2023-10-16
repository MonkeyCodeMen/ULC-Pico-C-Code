#pragma once
#include <Arduino.h>
#include "StringList.hpp"

u32_t convertStrToInt(const char * str);
u32_t convertStrToInt(String str);

u32_t clamp(u32_t lowEnd,u32_t value,u32_t highEnd);

u16_t toColor565(u8_t r,u8_t g,u8_t b);
u16_t toColor565(u32_t c);

u32_t dimColor255(u32_t color,u8_t dim );
u32_t dimColor255(u8_t r,u8_t g,u8_t b,u8_t dim );
inline u8_t  dimColorChannel255(u8_t c,u8_t dim) {    return ((u32_t)c*dim)/255; };
u32_t dimRgb24ToRgb(u32_t value);

u32_t get888ColorWheel(u8_t pos);


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