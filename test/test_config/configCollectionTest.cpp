#include <Arduino.h>
#include <unity.h>

#include <configCollection.hpp>

JsonDocument collectionConfig;
String collectionConfigStr;


/* {
 *      "project"     : "ULC pico : universal light controller raspberry prico"
 *      "type"        : "scenario configuration file"
 *      "date"        : "24.12.2024"
 *      "author"      : "MonkeyCodeMen"
 *      "version"     : "1.2.3"
 *      "scenarios":[
 *          { configScenario },
 *          { configScenario },
 *          { configScenario },
 *          { configScenario },
 *          { configScenario },
 *          { configScenario }
 *       ]
 * }
 */
 
String collection_config(void){
  String res="";
  
  res += "{";

  res += "   \"project\"   : \"test project\",                  \r\n";
  res += "   \"type\"      : \"config collection\",             \r\n";
  res += "   \"date\"      : \"yesterday\",                     \r\n";
  res += "   \"author\"    : \"MonkeyCodeMen\",                 \r\n";
  res += "   \"version\"   : \"1.2.3\",                         \r\n";
  res += "   \"scenarios\" :[                                   \r\n ";

  res += " {                                                    \r\n";
  res += "   \"name\"   : \"off\",                              \r\n";
  res += "   \"key\"    : \"A1\",                               \r\n";
  res += "   \"configs\": [                                     \r\n";
  res += "    {                                                 \r\n";
  res += "     \"obj\"  : \"LED0\",                             \r\n";
  res += "     \"cmd\"  : \"off\"                               \r\n";
  res += "    },                                                \r\n";
  res += "    {                                                 \r\n";
  res += "     \"obj\"  : \"LED1\",                             \r\n";
  res += "     \"cmd\"  : \"off\"                               \r\n";
  res += "    } ]                                                \r\n";
  res += " },                                                   \r\n";

  res += " {                                                    \r\n";
  res += "   \"name\"   : \"on\",                               \r\n";
  res += "   \"key\"    : \"A2\",                               \r\n";
  res += "   \"configs\": [                                     \r\n";
  res += "    {                                                 \r\n";
  res += "     \"obj\"  : \"LED0\",                             \r\n";
  res += "     \"cmd\"  : \"on\"                                \r\n";
  res += "    },                                                \r\n";
  res += "    {                                                 \r\n";
  res += "     \"obj\"  : \"LED1\",                             \r\n";
  res += "     \"cmd\"  : \"on\"                                \r\n";
  res += "    }]                                                 \r\n";
  res += " } ]                                                   \r\n";

  res += "}";

  TEST_ASSERT_TRUE(res.length() > 100);

  return res;
}


#include "expectedConfigType.h"

expectedConfigStruct expectedScenario1[2]= {
  {&ledCtrl1,"off","",0,0,0,0,0,{0,0,0,0,0}},
  {&ledCtrl2,"off","",0,0,0,0,0,{0,0,0,0,0}}
};

expectedConfigStruct expectedScenario2[2]= {
  {&ledCtrl1,"on","",0,0,0,0,0,{0,0,0,0,0}},
  {&ledCtrl2,"on","",0,0,0,0,0,{0,0,0,0,0}}
};

void test_emptyConfig_collection(configItem item){
  TEST_ASSERT_EQUAL_PTR(NULL    ,item.obj());
  TEST_ASSERT_EQUAL_STRING(""   ,item.cmd().c_str());
  TEST_ASSERT_EQUAL_STRING(""   ,item.str().c_str());
  TEST_ASSERT_EQUAL_UINT32(0    ,item.param(0));
  TEST_ASSERT_EQUAL_UINT32(0    ,item.param(1));
  TEST_ASSERT_EQUAL_UINT32(0    ,item.param(2));
  TEST_ASSERT_EQUAL_UINT32(0    ,item.param(3));
  TEST_ASSERT_EQUAL_UINT32(0    ,item.size());
}

