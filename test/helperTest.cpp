#include "helper.hpp"
#include "unity.h"

/*

u16_t toColor565(u8_t r,u8_t g,u8_t b);
u16_t toColor565(u32_t c);

u32_t dimColor255(u32_t color,u8_t dim );
u32_t dimColor255(u8_t r,u8_t g,u8_t b,u8_t dim );
inline u8_t  dimColorChannel255(u8_t c,u8_t dim) {    return ((u32_t)c*dim)/255; };
u32_t dimRgb24ToRgb(u32_t value);

u32_t get888ColorWheel(u8_t pos);

*/


void test_helper_dimColor(void){
    TEST_ASSERT_TRUE(dimColorChannel255(0,0)     ==   0 );

    TEST_ASSERT_TRUE(dimColorChannel255(255,0)   ==   0 );
    TEST_ASSERT_TRUE(dimColorChannel255(255,50)  ==  50 );
    TEST_ASSERT_TRUE(dimColorChannel255(255,100) == 100 );
    TEST_ASSERT_TRUE(dimColorChannel255(255,255) == 255 );

    TEST_ASSERT_TRUE(dimColorChannel255(  0,255) ==   0 );
    TEST_ASSERT_TRUE(dimColorChannel255( 42,255) ==  42 );
    TEST_ASSERT_TRUE(dimColorChannel255(100,255) == 100 );

    TEST_ASSERT_TRUE(dimColorChannel255(100,128) ==  50 );
    TEST_ASSERT_TRUE(dimColorChannel255(200,128) == 100 );
    


}


void test_helper_clamp(void){
    TEST_ASSERT_TRUE(clamp(0,42,255)    == 42);
    TEST_ASSERT_TRUE(clamp(100,42,255)  == 100);
    TEST_ASSERT_TRUE(clamp(100,300,255) == 255);
    TEST_ASSERT_TRUE(clamp(100,100,255) == 100);
    TEST_ASSERT_TRUE(clamp(100,255,255) == 255);
}

void test_helper_convertStrToIntDec(void){
    TEST_ASSERT_TRUE(convertStrToInt("12345678") == 12345678);
    TEST_ASSERT_TRUE(convertStrToInt("  12345678") == 12345678);
    TEST_ASSERT_TRUE(convertStrToInt(" 12345678") == 12345678);
    TEST_ASSERT_TRUE(convertStrToInt(" 12345678  ") == 12345678);
    TEST_ASSERT_TRUE(convertStrToInt(" 1 2345 678  ") == 12345678);


    String str;
    str = "42";    
    TEST_ASSERT_TRUE(convertStrToInt(str) == 42);

    TEST_ASSERT_TRUE(convertStrToInt("  ") == 0);
    TEST_ASSERT_TRUE(convertStrToInt("0") == 0);
    TEST_ASSERT_TRUE(convertStrToInt("") == 0);
    TEST_ASSERT_TRUE(convertStrToInt(" 1 2345 678 € ") == 0);
    TEST_ASSERT_TRUE(convertStrToInt(" 1.2345.678 ") == 0);

    TEST_ASSERT_TRUE(convertStrToInt(" 1  ")             == 1);
    TEST_ASSERT_TRUE(convertStrToInt(" 20  ")            == 20);
    TEST_ASSERT_TRUE(convertStrToInt(" 300  ")           == 300);
    TEST_ASSERT_TRUE(convertStrToInt(" 4000  ")          == 4000);
    TEST_ASSERT_TRUE(convertStrToInt(" 50 000  ")        == 50000);
    TEST_ASSERT_TRUE(convertStrToInt(" 600 000  ")       == 600000);
    TEST_ASSERT_TRUE(convertStrToInt(" 7 000 000  ")     == 7000000);
    TEST_ASSERT_TRUE(convertStrToInt(" 80 000 000  ")    == 80000000);
    TEST_ASSERT_TRUE(convertStrToInt(" 900 000 000  ")   == 900000000);

    TEST_ASSERT_TRUE(convertStrToInt(" 4 294 967 295  ") == 0xFFFFFFFF);
    TEST_ASSERT_TRUE(convertStrToInt(" 4 294 967 296  ") == 0);
    TEST_ASSERT_TRUE(convertStrToInt("14 000 000 000  ") == 0);
}

void test_helper_convertStrToIntHex(void){
    TEST_ASSERT_TRUE(convertStrToInt("0x12345678")      == 0x12345678);
    TEST_ASSERT_TRUE(convertStrToInt("  0x1234")        == 0x1234);
    TEST_ASSERT_TRUE(convertStrToInt(" 0xdeAdBeEF")     == 0xdeadbeef);
    TEST_ASSERT_TRUE(convertStrToInt(" 0X Dead Beef  ") == 0xdeadbeef);

    String str;
    str = "0x42";    
    TEST_ASSERT_TRUE(convertStrToInt(str) == 0x42);

    TEST_ASSERT_TRUE(convertStrToInt("0x") == 0);
    TEST_ASSERT_TRUE(convertStrToInt("0xAAAAX") == 0);

    TEST_ASSERT_TRUE(convertStrToInt(" 0x0  ")           == 0);
    TEST_ASSERT_TRUE(convertStrToInt(" 0xFFFF FFFF  ")   == 0xFFFFFFFF);
}


