#include <Arduino.h>

#include <Debug.hpp>
#include <helper.h>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include <unity.h>




/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//            simple test                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// in include files first define tests :
void test_simple_Test1(void) {
  TEST_ASSERT_TRUE(true);
}
void test_simple_Test2(void) {
  TEST_ASSERT_FALSE(false);
}
void test_simple_Test3(void) {
  TEST_ASSERT_FALSE(true == false);
}


void test_simple_DebugAndUnity(void) {
  ASSERT(true == true,"");
  LOG(F("LOG print in UINTY test framework"));
  TEST_ASSERT_TRUE(true);
}




/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//            helper test                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////



void test_helper_color565_1(void){
    TEST_ASSERT_EQUAL_UINT16( 0x0000 , toColor565(0x00,0x00,0x00));
    TEST_ASSERT_EQUAL_UINT16( 0xF800 , toColor565(0xFF,0x00,0x00));
    TEST_ASSERT_EQUAL_UINT16( 0x07E0 , toColor565(0x00,0xFF,0x00));
    TEST_ASSERT_EQUAL_UINT16( 0x001F , toColor565(0x00,0x00,0xFF));
    TEST_ASSERT_EQUAL_UINT16( 0xFFFF , toColor565(0xFF,0xFF,0xFF));
    
    TEST_ASSERT_EQUAL_UINT16( 0x632C , toColor565(0x64,0x64,0x64));
    
    TEST_ASSERT_EQUAL_UINT16( 0x07FF , toColor565(0x05,0xFF,0xFF));
    TEST_ASSERT_EQUAL_UINT16( 0xF83F , toColor565(0xFF,0x05,0xFF));
    TEST_ASSERT_EQUAL_UINT16( 0xFFE0 , toColor565(0xFF,0xFF,0x05));
}

void test_helper_color565_2(void){
    TEST_ASSERT_EQUAL_UINT16( 0x0000 , toColor565(0x00000000));
    TEST_ASSERT_EQUAL_UINT16( 0xF800 , toColor565(0x00FF0000));
    TEST_ASSERT_EQUAL_UINT16( 0x07E0 , toColor565(0x0000FF00));
    TEST_ASSERT_EQUAL_UINT16( 0x001F , toColor565(0x000000FF));
    TEST_ASSERT_EQUAL_UINT16( 0xFFFF , toColor565(0x00FFFFFF));
    
    TEST_ASSERT_EQUAL_UINT16( 0x632C , toColor565(0x00646464));
    
    TEST_ASSERT_EQUAL_UINT16( 0x07FF , toColor565(0x0005FFFF));
    TEST_ASSERT_EQUAL_UINT16( 0xF83F , toColor565(0x00FF05FF));
    TEST_ASSERT_EQUAL_UINT16( 0xFFE0 , toColor565(0x00FFFF05));
}

void test_helper_color565_3(void){
    TEST_ASSERT_EQUAL_UINT16( 0x0000 , toColor565(0x00,0x00,0x00,0xFF));
    TEST_ASSERT_EQUAL_UINT16( 0xFFFF , toColor565(0xFF,0xFF,0xFF,0xFF));
    TEST_ASSERT_EQUAL_UINT16( 0x0000 , toColor565(0xFF,0xFF,0xFF,0x00));
    TEST_ASSERT_EQUAL_UINT16( 0x632C , toColor565(0xFF,0xFF,0xFF,0x64));
}



void test_helper_color565_4(void){
    TEST_ASSERT_EQUAL_UINT16( 0x0000 , toColor565Dim(0xFF000000));
    TEST_ASSERT_EQUAL_UINT16( 0xFFFF , toColor565Dim(0xFFFFFFFF));
    TEST_ASSERT_EQUAL_UINT16( 0x0000 , toColor565Dim(0x00FFFFFF));
    TEST_ASSERT_EQUAL_UINT16( 0x632C , toColor565Dim(0x64FFFFFF));
}


void test_helper_dimColor255_1(void){
    TEST_ASSERT_EQUAL_UINT32( 0x00000000 , dimColor255(0x00000000,0x00) );
    TEST_ASSERT_EQUAL_UINT32( 0x00000000 , dimColor255(0x00FFFFFF,0x00) );
    TEST_ASSERT_EQUAL_UINT32( 0x00AABBCC , dimColor255(0x00AABBCC,0xFF) );
    TEST_ASSERT_EQUAL_UINT32( 0x00556677 , dimColor255(0x00AACCEE,0x80) );
}

