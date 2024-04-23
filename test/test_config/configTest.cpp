#include <Arduino.h>
#include <unity.h>

#include <configItem.hpp>

String configIn;
JsonDocument config;


void setup_configIn(void){
  configIn ="";
  configIn += " {                                                    \r\n";
  configIn += "     \"obj\"  : \"LED0\",                             \r\n";
  configIn += "     \"cmd\"  :    \"off\",                           \r\n";
  configIn += "     \"param\": [\"0x00FF00\",\"  2 \",\" 0 \",\"\"], \r\n";
  configIn += "     \"str\"  :  \"abc -# \",                         \r\n";
  configIn += "     \"bin\":[0,2,255,40,30]                          \r\n";
  configIn += " }                                         ";
}


// in include files first define tests :
void test_decode_config(void) {
  DeserializationError error = deserializeJson(config, configIn);
  TEST_ASSERT_FALSE_MESSAGE((error),"deserializeJson() returned error :");
  if (error){
    TEST_MESSAGE(error.c_str());
  }

}

void test_constructor_config(void){
  configItem item(config);
  
  
  TEST_ASSERT_EQUAL_PTR(&ledCtrl1   ,item.obj());
  TEST_ASSERT_EQUAL_STRING("off"    ,item.cmd().c_str());
  TEST_ASSERT_EQUAL_STRING("abc -# ",item.str().c_str());
  TEST_ASSERT_EQUAL_UINT32(0xFF00   ,item.param(0));
  TEST_ASSERT_EQUAL_UINT32(2        ,item.param(1));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(2));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(3));
  TEST_ASSERT_EQUAL_UINT32(5        ,item.size());
  TEST_ASSERT_EQUAL_UINT8(0         ,item.data(0));
  TEST_ASSERT_EQUAL_UINT8(2         ,item.data(1));
  TEST_ASSERT_EQUAL_UINT8(255       ,item.data(2));
  TEST_ASSERT_EQUAL_UINT8(40        ,item.data(3));
  TEST_ASSERT_EQUAL_UINT8(30        ,item.data(4));
  
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(100));
  TEST_ASSERT_EQUAL_UINT8(0         ,item.data(100));
  
}

void test_copyConstructor_config(void){
  configItem src(config);
  configItem item(src);

  TEST_ASSERT_EQUAL_PTR(&ledCtrl1   ,item.obj());
  TEST_ASSERT_EQUAL_STRING("off"    ,item.cmd().c_str());
  TEST_ASSERT_EQUAL_STRING("abc -# ",item.str().c_str());
  TEST_ASSERT_EQUAL_UINT32(0xFF00   ,item.param(0));
  TEST_ASSERT_EQUAL_UINT32(2        ,item.param(1));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(2));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(3));
  TEST_ASSERT_EQUAL_UINT32(5        ,item.size());
  TEST_ASSERT_EQUAL_UINT8(0         ,item.data(0));
  TEST_ASSERT_EQUAL_UINT8(2         ,item.data(1));
  TEST_ASSERT_EQUAL_UINT8(255       ,item.data(2));
  TEST_ASSERT_EQUAL_UINT8(40        ,item.data(3));
  TEST_ASSERT_EQUAL_UINT8(30        ,item.data(4));
}

void test_collection_configItem(void) {

  setup_configIn();
  RUN_TEST(test_decode_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_copyConstructor_config);
}

