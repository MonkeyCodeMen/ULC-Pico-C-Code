#include "StringList.hpp"
#include "unity.h"

void test_StringList_empytString(void) {
  char list[]="";
  StringList object(list,',');
  String res; 
  
  TEST_ASSERT_TRUE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached());

  object.rewind();

  TEST_ASSERT_TRUE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached());

}

void test_StringList_emptyList(void) {
  char list[]=",";
  StringList object(list,',');
  String res; 
  
  TEST_ASSERT_FALSE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("" , object.getNextListEntry().c_str());
  TEST_ASSERT_FALSE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached());

  object.rewind();

  TEST_ASSERT_FALSE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("" , object.getNextListEntry().c_str());
  TEST_ASSERT_FALSE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached());
}


void test_StringList_threeEntries(void) {
  char list[]="entry1,Entry-2,Entry  3";
  StringList object(list,',');
  String res; 
  
  TEST_ASSERT_FALSE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("entry1" , object.getNextListEntry().c_str());
  TEST_ASSERT_FALSE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("Entry-2" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_EQUAL_STRING("Entry  3" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached());

  object.rewind();

  TEST_ASSERT_FALSE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("entry1" , object.getNextListEntry().c_str());
  TEST_ASSERT_FALSE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("Entry-2" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_EQUAL_STRING("Entry  3" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached());
}

void test_StringList_fourEntries(void) {
  char list[]="entry1,Entry-2,Entry  3,";
  StringList object(list,',');
  String res; 
  
  TEST_ASSERT_FALSE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("entry1" , object.getNextListEntry().c_str());
  TEST_ASSERT_FALSE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("Entry-2" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_EQUAL_STRING("Entry  3" , object.getNextListEntry().c_str());
  TEST_ASSERT_FALSE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached());
}

void test_StringList_fourEntries_X(void) {
  char list[]="entry1XEntry-2XEntry  3X";
  StringList object(list,'X');
  String res; 
  
  TEST_ASSERT_FALSE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("entry1" , object.getNextListEntry().c_str());
  TEST_ASSERT_FALSE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("Entry-2" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached() == false);
  TEST_ASSERT_EQUAL_STRING("Entry  3" , object.getNextListEntry().c_str());
  TEST_ASSERT_FALSE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached());
}

// collect all tests of this file to one collection
void test_collection_StringList(void) {
  RUN_TEST(test_StringList_empytString);
  RUN_TEST(test_StringList_emptyList);
  RUN_TEST(test_StringList_threeEntries);
  RUN_TEST(test_StringList_fourEntries);
  RUN_TEST(test_StringList_fourEntries_X);
}
