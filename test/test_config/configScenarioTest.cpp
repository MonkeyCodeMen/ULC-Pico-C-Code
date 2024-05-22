#include <Arduino.h>
#include <unity.h>

#include <configScenario.hpp>

JsonDocument scenario;
String scenarioConfig;

void test_empty_config(configItem * pConf);

String scenario_config(void){
  String res="";
  res += " {                                                    \r\n";
  res += "   \"name\"   : \"off\",                              \r\n";
  res += "   \"key\"    : \"A1\",                               \r\n";
  res += "   \"configs\": [                                     \r\n";


  res += "    {                                                 \r\n";
  res += "     \"obj\"  : \"LED0\",                             \r\n";
  res += "     \"cmd\"  :    \"off\",                           \r\n";
  res += "     \"param\": [\"0x00FF00\",\"  2 \",\" 0 \",\"\"], \r\n";
  res += "     \"str\"  :  \"abc -# \",                         \r\n";
  res += "    },                                                \r\n";
  res += "    {                                                 \r\n";
  res += "     \"obj\"  : \"LED1\",                             \r\n";
  res += "     \"cmd\"  :    \"off\",                           \r\n";
  res += "     \"param\": [\"0x00FF00\",\"  2 \",\" 0 \",\"\"], \r\n";
  res += "     \"str\"  :  \"def +* \",                         \r\n";
  res += "    },                                                \r\n";

  res += "    {                                                 \r\n";
  res += "     \"obj\"  : \"LED2\",                             \r\n";
  res += "     \"cmd\"  : \"off\",                              \r\n";
  res += "     \"param\": [],                                   \r\n";
  res += "     \"str\"  :  \"\"                                 \r\n";
  res += "    },                                                \r\n";

  res += "    {                                                 \r\n";
  res += "     \"obj\"  : \"LED3\",                             \r\n";
  res += "     \"cmd\"  :    \"off\"                            \r\n";
  res += "    },                                                \r\n";

  res += "    {                                                 \r\n";
  res += "     \"obj\"  : \"NEO0\",                             \r\n";
  res += "     \"cmd\"  :    \"off\"                            \r\n";
  res += "    },                                                \r\n";
  
  res += "    {                                                 \r\n";
  res += "     \"obj\"  : \"NEO9\",                             \r\n";
  res += "     \"cmd\"  :    \"off\"                            \r\n";
  res += "    }                                                 \r\n";

  res += "   ]                                                  \r\n";

  res += " }  \r\n ";

  return res;
}

#include "expectedConfigType.h"

expectedConfigStruct expectedConfigs[6]= {
  {&ledCtrl1,"off","abc -# ",0xFF00,2,0,0},
  {&ledCtrl2,"off","def +* ",0xFF00,2,0,0},
  {&ledCtrl3,"off","",0,0,0,0},
  {&ledCtrl4,"off","",0,0,0,0},
  {&neoStripeCtrl1,"off","",0,0,0,0},
  {NULL,"off","",0,0,0,0}
};

void test_empty_scenario(configScenario * pScen){
  TEST_ASSERT_EQUAL_STRING("",pScen->name().c_str());
  TEST_ASSERT_EQUAL_UINT32((uint32_t)  EVENT_NONE ,(uint32_t)pScen->event());
  TEST_ASSERT_EQUAL_UINT32(0  ,pScen->count());
  configItem cfg = pScen->config(0);
  test_empty_config(&cfg); 
}




void test_expected_scenario(configScenario * pScen){
  TEST_ASSERT_EQUAL_STRING("off"    ,pScen->name().c_str());
  TEST_ASSERT_EQUAL_UINT32(EVENT_A1 ,pScen->event());
  TEST_ASSERT_EQUAL_UINT32(6        ,pScen->count());
  configItem cfg;
  for (int i=0; i < 6;i++){
    cfg = pScen->config(i);
    test_expected_config(expectedConfigs[i],&cfg);
  }
  cfg = pScen->config(6);
  test_empty_config(&cfg);
  
}


// in include files first define tests :
void test_decode_scenario(void) {
  DeserializationError error = deserializeJson(scenario, scenarioConfig);
  TEST_ASSERT_FALSE_MESSAGE((error),"deserializeJson() returned error :");
  if (error){
    TEST_MESSAGE(error.c_str());
  }

}


void test_baseConstructor_scenario(void){
  configScenario item;
  test_empty_scenario(&item);
}

void test_constructor_scenario(void){
  configScenario item(scenario);
  test_expected_scenario(&item);
}

void test_copyConstructor_scenario(void){
  configScenario src(scenario);
  test_expected_scenario(&src);
  
  configScenario item(src);
  test_expected_scenario(&item);
}

void test_assign_scenario(void){
  configScenario src(scenario);
  test_expected_scenario(&src);

  configScenario item;
  test_empty_scenario(&item);

  item = src;
  test_expected_scenario(&item);

  configScenario temp;
  test_empty_scenario(&temp);

  src = temp;
  test_empty_scenario(&temp);
  test_empty_scenario(&src);
  test_expected_scenario(&item);
}


void test_collection_scenario(void) {

  scenarioConfig = scenario_config();
  RUN_TEST(test_decode_scenario);   
  RUN_TEST(test_baseConstructor_scenario);
  RUN_TEST(test_constructor_scenario);
  RUN_TEST(test_assign_scenario);
  RUN_TEST(test_copyConstructor_scenario);
  scenarioConfig = "";
}