void test_helper_dimColor255_2(void){
    TEST_ASSERT_EQUAL_UINT32( 0x00000000 , dimColor255(0x00,0x00,0x00,0x00) );
    TEST_ASSERT_EQUAL_UINT32( 0x00000000 , dimColor255(0xFF,0xFF,0xFF,0x00) );
    TEST_ASSERT_EQUAL_UINT32( 0x00AABBCC , dimColor255(0xAA,0xBB,0xCC,0xFF) );
    TEST_ASSERT_EQUAL_UINT32( 0x00556677 , dimColor255(0xAA,0xCC,0xEE,0x80) );
}

void test_helper_dimColor255_3(void){
    TEST_ASSERT_EQUAL_UINT32( 0x00000000 , dimRgb24ToRgb(0x00000000) );
    TEST_ASSERT_EQUAL_UINT32( 0x00000000 , dimRgb24ToRgb(0x00FFFFFF) );
    TEST_ASSERT_EQUAL_UINT32( 0x00AABBCC , dimRgb24ToRgb(0xFFAABBCC) );
    TEST_ASSERT_EQUAL_UINT32( 0x00556677 , dimRgb24ToRgb(0x80AACCEE) );
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

    TEST_ASSERT_EQUAL_UINT32(42,    clampUint32(0,    42,   255));
    TEST_ASSERT_EQUAL_UINT32(100,   clampUint32(100,  42,   255));
    TEST_ASSERT_EQUAL_UINT32(255,   clampUint32(100,  300,  255));
    TEST_ASSERT_EQUAL_UINT32(100,   clampUint32(100,  100,  255));
    TEST_ASSERT_EQUAL_UINT32(255,   clampUint32(100,  255,  255));
    TEST_ASSERT_EQUAL_UINT32(0xFFFFFFF0,    clampUint32(0,    0xFFFFFFFF,  0xFFFFFFF0));
    TEST_ASSERT_EQUAL_UINT32(0xF0,          clampUint32(0xF0, 0x0,         0xFFFFFFF0));

    TEST_ASSERT_EQUAL_UINT32(42,    clamp(0,    42,   255));
    TEST_ASSERT_EQUAL_UINT32(100,   clamp(100,  42,   255));
    TEST_ASSERT_EQUAL_UINT32(255,   clamp(100,  300,  255));
    TEST_ASSERT_EQUAL_UINT32(100,   clamp(100,  100,  255));
    TEST_ASSERT_EQUAL_UINT32(255,   clamp(100,  255,  255));

    TEST_ASSERT_EQUAL_INT32(-1,     clamp(-10, -1,    100));
    TEST_ASSERT_EQUAL_INT32(-5,     clamp(-10, -5,    -1) );
    TEST_ASSERT_EQUAL_INT32( 3,     clamp(-10, 3 ,    10) );
    TEST_ASSERT_EQUAL_INT32(-5,     clamp(-5,  -10,   10) );
    TEST_ASSERT_EQUAL_INT32(100,    clamp(-100,1000,  100));


}


void test_helper_wrapAround(void){
    TEST_ASSERT_EQUAL_INT32(42,     wrapAround(0,   42, 255)    );
    TEST_ASSERT_EQUAL_INT32(0,      wrapAround(0,   10, 9)      );
    TEST_ASSERT_EQUAL_INT32(20,     wrapAround(20,  30, 29)     );
    TEST_ASSERT_EQUAL_INT32(29,     wrapAround(20,  19, 29)     );
    
    TEST_ASSERT_EQUAL_INT32(9,      wrapAround(0,   -1, 9)      );
    TEST_ASSERT_EQUAL_INT32(0,      wrapAround(0,   0,  9)      );
    TEST_ASSERT_EQUAL_INT32(9,      wrapAround(0,   9,  9)      );
    
    TEST_ASSERT_EQUAL_INT32(-42,    wrapAround(-100, -42, -10)    );
    TEST_ASSERT_EQUAL_INT32(-50,    wrapAround(-50,  -9,  -10)    );
    TEST_ASSERT_EQUAL_INT32(-10,    wrapAround(-50,  -51, -10)    );

    TEST_ASSERT_EQUAL_INT32(3,      wrapAround(0,  0-1, 3)        );

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
    TEST_ASSERT_EQUAL_INT32(          -1,   (int32_t ) convertStrToInt("-1"));
    TEST_ASSERT_EQUAL_INT32(        -233,   (int32_t ) convertStrToInt("  - 233 "));
    TEST_ASSERT_EQUAL_INT32(       -1233,   (int32_t ) convertStrToInt("-  1 233"));

    TEST_ASSERT_EQUAL_INT32(           0,   (int32_t ) convertStrToInt("-0"));
    TEST_ASSERT_EQUAL_INT32(           0,   (int32_t ) convertStrToInt("-1233-"));
}


