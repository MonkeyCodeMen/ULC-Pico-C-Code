#include "helper.hpp"
#include "unity.h"

/*
u32_t convertStrToInt(const char * str);
u32_t convertStrToInt(String str);

u32_t clamp(u32_t lowEnd,u32_t value,u32_t highEnd);

u16_t toColor565(u8_t r,u8_t g,u8_t b);
u16_t toColor565(u32_t c);

u32_t get888ColorWheel(u8_t pos);

*/


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
  
}
