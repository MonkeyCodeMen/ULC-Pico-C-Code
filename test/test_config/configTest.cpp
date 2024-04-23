#include <Arduino.h>
#include <unity.h>

#include <configItem.hpp>

String streamIn;
JsonDocument config;


// test


void setup_InStream(void){
  streamIn ="";
  streamIn += " {                                         \r\n";
  streamIn += "     \"group\" : \"LED0\",                 \r\n";
  streamIn += "     \"cmd\":    \"off\",                  \r\n";
  streamIn += "     \"param\": [\"0x00FF00\",\"  2 \",\" 0 \",\"\"], \r\n";
  streamIn += "     \"str\":\"abc -# \",                      \r\n";
  streamIn += "     \"bin\":[0,2,255,40,30]               \r\n";
  streamIn += " }                                         ";
}


// in include files first define tests :
void test_decode(void) {
  DeserializationError error = deserializeJson(config, streamIn);
  TEST_ASSERT_FALSE_MESSAGE((error),"deserializeJson() returned error :");
  if (error){
    TEST_MESSAGE(error.c_str());
  }

}

void test_constructor1(void){
  configItem item(config);
  LOG("test constructor entry");
  //TEST_ASSERT_EQUAL_PTR(&ledCtrl1   ,item.obj());
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
  LOG("test constructor exit");
}

void test_copyConstrutcor(void){
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

  setup_InStream();
  RUN_TEST(test_decode);
  RUN_TEST(test_constructor1);
  RUN_TEST(test_copyConstrutcor);
}