void test_helper_convertStrToIntNeg(void){
    TEST_ASSERT_TRUE((s32_t) convertStrToInt("-1")              == -1);
    TEST_ASSERT_TRUE((s32_t) convertStrToInt("  - 233 ")        == -123);
    TEST_ASSERT_TRUE((s32_t) convertStrToInt("-  1 233")        == -1233);

    TEST_ASSERT_TRUE((s32_t) convertStrToInt("-0")              == 0);
    TEST_ASSERT_TRUE((s32_t) convertStrToInt("-1233-")          == 0);
}


void test_helper_u32_byteAcess(void) {
    union u32_byteAcess obj;

    obj.ival = 0x44332211;

    TEST_ASSERT_TRUE(obj.bval.HHH  == 0x44);
    TEST_ASSERT_TRUE(obj.bval.HH   == 0x33);
    TEST_ASSERT_TRUE(obj.bval.H    == 0x22);
    TEST_ASSERT_TRUE(obj.bval.L    == 0x11);

    TEST_ASSERT_TRUE(obj.bval.HHH  == 0x44);
    TEST_ASSERT_TRUE(obj.bval.HH   == 0x33);
    TEST_ASSERT_TRUE(obj.bval.H    == 0x22);
    TEST_ASSERT_TRUE(obj.bval.L    == 0x11);

    TEST_ASSERT_TRUE(obj.ival == 0x44332211);

    obj.bval.HHH = 0x55;
    obj.bval.HH  = 0x66;
    obj.bval.H   = 0x77;
    obj.bval.L   = 0x88;

    TEST_ASSERT_TRUE(obj.bval.HHH  == 0x55);
    TEST_ASSERT_TRUE(obj.bval.HH   == 0x66);
    TEST_ASSERT_TRUE(obj.bval.H    == 0x77);
    TEST_ASSERT_TRUE(obj.bval.L    == 0x88);

    TEST_ASSERT_TRUE(obj.ival == 0x55667788);
}

void test_helper_u16_byteAcess(void) {
    union u16_byteAcess obj;

    obj.ival = 0x2211;

    TEST_ASSERT_TRUE(obj.bval.H    == 0x22);
    TEST_ASSERT_TRUE(obj.bval.L    == 0x11);

    TEST_ASSERT_TRUE(obj.bval.H    == 0x22);
    TEST_ASSERT_TRUE(obj.bval.L    == 0x11);

    TEST_ASSERT_TRUE(obj.ival == 0x2211);

    obj.bval.H   = 0x77;
    obj.bval.L   = 0x88;

    TEST_ASSERT_TRUE(obj.bval.H    == 0x77);
    TEST_ASSERT_TRUE(obj.bval.L    == 0x88);

    TEST_ASSERT_TRUE(obj.ival == 0x7788);
}


void test_helper_HHH_BYTE(void) {
    u32_t value = 0x44332211;
  
    TEST_ASSERT_TRUE(HHH_BYTE(value) == 0x44);
    TEST_ASSERT_TRUE(HH_BYTE(value)  == 0x33);
    TEST_ASSERT_TRUE(H_BYTE(value)   == 0x22);
    TEST_ASSERT_TRUE(L_BYTE(value)   == 0x11);

    TEST_ASSERT_TRUE(HHH_BYTE(value) == 0x44);
    TEST_ASSERT_TRUE(HH_BYTE(value)  == 0x33);
    TEST_ASSERT_TRUE(H_BYTE(value)   == 0x22);
    TEST_ASSERT_TRUE(L_BYTE(value)   == 0x11);

    TEST_ASSERT_TRUE(value == 0x44332211);

    HHH_BYTE(value) = 0x55;
    HH_BYTE(value)  = 0x66;
    H_BYTE(value)   = 0x77;
    L_BYTE(value)   = 0x88;

    TEST_ASSERT_TRUE(value == 0x55667788);
}


void test_helper_H_WORD(void) {
    u32_t value = 0x44332211;
  
    TEST_ASSERT_TRUE(H_WORD(value) == 0x4433);
    TEST_ASSERT_TRUE(L_WORD(value) == 0x2211);

    TEST_ASSERT_TRUE(H_WORD(value) == 0x4433);
    TEST_ASSERT_TRUE(L_WORD(value) == 0x2211);

    TEST_ASSERT_TRUE(value == 0x44332211);

    HHH_BYTE(value) = 0x55;
    HH_BYTE(value)  = 0x66;
    H_BYTE(value)   = 0x77;
    L_BYTE(value)   = 0x88;

    TEST_ASSERT_TRUE(H_WORD(value) == 0x5566);
    TEST_ASSERT_TRUE(L_WORD(value) == 0x7788);

    H_WORD(value) = 0x99AA;
    L_WORD(value) = 0xBBCC;
    TEST_ASSERT_TRUE(value == 0x99AABBCC);
}


// collect all tests of this file to one collection
void test_collection_helper(void) {
  RUN_TEST(test_helper_HHH_BYTE);
  RUN_TEST(test_helper_H_WORD);
  RUN_TEST(test_helper_u32_byteAcess);
  RUN_TEST(test_helper_u16_byteAcess);

  RUN_TEST(test_helper_convertStrToIntDec);
  RUN_TEST(test_helper_convertStrToIntHex);
  RUN_TEST(test_helper_convertStrToIntNeg);

  RUN_TEST(test_helper_clamp);
  
}
