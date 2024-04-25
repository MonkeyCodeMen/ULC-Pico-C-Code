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

#include "expectedConfigType.h"
expectedConfigStruct sollEmptyConfig = {NULL,"","",0,0,0,0,0,{0,0,0,0,0}};
expectedConfigStruct sollConfig      = {&ledCtrl1,"off","abc -# ",0xFF00,2,0,0,5,{0,2,255,40,30}};

void test_expected_config(expectedConfigStruct soll,configItem * pItem){
  TEST_ASSERT_EQUAL_PTR(soll.pObj       ,pItem->obj());
  TEST_ASSERT_EQUAL_STRING(soll.cmd     ,pItem->cmd().c_str());
  TEST_ASSERT_EQUAL_STRING(soll.str     ,pItem->str().c_str());
  TEST_ASSERT_EQUAL_UINT32(soll.p1      ,pItem->param(0));
  TEST_ASSERT_EQUAL_UINT32(soll.p2      ,pItem->param(1));
  TEST_ASSERT_EQUAL_UINT32(soll.p3      ,pItem->param(2));
  TEST_ASSERT_EQUAL_UINT32(soll.p4      ,pItem->param(3));
  TEST_ASSERT_EQUAL_UINT32(soll.size    ,pItem->size());
  for(int i=0;i < soll.size;i++){
    TEST_ASSERT_EQUAL_UINT8(soll.data[i]  ,pItem->data(i));
  }
  // test out of range  
  TEST_ASSERT_EQUAL_UINT8(0  ,pItem->data(soll.size));
  TEST_ASSERT_EQUAL_UINT32(0 ,pItem->param(4));
}

void test_empty_config(configItem * pItem){
  test_expected_config(sollEmptyConfig,pItem);
}


// in include files first define tests :
void test_decode_config(void) {
  DeserializationError error = deserializeJson(config, configIn);
  TEST_ASSERT_FALSE_MESSAGE((error),"deserializeJson() returned error :");
  if (error){
    TEST_MESSAGE(error.c_str());
  }

}

void test_baseConstructor_config(void){
  configItem item;
  test_empty_config(&item);
}

void test_constructor_config(void){
  configItem item(config);
  test_expected_config(sollConfig,&item);
  
}

void test_copyOperations_config(void){
  configItem src(config);
  test_expected_config(sollConfig,&src);

  configItem item(src);
  test_expected_config(sollConfig,&item);

  configItem temp;
  test_empty_config(&temp);
  
  item = temp;
  test_empty_config(&item);
  test_expected_config(sollConfig,&src);
}


void test_collection_configItem(void) {

  setup_configIn();
  RUN_TEST(test_decode_config);
  RUN_TEST(test_baseConstructor_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_constructor_config);
  RUN_TEST(test_copyOperations_config);
}

