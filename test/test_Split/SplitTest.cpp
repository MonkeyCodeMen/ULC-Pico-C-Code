
/*
 * MIT License
 * 
 * Copyright (c) 2024 MonkeyCodeMen@GitHub
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * provided to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <Arduino.h>
#include <Debug.hpp>
#include <helper.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include <unity.h>



#include "Split.hpp"

void test_Split_empytString(void) {
  char list[]="";
  Split object(list,',');
  String res; 
  
  TEST_ASSERT_TRUE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached());

  object.rewind();

  TEST_ASSERT_TRUE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached());

}

void test_Split_emptyList(void) {
  char list[]=",";
  Split object(list,',');
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


void test_Split_threeEntries(void) {
  char list[]="entry1,Entry-2,Entry  3";
  Split object(list,',');
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

void test_Split_fourEntries(void) {
  char list[]="entry1,Entry-2,Entry  3,";
  Split object(list,',');
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

void test_Split_fourEntries_X(void) {
  char list[]="entry1XEntry-2XEntry  3X";
  Split object(list,'X');
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



void test_Split_empytStringStrSep(void) {
  char list[]="";
  Split object(list,(char*)"###");
  String res; 
  
  TEST_ASSERT_TRUE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached());

  object.rewind();

  TEST_ASSERT_TRUE(object.isEndReached());
  TEST_ASSERT_EQUAL_STRING("" , object.getNextListEntry().c_str());
  TEST_ASSERT_TRUE(object.isEndReached());

}

void test_Split_emptyListStrSep(void) {
  char list[]="~&~";
  Split object(list,(char*)"~&~");
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


void test_Split_threeEntriesStrSep(void) {
  char list[]="entry1~~Entry-2~~Entry  3";
  Split object(list,(char*)"~~");
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


void test_Split_stringList(void) {
  String list="entry1,Entry-2,Entry  3";
  Split object(list,',');
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



void test_Split_stringListStringSep(void) {
  String list="entry1~~Entry-2~~Entry  3";
  String Sep="~~";
  Split object(list,Sep);
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

// collect all tests of this file to one collection
void runAllTests(void) {
  UNITY_BEGIN();
  
  RUN_TEST(test_Split_empytString);
  RUN_TEST(test_Split_emptyList);
  RUN_TEST(test_Split_threeEntries);
  RUN_TEST(test_Split_fourEntries);
  RUN_TEST(test_Split_fourEntries_X);

  RUN_TEST(test_Split_empytStringStrSep);
  RUN_TEST(test_Split_emptyListStrSep);
  RUN_TEST(test_Split_threeEntriesStrSep);

  RUN_TEST(test_Split_stringList);
  RUN_TEST(test_Split_stringListStringSep);

  UNITY_END();
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
