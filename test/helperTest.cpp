#include "helper.hpp"
#include "unity.h"

/*

u16_t toColor565(u8_t r,u8_t g,u8_t b);
u16_t toColor565(u32_t c);
u32_t get888ColorWheel(u8_t pos);

u32_t dimRgb24ToRgb(u32_t value);


*/


void test_helper_dimColor255_1(void){
    TEST_ASSERT_EQUAL_UINT32( 0x00000000 , dimColor255(0x00000000,0x00) );
    TEST_ASSERT_EQUAL_UINT32( 0x00000000 , dimColor255(0x00FFFFFF,0x00) );
    TEST_ASSERT_EQUAL_UINT32( 0x00AABBCC , dimColor255(0x00AABBCC,0xFF) );
    TEST_ASSERT_EQUAL_UINT32( 0x00556677 , dimColor255(0x00AACCDD,0x80) );
}

void test_helper_dimColor255_2(void){
    TEST_ASSERT_EQUAL_UINT32( 0x00000000 , dimColor255(0x00,0x00,0x00,0x00) );
    TEST_ASSERT_EQUAL_UINT32( 0x00000000 , dimColor255(0xFF,0xFF,0xFF,0x00) );
    TEST_ASSERT_EQUAL_UINT32( 0x00AABBCC , dimColor255(0xAA,0xBB,0xCC,0xFF) );
    TEST_ASSERT_EQUAL_UINT32( 0x00556677 , dimColor255(0xAA,0xCC,0xDD,0x80) );
}

void test_helper_dimColor255_3(void){
    TEST_ASSERT_EQUAL_UINT32( 0x00000000 , dimRgb24ToRgb(0x00000000) );
    TEST_ASSERT_EQUAL_UINT32( 0x00000000 , dimRgb24ToRgb(0x00FFFFFF) );
    TEST_ASSERT_EQUAL_UINT32( 0x00AABBCC , dimRgb24ToRgb(0xFFAABBCC) );
    TEST_ASSERT_EQUAL_UINT32( 0x00556677 , dimRgb24ToRgb(0x80AACCDD) );
}


void test_helper_dimSingleColor(void){
    TEST_ASSERT_EQUAL_UINT32( 0     , dimColorChannel255(0,0) );

    TEST_ASSERT_EQUAL_UINT32( 0     , dimColorChannel255(255,0));
    TEST_ASSERT_EQUAL_UINT32( 50    , dimColorChannel255(255,50));
    TEST_ASSERT_EQUAL_UINT32( 100   , dimColorChannel255(255,100));
    TEST_ASSERT_EQUAL_UINT32( 255   , dimColorChannel255(255,255));

    TEST_ASSERT_EQUAL_UINT32( 0     , dimColorChannel255(0,255));
    TEST_ASSERT_EQUAL_UINT32( 42    , dimColorChannel255( 42,255));
    TEST_ASSERT_EQUAL_UINT32( 100   , dimColorChannel255(100,255));

    TEST_ASSERT_EQUAL_UINT32( 50    , dimColorChannel255(100,128));
    TEST_ASSERT_EQUAL_UINT32( 100   , dimColorChannel255(200,128));
}


void test_helper_clamp(void){
    TEST_ASSERT_EQUAL_UINT32(42,    clamp(0,42,255));
    TEST_ASSERT_EQUAL_UINT32(100,   clamp(100,42,255));
    TEST_ASSERT_EQUAL_UINT32(255,   clamp(100,300,255));
    TEST_ASSERT_EQUAL_UINT32(100,   clamp(100,100,255));
    TEST_ASSERT_EQUAL_UINT32(255,   clamp(100,255,255));
}

void test_helper_convertStrToIntDec1(void){
    TEST_ASSERT_EQUAL_UINT32(12345678,  convertStrToInt("12345678"));
    TEST_ASSERT_EQUAL_UINT32(12345678,  convertStrToInt("  12345678"));
    TEST_ASSERT_EQUAL_UINT32(12345678,  convertStrToInt(" 12345678"));
    TEST_ASSERT_EQUAL_UINT32(12345678,  convertStrToInt(" 12345678  "));
    TEST_ASSERT_EQUAL_UINT32(12345678,  convertStrToInt(" 1 2345 678  "));
};

void test_helper_convertStrToIntDec2(void){
    String str;
    str = "42";    
    TEST_ASSERT_EQUAL_UINT32(42,        convertStrToInt(str));

    TEST_ASSERT_EQUAL_UINT32(0,         convertStrToInt("  "));
    TEST_ASSERT_EQUAL_UINT32(0,         convertStrToInt("0"));
    TEST_ASSERT_EQUAL_UINT32(0,         convertStrToInt(""));
    TEST_ASSERT_EQUAL_UINT32(0,         convertStrToInt(" 1 2345 678 € "));
    //TEST_ASSERT_EQUAL_UINT32(0,         convertStrToInt(" 1.2345.678 "));
}

