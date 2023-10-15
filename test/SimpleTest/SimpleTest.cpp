#include <Arduino.h>
#include "unity.h"

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

int runSimpleTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_function_simpleTest1);
  RUN_TEST(test_function_simpleTest2);
  RUN_TEST(test_function_simpleTest3);
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
  
  
  runSimpleTests();
}
void loop() {}
