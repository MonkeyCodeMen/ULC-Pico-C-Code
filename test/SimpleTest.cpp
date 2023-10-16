#include <Arduino.h>
#include "unity.h"


// in include files first define tests :
void test_simple_Test1(void) {
  TEST_ASSERT_TRUE(true == true);
}
void test_simple_Test2(void) {
  TEST_ASSERT_TRUE(false == false);
}
void test_simple_Test3(void) {
  TEST_ASSERT_FALSE(true == false);
}

void test_collection_SimpleTest(void) {
  RUN_TEST(test_simple_Test1);
  RUN_TEST(test_simple_Test2);
  RUN_TEST(test_simple_Test3);
}