void test_helper_convertStrToIntDec3(void){
    TEST_ASSERT_EQUAL_UINT32(         1,    convertStrToInt(" 1  ")             );
    TEST_ASSERT_EQUAL_UINT32(        20,    convertStrToInt(" 20  ")            );
    TEST_ASSERT_EQUAL_UINT32(       300,    convertStrToInt(" 300  ")           );
    TEST_ASSERT_EQUAL_UINT32(      4000,    convertStrToInt(" 4000  ")          );
    TEST_ASSERT_EQUAL_UINT32(     50000,    convertStrToInt(" 50 000  ")        );
    TEST_ASSERT_EQUAL_UINT32(    600000,    convertStrToInt(" 600 000  ")       );
    TEST_ASSERT_EQUAL_UINT32(   7000000,    convertStrToInt(" 7 000 000  ")     );
    TEST_ASSERT_EQUAL_UINT32(  80000000,    convertStrToInt(" 80 000 000  ")    );
    TEST_ASSERT_EQUAL_UINT32( 900000000,    convertStrToInt(" 900 000 000  ")   );
    TEST_ASSERT_EQUAL_UINT32( 0xFFFFFFFF,   convertStrToInt(" 4 294 967 295  ") );
    TEST_ASSERT_EQUAL_UINT32(          0,   convertStrToInt(" 4 294 967 296  ") );
    TEST_ASSERT_EQUAL_UINT32(          0,   convertStrToInt("14 000 000 000  ") );
}

void test_helper_convertStrToIntHex(void){
    TEST_ASSERT_EQUAL_UINT32( 0x12345678,   convertStrToInt("0x12345678")      );
    TEST_ASSERT_EQUAL_UINT32(     0x1234,   convertStrToInt("  0x1234")        );
    TEST_ASSERT_EQUAL_UINT32( 0xdeadbeef,   convertStrToInt(" 0xdeAdBeEF")     );
    TEST_ASSERT_EQUAL_UINT32( 0xdeadbeef,   convertStrToInt(" 0X Dead Beef  ") );

    String str;
    str = "0x42";    
    TEST_ASSERT_EQUAL_UINT32(       0x42,   convertStrToInt(str));

    TEST_ASSERT_EQUAL_UINT32(          0,   convertStrToInt("0x"));
    TEST_ASSERT_EQUAL_UINT32(          0,   convertStrToInt("0xAAAAX"));
    TEST_ASSERT_EQUAL_UINT32(          0,   convertStrToInt(" 0x0  "));
    TEST_ASSERT_EQUAL_UINT32( 0xFFFFFFFF,   convertStrToInt(" 0xFFFF FFFF  "));
}


void test_helper_convertStrToIntNeg(void){
    TEST_ASSERT_EQUAL_INT32(          -1,   (s32_t) convertStrToInt("-1"));
    TEST_ASSERT_EQUAL_INT32(        -233,   (s32_t) convertStrToInt("  - 233 "));
    TEST_ASSERT_EQUAL_INT32(       -1233,   (s32_t) convertStrToInt("-  1 233"));

    TEST_ASSERT_EQUAL_INT32(           0,   (s32_t) convertStrToInt("-0"));
    TEST_ASSERT_EQUAL_INT32(           0,   (s32_t) convertStrToInt("-1233-"));
}


void test_helper_u32_byteAcess(void) {
    union u32_byteAcess obj;

    obj.ival = 0x44332211;

    TEST_ASSERT_EQUAL_UINT8(0x44, obj.bval.HHH);
    TEST_ASSERT_EQUAL_UINT8(0x33, obj.bval.HH);
    TEST_ASSERT_EQUAL_UINT8(0x22, obj.bval.H);
    TEST_ASSERT_EQUAL_UINT8(0x11, obj.bval.L);

    TEST_ASSERT_EQUAL_UINT8(0x44, obj.bval.HHH);
    TEST_ASSERT_EQUAL_UINT8(0x33, obj.bval.HH);
    TEST_ASSERT_EQUAL_UINT8(0x22, obj.bval.H);
    TEST_ASSERT_EQUAL_UINT8(0x11, obj.bval.L);

    TEST_ASSERT_EQUAL_UINT32(0x44332211 , obj.ival);

    obj.bval.HHH = 0x55;
    obj.bval.HH  = 0x66;
    obj.bval.H   = 0x77;
    obj.bval.L   = 0x88;

    TEST_ASSERT_EQUAL_UINT8(0x55, obj.bval.HHH);
    TEST_ASSERT_EQUAL_UINT8(0x66, obj.bval.HH);
    TEST_ASSERT_EQUAL_UINT8(0x77, obj.bval.H);
    TEST_ASSERT_EQUAL_UINT8(0x88, obj.bval.L);

    TEST_ASSERT_EQUAL_UINT32(0x55667788 , obj.ival);
}

