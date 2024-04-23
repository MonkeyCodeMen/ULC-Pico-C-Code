#include <Arduino.h>
#include <unity.h>

#include <configScenario.hpp>

String configScenarioIn;
JsonDocument scenario;


void setup_scenario_config(void){
  configScenarioIn ="";

  configScenarioIn += " {                                                    \r\n";
  configScenarioIn += "   \"name\"   : \"off\",                              \r\n";
  configScenarioIn += "   \"key\"    : \"A1\",                               \r\n";
  configScenarioIn += "   \"configs\": [                                     \r\n";


  configScenarioIn += "    {                                                 \r\n";
  configScenarioIn += "     \"obj\"  : \"LED0\",                             \r\n";
  configScenarioIn += "     \"cmd\"  :    \"off\",                           \r\n";
  configScenarioIn += "     \"param\": [\"0x00FF00\",\"  2 \",\" 0 \",\"\"], \r\n";
  configScenarioIn += "     \"str\"  :  \"abc -# \",                         \r\n";
  configScenarioIn += "     \"bin\":[0,2,255,40,30]                          \r\n";
  configScenarioIn += "    },                                                \r\n";
  configScenarioIn += "    {                                                 \r\n";
  configScenarioIn += "     \"obj\"  : \"LED1\",                             \r\n";
  configScenarioIn += "     \"cmd\"  :    \"off\",                           \r\n";
  configScenarioIn += "     \"param\": [\"0x00FF00\",\"  2 \",\" 0 \",\"\"], \r\n";
  configScenarioIn += "     \"str\"  :  \"def +* \",                         \r\n";
  configScenarioIn += "     \"bin\":[0,2,255,40,30]                          \r\n";
  configScenarioIn += "    },                                                \r\n";

  configScenarioIn += "    {                                                 \r\n";
  configScenarioIn += "     \"obj\"  : \"LED2\",                             \r\n";
  configScenarioIn += "     \"cmd\"  : \"off\",                              \r\n";
  configScenarioIn += "     \"param\": [],                                   \r\n";
  configScenarioIn += "     \"str\"  :  \"\"                                 \r\n";
  configScenarioIn += "    },                                                \r\n";

  configScenarioIn += "    {                                                 \r\n";
  configScenarioIn += "     \"obj\"  : \"LED3\",                             \r\n";
  configScenarioIn += "     \"cmd\"  :    \"off\"                            \r\n";
  configScenarioIn += "    },                                                \r\n";

  configScenarioIn += "    {                                                 \r\n";
  configScenarioIn += "     \"obj\"  : \"NEO0\",                             \r\n";
  configScenarioIn += "     \"cmd\"  :    \"off\"                            \r\n";
  configScenarioIn += "    },                                                \r\n";
  
  configScenarioIn += "    {                                                 \r\n";
  configScenarioIn += "     \"obj\"  : \"NEO9\",                             \r\n";
  configScenarioIn += "     \"cmd\"  :    \"off\"                            \r\n";
  configScenarioIn += "    }                                                 \r\n";

  configScenarioIn += "   ]                                                  \r\n";

  configScenarioIn += " }  \r\n ";


}

void test_emptyConfig_scenario(configItem item){
  TEST_ASSERT_EQUAL_PTR(NULL    ,item.obj());
  TEST_ASSERT_EQUAL_STRING(""   ,item.cmd().c_str());
  TEST_ASSERT_EQUAL_STRING(""   ,item.str().c_str());
  TEST_ASSERT_EQUAL_UINT32(0    ,item.param(0));
  TEST_ASSERT_EQUAL_UINT32(0    ,item.param(1));
  TEST_ASSERT_EQUAL_UINT32(0    ,item.param(2));
  TEST_ASSERT_EQUAL_UINT32(0    ,item.param(3));
  TEST_ASSERT_EQUAL_UINT32(0    ,item.size());
  // test out of range
  TEST_ASSERT_EQUAL_UINT32(0    ,item.param(4));
  TEST_ASSERT_EQUAL_UINT8(0     ,item.data(0));
}


void test_config0_scenario(configItem item){
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

void test_config1_scenario(configItem item){
  TEST_ASSERT_EQUAL_PTR(&ledCtrl2   ,item.obj());
  TEST_ASSERT_EQUAL_STRING("off"    ,item.cmd().c_str());
  TEST_ASSERT_EQUAL_STRING("def +* ",item.str().c_str());
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

void test_config2_scenario(configItem item){
  TEST_ASSERT_EQUAL_PTR(&ledCtrl3   ,item.obj());
  TEST_ASSERT_EQUAL_STRING("off"    ,item.cmd().c_str());
  TEST_ASSERT_EQUAL_STRING(""       ,item.str().c_str());
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(0));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(1));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(2));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(3));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.size());
}

