#include <Arduino.h>

#include <Debug.hpp>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include <unity.h>

#include <configItem.hpp>

struct  expectedConfigStruct {
  void *     pObj;
  const char *  cmd;
  const char *  str;
  uint32_t      p1;
  uint32_t      p2;
  uint32_t      p3;
  uint32_t      p4;
};

extern expectedConfigStruct sollEmptyConfig;

extern void test_empty_config(configItem * pScen);
extern void test_expected_config(expectedConfigStruct soll,configItem * pScen);


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//            configItem test                                              //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////



String configIn;
JsonDocument config;


void setup_configIn(void){
  configIn ="";
  configIn += " {                                                    \r\n";
  configIn += "     \"obj\"  : \"LED0\",                             \r\n";
  configIn += "     \"cmd\"  :    \"off\",                           \r\n";
  configIn += "     \"param\": [\"0x00FF00\",\"  2 \",\" 0 \",\"\"], \r\n";
  configIn += "     \"str\"  :  \"abc -# \",                         \r\n";
  configIn += " }                                         ";
}

expectedConfigStruct sollEmptyConfig = {NULL,"","",0,0,0,0};
expectedConfigStruct sollConfig      = {&ledCtrl1,"off","abc -# ",0xFF00,2,0,0};

void test_expected_config(expectedConfigStruct soll,configItem * pItem){
  TEST_ASSERT_EQUAL_PTR(soll.pObj       ,pItem->obj());
  TEST_ASSERT_EQUAL_STRING(soll.cmd     ,pItem->cmd().c_str());
  TEST_ASSERT_EQUAL_STRING(soll.str     ,pItem->cfg().str.c_str());
  TEST_ASSERT_EQUAL_UINT32(soll.p1      ,pItem->cfg().dimCfg.uint32);
  TEST_ASSERT_EQUAL_UINT32(soll.p2      ,pItem->cfg().colorCfg.uint32);
  TEST_ASSERT_EQUAL_UINT32(soll.p3      ,pItem->cfg().flashCfg.uint32);
  TEST_ASSERT_EQUAL_UINT32(soll.p4      ,pItem->cfg().breathCfg.uint32);
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




/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//            scenario test                                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
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


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//            collection test                                              //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////


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


expectedConfigStruct expectedScenario1[2]= {
  {&ledCtrl1,"off","",0,0,0,0},
  {&ledCtrl2,"off","",0,0,0,0}
};

expectedConfigStruct expectedScenario2[2]= {
  {&ledCtrl1,"on","",0,0,0,0},
  {&ledCtrl2,"on","",0,0,0,0}
};

void test_emptyConfig_collection(configItem item){
  TEST_ASSERT_EQUAL_PTR(NULL    ,item.obj());
  TEST_ASSERT_EQUAL_STRING(""   ,item.cmd().c_str());
  TEST_ASSERT_EQUAL_STRING(""   ,item.cfg().str.c_str());
  TEST_ASSERT_EQUAL_UINT32(0    ,item.cfg().dimCfg.uint32);
  TEST_ASSERT_EQUAL_UINT32(0    ,item.cfg().colorCfg.uint32);
  TEST_ASSERT_EQUAL_UINT32(0    ,item.cfg().flashCfg.uint32);
  TEST_ASSERT_EQUAL_UINT32(0    ,item.cfg().breathCfg.uint32);
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



// now we call here all test collections
int runAllTests(void) {
  UNITY_BEGIN();
  test_collection_configItem();
  test_collection_scenario();
  test_collection_collection();
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


