#include <Arduino.h>

#include <Debug.hpp>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include <MainConfig.h>
#include <SDcard.hpp>
#include <LedObjects.hpp>
#include <Ctrl.hpp>
#include <unity.h>





void test_Ctrl_constructor0(void) {

  Ctrl * pObject;

  pObject = new Ctrl();
  TEST_ASSERT_TRUE(pObject != NULL);
  TEST_ASSERT_EQUAL_UINT32(0,pObject->getAniCount());


  // destructor Ctrl
}

void test_Ctrl_emptyList(void){
  Ctrl object;

  TEST_ASSERT_EQUAL_UINT32(0,object.getAniCount());
  TEST_ASSERT_EQUAL_STRING("",object.getNameList());
  TEST_ASSERT_EQUAL_STRING("",object.getName());
  TEST_ASSERT_EQUAL_UINT32(0,object.getAniCount());
  TEST_ASSERT_FALSE(object.isAniSelected());
}

void test_Ctrl_changeByIndex(void){
  Ctrl object;

  // change to not existing animation
  TEST_ASSERT_EQUAL_INT(ANI_ERROR_PROGRAM_DOES_NOT_EXIST,object.select(3));  
  TEST_ASSERT_EQUAL_UINT32(0,object.getAniCount());
  TEST_ASSERT_EQUAL_STRING("",object.getNameList());
  TEST_ASSERT_EQUAL_STRING("",object.getName());
}


void test_Ctrl_changeByName(void){
  Ctrl object;

  // change to not existing animation
  TEST_ASSERT_EQUAL_INT(ANI_ERROR_PROGRAM_DOES_NOT_EXIST,object.select("Roger Rabbit"));  
  TEST_ASSERT_EQUAL_UINT32(0,object.getAniCount());
  TEST_ASSERT_EQUAL_STRING("",object.getNameList());
  TEST_ASSERT_EQUAL_STRING("",object.getName());
}


void test_Ctrl_setup(void) {
  Ctrl object;

  // try parameter change on not existing animation
  TEST_ASSERT_EQUAL_INT(ANI_ERROR_PROGRAM_DOES_NOT_EXIST,object.config(AniCfg(0,0,0,0,"")));  
  TEST_ASSERT_EQUAL_UINT32(0,object.getAniCount());
  TEST_ASSERT_EQUAL_STRING("",object.getNameList());
  TEST_ASSERT_EQUAL_STRING("",object.getName());
  
}








void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}




// now we call here all test collections
int runAllTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_Ctrl_constructor0);
  RUN_TEST(test_Ctrl_emptyList);
  RUN_TEST(test_Ctrl_changeByName);
  RUN_TEST(test_Ctrl_changeByIndex);
  RUN_TEST(test_Ctrl_setup);
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
