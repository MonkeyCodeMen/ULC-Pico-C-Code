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


void test_function_threeEntries(void) {
  char list[]="entry1,Entry-2,Entry  3";
  StringList object(list,',');
  String res; 
  
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("entry1")));
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("Entry-2")));
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("Entry  3")));
  TEST_ASSERT_TRUE(object.isEndReached() == true);

  object.rewind();

  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("entry1")));
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("Entry-2")));
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("Entry  3")));
  TEST_ASSERT_TRUE(object.isEndReached() == true);
}

void test_function_fourEntries(void) {
  char list[]="entry1,Entry-2,Entry  3,";
  StringList object(list,',');
  String res; 
  
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("entry1")));
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("Entry-2")));
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("Entry  3")));
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("")));
  TEST_ASSERT_TRUE(object.isEndReached() == true);
}

void test_function_fourEntries_X(void) {
  char list[]="entry1XEntry-2XEntry  3X";
  StringList object(list,'X');
  String res; 
  
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("entry1")));
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("Entry-2")));
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("Entry  3")));
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_TRUE(object.getNextListEntry() == String(F("")));
  TEST_ASSERT_TRUE(object.isEndReached() == true);
}

// collect all tests of this file to one collection
void test_collection_StringList(void) {
  RUN_TEST(test_function_empytString);
  RUN_TEST(test_function_emptyList);
  RUN_TEST(test_function_threeEntries);
  RUN_TEST(test_function_fourEntries);
  RUN_TEST(test_function_fourEntries_X);
}