void test_helper_u16_byteAcess(void) {
    union u16_byteAcess obj;

    obj.ival = 0x2211;

    TEST_ASSERT_EQUAL_UINT32(obj.bval.H    , 0x22);
    TEST_ASSERT_EQUAL_UINT32(obj.bval.L    , 0x11);

    TEST_ASSERT_EQUAL_UINT32(obj.bval.H    , 0x22);
    TEST_ASSERT_EQUAL_UINT32(obj.bval.L    , 0x11);

    TEST_ASSERT_EQUAL_UINT32(obj.ival , 0x2211);

    obj.bval.H   = 0x77;
    obj.bval.L   = 0x88;

    TEST_ASSERT_EQUAL_UINT32(obj.bval.H    , 0x77);
    TEST_ASSERT_EQUAL_UINT32(obj.bval.L    , 0x88);

    TEST_ASSERT_EQUAL_UINT32(obj.ival , 0x7788);
}


void test_helper_HHH_BYTE(void) {
    u32_t value = 0x44332211;
  
    TEST_ASSERT_EQUAL_UINT32(HHH_BYTE(value) , 0x44);
    TEST_ASSERT_EQUAL_UINT32(HH_BYTE(value)  , 0x33);
    TEST_ASSERT_EQUAL_UINT32(H_BYTE(value)   , 0x22);
    TEST_ASSERT_EQUAL_UINT32(L_BYTE(value)   , 0x11);

    TEST_ASSERT_EQUAL_UINT32(HHH_BYTE(value) , 0x44);
    TEST_ASSERT_EQUAL_UINT32(HH_BYTE(value)  , 0x33);
    TEST_ASSERT_EQUAL_UINT32(H_BYTE(value)   , 0x22);
    TEST_ASSERT_EQUAL_UINT32(L_BYTE(value)   , 0x11);

    TEST_ASSERT_EQUAL_UINT32(value , 0x44332211);

    HHH_BYTE(value) = 0x55;
    HH_BYTE(value)  = 0x66;
    H_BYTE(value)   = 0x77;
    L_BYTE(value)   = 0x88;

    TEST_ASSERT_EQUAL_UINT32(value , 0x55667788);
}


void test_helper_H_WORD(void) {
    u32_t value = 0x44332211;
  
    TEST_ASSERT_EQUAL_UINT16(0x4433 , H_WORD(value));
    TEST_ASSERT_EQUAL_UINT16(0x2211 , L_WORD(value));

    TEST_ASSERT_EQUAL_UINT16(0x4433 , H_WORD(value));
    TEST_ASSERT_EQUAL_UINT16(0x2211 , L_WORD(value));

    TEST_ASSERT_EQUAL_UINT32(0x44332211 , value);

    HHH_BYTE(value) = 0x55;
    HH_BYTE(value)  = 0x66;
    H_BYTE(value)   = 0x77;
    L_BYTE(value)   = 0x88;

    TEST_ASSERT_EQUAL_UINT16(0x5566 , H_WORD(value));
    TEST_ASSERT_EQUAL_UINT16(0x7788 , L_WORD(value));

    H_WORD(value) = 0x99AA;
    L_WORD(value) = 0xBBCC;
    TEST_ASSERT_EQUAL_UINT32(0x99AABBCC, value);
}


// collect all tests of this file to one collection
void test_collection_helper(void) {
  RUN_TEST(test_helper_HHH_BYTE);
  RUN_TEST(test_helper_H_WORD);
  RUN_TEST(test_helper_u32_byteAcess);
  RUN_TEST(test_helper_u16_byteAcess);

  RUN_TEST(test_helper_convertStrToIntDec1);
  RUN_TEST(test_helper_convertStrToIntDec2);
  RUN_TEST(test_helper_convertStrToIntDec3);
  RUN_TEST(test_helper_convertStrToIntHex);
  RUN_TEST(test_helper_convertStrToIntNeg);

  RUN_TEST(test_helper_clamp);

  RUN_TEST(test_helper_dimSingleColor);
  RUN_TEST(test_helper_dimColor255_1);
  RUN_TEST(test_helper_dimColor255_2);

}