void test_emptyScenrio_collection(configScenario item){
  TEST_ASSERT_EQUAL_STRING("",item.name().c_str());
  TEST_ASSERT_EQUAL_UINT32((uint32_t)  EVENT_NONE ,(uint32_t)item.event());
  TEST_ASSERT_EQUAL_UINT32(0  ,item.count());
  test_emptyConfig_collection(item.config(0));
}



void test_baseConstructor_collection(void){
  configCollection item;
  
  TEST_ASSERT_EQUAL_STRING("",item.project().c_str());
  TEST_ASSERT_EQUAL_STRING("",item.type().c_str());
  TEST_ASSERT_EQUAL_STRING("",item.date().c_str());
  TEST_ASSERT_EQUAL_STRING("",item.author().c_str());
  TEST_ASSERT_EQUAL_STRING("",item.version().c_str());
  test_emptyScenrio_collection(item.scenario(EVENT_A1));
}

void test_constructor_collection(void){
  configCollection collection;
  TEST_ASSERT_TRUE(collection.openFromString(collectionConfigStr) );

  TEST_ASSERT_EQUAL_STRING("test project"       ,collection.project().c_str());
  TEST_ASSERT_EQUAL_STRING("config collection"  ,collection.type().c_str());
  TEST_ASSERT_EQUAL_STRING("yesterday"          ,collection.date().c_str());
  TEST_ASSERT_EQUAL_STRING("MonkeyCodeMen"      ,collection.author().c_str());
  TEST_ASSERT_EQUAL_STRING("1.2.3"              ,collection.version().c_str());
  TEST_ASSERT_EQUAL_UINT32(2                    ,collection.count());

  configScenario scenario;
  int i;
  
  scenario = collection.scenario(EVENT_A1);
  TEST_ASSERT_EQUAL_STRING("off"    ,scenario.name().c_str());
  TEST_ASSERT_EQUAL_UINT32(EVENT_A1 ,scenario.event());
  TEST_ASSERT_EQUAL_UINT32(2        ,scenario.count());
  for(i=0;i < 2;i++){
    configItem cfg = scenario.config(i);
    test_expected_config(expectedScenario1[i],&cfg);
  }

  scenario = collection.scenario(0);
  TEST_ASSERT_EQUAL_STRING("off"    ,scenario.name().c_str());
  TEST_ASSERT_EQUAL_UINT32(EVENT_A1 ,scenario.event());
  TEST_ASSERT_EQUAL_UINT32(2        ,scenario.count());
  for(i=0;i < 2;i++){
    configItem cfg = scenario.config(i);
    test_expected_config(expectedScenario1[i],&cfg);
  }

  scenario = collection.scenario(EVENT_A2);
  TEST_ASSERT_EQUAL_STRING("on"     ,scenario.name().c_str());
  TEST_ASSERT_EQUAL_UINT32(EVENT_A2 ,scenario.event());
  TEST_ASSERT_EQUAL_UINT32(2        ,scenario.count());
  for(i=0;i < 2;i++){
    configItem cfg = scenario.config(i);
    test_expected_config(expectedScenario2[i],&cfg);
  }

  scenario = collection.scenario(1);
  TEST_ASSERT_EQUAL_STRING("on"     ,scenario.name().c_str());
  TEST_ASSERT_EQUAL_UINT32(EVENT_A2 ,scenario.event());
  TEST_ASSERT_EQUAL_UINT32(2        ,scenario.count());
  for(i=0;i < 2;i++){
    configItem cfg = scenario.config(i);
    test_expected_config(expectedScenario2[i],&cfg);
  }

  scenario = collection.scenario(EVENT_A3);
  test_emptyScenrio_collection(scenario);

  scenario = collection.scenario(2);
  test_emptyScenrio_collection(scenario);
}



void test_collection_collection(void) {

  collectionConfigStr = collection_config();
  RUN_TEST(test_baseConstructor_collection);
  RUN_TEST(test_constructor_collection);
  collectionConfigStr = "";
}

