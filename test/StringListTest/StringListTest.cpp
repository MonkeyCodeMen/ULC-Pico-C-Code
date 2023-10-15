#include "StringList.hpp"
#include "unity.h"

void test_function_empytString(void) {
  char list[]="";
  StringList object(list,',');
  String res; 
  
  TEST_ASSERT_TRUE(object.isEndReached() == true);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("")));
  TEST_ASSERT_TRUE(object.isEndReached() == true);

  object.rewind();

  TEST_ASSERT_TRUE(object.isEndReached() == true);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("")));
  TEST_ASSERT_TRUE(object.isEndReached() == true);

}

void test_function_emptyList(void) {
  char list[]=",";
  StringList object(list,',');
  String res; 
  
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("")));
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("")));
  TEST_ASSERT_TRUE(object.isEndReached() == true);

  object.rewind();

  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("")));
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("")));
  TEST_ASSERT_TRUE(object.isEndReached() == true);

}


// collect all tests of this file to one collection
void test_collection_StringList(void) {
  RUN_TEST(test_function_empytString);
  RUN_TEST(test_function_emptyList);
}
