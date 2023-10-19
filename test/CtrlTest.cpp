#include <Arduino.h>

#include "Ctrl.hpp"
#include "StringList.hpp"

#include "unity.h"




void test_Ctrl_constructor(void) {

  Ctrl object;

  TEST_ASSERT_EQUAL_UINT32(0,object.getAniCount());
  TEST_ASSERT_EQUAL_STRING("",object.getNameList().c_str());
  TEST_ASSERT_EQUAL_STRING("",object.getName().c_str());
  
  object.setup(3);  // change to not existing animation
  TEST_ASSERT_EQUAL_UINT32(0,object.getAniCount());
  TEST_ASSERT_EQUAL_STRING("",object.getNameList().c_str());
  TEST_ASSERT_EQUAL_STRING("",object.getName().c_str());

  object.setup("who knows");  // change to not existing animation
  TEST_ASSERT_EQUAL_UINT32(0,object.getAniCount());
  TEST_ASSERT_EQUAL_STRING("",object.getNameList().c_str());
  TEST_ASSERT_EQUAL_STRING("",object.getName().c_str());

  object.setup(0,0,0,0,0,NULL);  // try parameter change on not existing animation
  TEST_ASSERT_EQUAL_UINT32(0,object.getAniCount());
  TEST_ASSERT_EQUAL_STRING("",object.getNameList().c_str());
  TEST_ASSERT_EQUAL_STRING("",object.getName().c_str());
}


// collect all tests of this file to one collection
void test_collection_Ctrl(void) {
  RUN_TEST(test_Ctrl_constructor);
}
