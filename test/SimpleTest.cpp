#include <Arduino.h>
#include "unity.h"


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

void test_collection_SimpleTest(void) {
  RUN_TEST(test_function_simpleTest1);
  RUN_TEST(test_function_simpleTest2);
  RUN_TEST(test_function_simpleTest3);
}

