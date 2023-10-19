#include <Arduino.h>

#include <Debug.hpp>
#include <helper.hpp>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include "Ctrl.hpp"
#include <unity.h>


void test_Ctrl_constructor0(void) {

  Ctrl * pObject;

  pObject = new Ctrl();
  TEST_ASSERT_TRUE(pObject != NULL);
  TEST_ASSERT_EQUAL_UINT32(0,pObject->getAniCount());


  // destructor Ctrl
}


void test_Ctrl_constructor1(void) {

  Ctrl object;

  TEST_ASSERT_EQUAL_UINT32(0,object.getAniCount());
  TEST_ASSERT_EQUAL_STRING("",object.getNameList());
  TEST_ASSERT_EQUAL_STRING("",object.getName());
  
  object.setup(3);  // change to not existing animation
  TEST_ASSERT_EQUAL_UINT32(0,object.getAniCount());
  TEST_ASSERT_EQUAL_STRING("",object.getNameList());
  TEST_ASSERT_EQUAL_STRING("",object.getName());

  object.setup("who knows");  // change to not existing animation
  TEST_ASSERT_EQUAL_UINT32(0,object.getAniCount());
  TEST_ASSERT_EQUAL_STRING("",object.getNameList());
  TEST_ASSERT_EQUAL_STRING("",object.getName());

  object.setup(0,0,0,0,0,NULL);  // try parameter change on not existing animation
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
int runAllCollections(void) {
  UNITY_BEGIN();
  RUN_TEST(test_Ctrl_constructor0);
  RUN_TEST(test_Ctrl_constructor1);
  return UNITY_END();
}



/**
  * For Arduino framework
  */
void setup() {
  // Wait ~2 seconds before the Unity test runner
  // establishes connection with a board Serial interface
  delay(2000);
  
  runAllCollections();

  pinMode(LED_BUILTIN,OUTPUT);
}


void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);

}