void test_helper_uint32_t_byteAcess(void) {
    union uint32_t_byteAcess obj;

    obj.uint32 = 0x44332211;

    TEST_ASSERT_EQUAL_UINT8(0x44, obj.byte.HHH);
    TEST_ASSERT_EQUAL_UINT8(0x33, obj.byte.HH);
    TEST_ASSERT_EQUAL_UINT8(0x22, obj.byte.H);
    TEST_ASSERT_EQUAL_UINT8(0x11, obj.byte.L);

    TEST_ASSERT_EQUAL_UINT8(0x44, obj.byte.HHH);
    TEST_ASSERT_EQUAL_UINT8(0x33, obj.byte.HH);
    TEST_ASSERT_EQUAL_UINT8(0x22, obj.byte.H);
    TEST_ASSERT_EQUAL_UINT8(0x11, obj.byte.L);

    TEST_ASSERT_EQUAL_UINT32(0x44332211 , obj.uint32);

    obj.byte.HHH = 0x55;
    obj.byte.HH  = 0x66;
    obj.byte.H   = 0x77;
    obj.byte.L   = 0x88;

    TEST_ASSERT_EQUAL_UINT8(0x55, obj.byte.HHH);
    TEST_ASSERT_EQUAL_UINT8(0x66, obj.byte.HH);
    TEST_ASSERT_EQUAL_UINT8(0x77, obj.byte.H);
    TEST_ASSERT_EQUAL_UINT8(0x88, obj.byte.L);

    TEST_ASSERT_EQUAL_UINT32(0x55667788 , obj.uint32);
}

void test_helper_u16_byteAcess(void) {
    union uint16_t_byteAcess obj;

    obj.uint16 = 0x2211;

    TEST_ASSERT_EQUAL_UINT32(obj.byte.H    , 0x22);
    TEST_ASSERT_EQUAL_UINT32(obj.byte.L    , 0x11);

    TEST_ASSERT_EQUAL_UINT32(obj.byte.H    , 0x22);
    TEST_ASSERT_EQUAL_UINT32(obj.byte.L    , 0x11);

    TEST_ASSERT_EQUAL_UINT32(obj.uint16 , 0x2211);

    obj.byte.H   = 0x77;
    obj.byte.L   = 0x88;

    TEST_ASSERT_EQUAL_UINT32(obj.byte.H    , 0x77);
    TEST_ASSERT_EQUAL_UINT32(obj.byte.L    , 0x88);

    TEST_ASSERT_EQUAL_UINT32(obj.uint16 , 0x7788);
}


void test_helper_HHH_BYTE(void) {
    uint32_t value = 0x44332211;
  
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
    uint32_t value = 0x44332211;
  
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


// now we call here all test collections
int runAllTests(void) {
  UNITY_BEGIN();

  RUN_TEST(test_simple_Test1);
  RUN_TEST(test_simple_Test2);
  RUN_TEST(test_simple_Test3);
  RUN_TEST(test_simple_DebugAndUnity);


  RUN_TEST(test_helper_HHH_BYTE);
  RUN_TEST(test_helper_H_WORD);
  RUN_TEST(test_helper_uint32_t_byteAcess);
  RUN_TEST(test_helper_u16_byteAcess);

  RUN_TEST(test_helper_convertStrToIntDec1);
  RUN_TEST(test_helper_convertStrToIntDec2);
  RUN_TEST(test_helper_convertStrToIntDec3);
  RUN_TEST(test_helper_convertStrToIntHex);
  RUN_TEST(test_helper_convertStrToIntNeg);

  RUN_TEST(test_helper_clamp);
  RUN_TEST(test_helper_wrapAround);
  

  RUN_TEST(test_helper_dimSingleColor);
  RUN_TEST(test_helper_dimColor255_1);
  RUN_TEST(test_helper_dimColor255_2);
  RUN_TEST(test_helper_dimColor255_3);

  RUN_TEST(test_helper_color565_1);
  RUN_TEST(test_helper_color565_2);
  RUN_TEST(test_helper_color565_3);
  RUN_TEST(test_helper_color565_4);

  return UNITY_END();
}





/**
  * For Arduino framework
  */
#include <MainConfig.h>
#include <Blink.hpp>
BlinkingLED  blink = BlinkingLED(LED_BUILTIN);
std::vector<uint32_t> testBlinkSeq = BLINK_SEQ_TEST;

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}


void setup() {
  blink.on();

  // Wait ~2 seconds before the Unity test runner
  // establishes connection with a board Serial interface
  delay(WAIT_FOR_UINTY_FRAMEWORK);

  runAllTests();

  blink.setup(testBlinkSeq);
}


void loop() {
  uint32_t now = millis();
  blink.loop(now);  
}
