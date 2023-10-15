#include <Arduino.h>
#include "unity.h"


#include <Debug.hpp>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <cube.hpp>
#include <WS2812FX.h>
#include <RgbLedCtrl.hpp>
#include <LedCtrl.hpp>
#include <NeoStripeCtrl.hpp>
#include <NeoMatrixCtrl.hpp>
#include "Com.hpp"

// prototype here all external test collections 
void test_collection_StringList();




void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}


// in include files first define tests :
void test_function_simpleTest1(void) {
  TEST_ASSERT_TRUE(true == true);
}
void test_function_simpleTest2(void) {
  TEST_ASSERT_TRUE(false == false);
}
void test_function_simpleTest3(void) {
  TEST_ASSERT_FALSE(true == false);
}

// then a collection function:
void test_collection_simple(void){
  RUN_TEST(test_function_simpleTest1);
  RUN_TEST(test_function_simpleTest2);
  RUN_TEST(test_function_simpleTest3);
}

// now we call here all test collections
int runAllCollections(void) {
  UNITY_BEGIN();
  test_collection_simple();
  test_collection_StringList();
  return UNITY_END();
}



/**
  * For Arduino framework
  */
void setup() {
  // Wait ~2 seconds before the Unity test runner
  // establishes connection with a board Serial interface
  delay(2000);

  /*  
  in case of issues with test results (outputs) 
  try the next commands to catch on a simple ternminal
  Serial1.begin(115200);
  Serial1.println(" serial 1 begin 115200 ");
  */
  
  
  runAllCollections();
}
void loop() {}