void test_config3_scenario(configItem item){
  TEST_ASSERT_EQUAL_PTR(&ledCtrl4   ,item.obj());
  TEST_ASSERT_EQUAL_STRING("off"    ,item.cmd().c_str());
  TEST_ASSERT_EQUAL_STRING(""       ,item.str().c_str());
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(0));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(1));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(2));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(3));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.size());
}

void test_config4_scenario(configItem item){
  TEST_ASSERT_EQUAL_PTR(&neoStripeCtrl1 ,item.obj());
  TEST_ASSERT_EQUAL_STRING("off"    ,item.cmd().c_str());
  TEST_ASSERT_EQUAL_STRING(""       ,item.str().c_str());
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(0));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(1));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(2));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(3));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.size());
}

void test_config5_scenario(configItem item){
  TEST_ASSERT_EQUAL_PTR(NULL ,item.obj());
  TEST_ASSERT_EQUAL_STRING("off"    ,item.cmd().c_str());
  TEST_ASSERT_EQUAL_STRING(""       ,item.str().c_str());
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(0));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(1));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(2));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.param(3));
  TEST_ASSERT_EQUAL_UINT32(0        ,item.size());
}


// in include files first define tests :
void test_decode_scenario(void) {
  DeserializationError error = deserializeJson(scenario, configScenarioIn);
  TEST_ASSERT_FALSE_MESSAGE((error),"deserializeJson() returned error :");
  if (error){
    TEST_MESSAGE(error.c_str());
  }

}


void test_baseConstructor_scenario(void){
  configScenario item;
  
  TEST_ASSERT_EQUAL_STRING(""    ,item.name().c_str());
  TEST_ASSERT_EQUAL_UINT32((uint32_t)  EVENT_NONE ,(uint32_t)item.event());
  TEST_ASSERT_EQUAL_UINT32(0  ,item.count());
  test_emptyConfig_scenario(item.config(0));
}

void test_constructor_scenario(void){
  configScenario item(scenario);
  
  TEST_ASSERT_EQUAL_STRING("off"    ,item.name().c_str());
  TEST_ASSERT_EQUAL_UINT32((uint32_t)  EVENT_A1 ,(uint32_t)item.event());
  TEST_ASSERT_EQUAL_UINT32(6  ,item.count());
  test_config0_scenario(item.config(0));
  test_config1_scenario(item.config(1));
  test_config2_scenario(item.config(2));
  test_config3_scenario(item.config(3));
  test_config4_scenario(item.config(4));
  test_config5_scenario(item.config(5));
  test_emptyConfig_scenario(item.config(6));

}

void test_copyConstructor_scenario(void){
  configScenario src(scenario);
  configScenario item(src);

  TEST_ASSERT_EQUAL_STRING("off"    ,item.name().c_str());
  TEST_ASSERT_EQUAL_UINT32((uint32_t)  EVENT_A1 ,(uint32_t)item.event());
  TEST_ASSERT_EQUAL_UINT32(6  ,item.count());
  test_config0_scenario(item.config(0));
  test_config1_scenario(item.config(1));
  test_config2_scenario(item.config(2));
  test_config3_scenario(item.config(3));
  test_config4_scenario(item.config(4));
  test_config5_scenario(item.config(5));
  test_emptyConfig_scenario(item.config(6));
}

void test_assign_scenario(void){
  configScenario src(scenario);
  configScenario item;

  TEST_ASSERT_EQUAL_STRING(""    ,item.name().c_str());
  TEST_ASSERT_EQUAL_UINT32((uint32_t)  EVENT_NONE ,(uint32_t)item.event());
  TEST_ASSERT_EQUAL_UINT32(0  ,item.count());
  test_emptyConfig_scenario(item.config(0));

  item = src;

  TEST_ASSERT_EQUAL_STRING("off"    ,item.name().c_str());
  TEST_ASSERT_EQUAL_UINT32((uint32_t)  EVENT_A1 ,(uint32_t)item.event());
  TEST_ASSERT_EQUAL_UINT32(6  ,item.count());
  test_config0_scenario(item.config(0));
  test_config1_scenario(item.config(1));
  test_config2_scenario(item.config(2));
  test_config3_scenario(item.config(3));
  test_config4_scenario(item.config(4));
  test_config5_scenario(item.config(5));
  test_emptyConfig_scenario(item.config(6));
}


void test_collection_scenario(void) {

  setup_scenario_config();
  RUN_TEST(test_decode_scenario);
  RUN_TEST(test_baseConstructor_scenario);
  RUN_TEST(test_constructor_scenario);
  RUN_TEST(test_copyConstructor_scenario);
  RUN_TEST(test_assign_scenario);
}

