#include <Arduino.h>

#include <Debug.hpp>
#include <helper.hpp>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include "Led.hpp"
#include "LedCtrl.hpp"
#include "StringList.hpp"

#include "unity.h"

void test_LedCtrl_constructor(void) {
  // create Sim LED object and test it
  Led *pSimLed;
  pSimLed = new Led();
  TEST_ASSERT_TRUE( pSimLed != NULL);
  TEST_ASSERT_EQUAL_UINT8( 0 , pSimLed->get());

  // now let's create a LedCtrl object
  LedCtrl *pObject;
  pObject = new LedCtrl(pSimLed);
  TEST_ASSERT_TRUE( pObject != NULL);
  delete pObject;
  TEST_ASSERT_TRUE( pSimLed != NULL);
  TEST_ASSERT_EQUAL_UINT8( 0 , pSimLed->get()); // Ctrl object should not haved touched LED (loop not called)

}

void test_LedCtrl_init(void) {
  // create Sim LED object and test it
  Led *pSimLed;
  pSimLed = new Led();
  TEST_ASSERT_TRUE( pSimLed != NULL);
  TEST_ASSERT_EQUAL_UINT8( 0 , pSimLed->get());

  // now let's create a LedCtrl object
  LedCtrl *pObject;
  pObject = new LedCtrl(pSimLed);
  TEST_ASSERT_TRUE( pObject != NULL);
  TEST_ASSERT_TRUE( pSimLed != NULL);
  TEST_ASSERT_EQUAL_UINT8( 0 , pSimLed->get()); // Ctrl object should not haved touched LED (loop not called)

  // check sim mode & value
  TEST_ASSERT_EQUAL_STRING("off",pObject->getName());
}

void test_LedCtrl_aniList(void) {
  // create Sim LED object and test it
  Led *pSimLed;
  pSimLed = new Led(LED_BUILTIN,true);
  TEST_ASSERT_TRUE( pSimLed != NULL);
  TEST_ASSERT_EQUAL_UINT8( 0 , pSimLed->get());

  // now let's create a LedCtrl object
  LedCtrl object(pSimLed);

  // check ani List
  String aniList = object.getNameList();
  TEST_ASSERT_GREATER_OR_EQUAL_UINT32( 5 , aniList.length());
  StringList list(aniList.c_str(),',');
  int count=0;
  TEST_ASSERT_FALSE(list.isEndReached());
    
  while (list.isEndReached() == false){
    String aniName = list.getNextListEntry();
    StringList split(aniName.c_str(),':');

    TEST_ASSERT_FALSE(split.isEndReached());
    String number=split.getNextListEntry();
    TEST_ASSERT_FALSE(split.isEndReached());
    String name=split.getNextListEntry();
    TEST_ASSERT_TRUE(split.isEndReached());

    TEST_ASSERT_EQUAL_INT(count,convertStrToInt(number));
    count++;
  }
}

void test_LedCtrl_off(void){
  Led simLed(LED_BUILTIN);
  LedCtrl object(&simLed);

  // check sim mode & value
  TEST_ASSERT_EQUAL_STRING("off",object.getName());
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());

  // test loop
  object.loop(70);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  object.loop(200);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  object.loop(1000);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
}


void test_LedCtrl_on(void){
  Led simLed(LED_BUILTIN);
  LedCtrl object(&simLed);

  // check sim mode & value
  TEST_ASSERT_EQUAL_STRING("off",object.getName());
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());

  // change mode
  object.setup(1);
  TEST_ASSERT_EQUAL_STRING("on",object.getName());
  object.setup(0);
  TEST_ASSERT_EQUAL_STRING("off",object.getName());
  object.setup((const char *) "on");
  TEST_ASSERT_EQUAL_STRING("on",object.getName());

  // test loop
  object.loop(70);
  TEST_ASSERT_EQUAL_UINT8( LED_MAX , simLed.get());
  object.loop(200);
  TEST_ASSERT_EQUAL_UINT8( LED_MAX , simLed.get());
  object.loop(1000);
  TEST_ASSERT_EQUAL_UINT8( LED_MAX , simLed.get());
}

void test_LedCtrl_dim(void){
  Led simLed(LED_BUILTIN);
  LedCtrl object(&simLed);

  // check sim mode & value
  TEST_ASSERT_EQUAL_STRING("off",object.getName());
  object.loop(50);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());

  // change mode
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  object.setup((const char *) "dim");
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  TEST_ASSERT_EQUAL_STRING("dim",object.getName());


  object.loop(70);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());
  object.loop(200);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());
  object.loop(1000);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());

  object.setup(123,0,0,0,0,NULL);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());
  object.loop(1001);
  TEST_ASSERT_EQUAL_UINT8( 123 , simLed.get());

}

      /*  
        ref    | default value |  layout
        =======+===============+===========================
        name:  |               |  blink
        -------+---------------+---------------------------
        p1:    | 0x0000 0080   |  0x0000 00DD
               |               |  D: dim value  
        -------+---------------+---------------------------
        p2:    | 250           |  on time in ms     
        -------+---------------+---------------------------
        p3:    | 250           |  off time in ms
        -------+---------------+---------------------------
        p4:    |               |  N/A
        -------+---------------+---------------------------
        pData: | N/A           |  length(0):
               |               |    N/A        
    */
void test_LedCtrl_blink(void){
  Led simLed(LED_BUILTIN);
  LedCtrl object(&simLed);

  // check sim mode & value
  TEST_ASSERT_EQUAL_STRING("off",object.getName());
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());

  // change mode
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  object.setup((const char *) "blink");
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());

  object.loop(1);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  object.loop(3);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  object.loop(3);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  object.loop(10);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  object.loop(100);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  object.loop(249);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  object.loop(250);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());

  object.loop(251);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());
  object.loop(300);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());
  object.loop(500);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());

  object.loop(501);
  TEST_ASSERT_EQUAL_UINT8( 0x0 , simLed.get());
  object.loop(750);
  TEST_ASSERT_EQUAL_UINT8( 0x0 , simLed.get());

  object.loop(751);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());
  object.loop(1000);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());

  object.loop(1001);
  TEST_ASSERT_EQUAL_UINT8( 0x0 , simLed.get());

  object.setup(123,100,100,100,100,NULL);
  TEST_ASSERT_EQUAL_UINT8( 0x0 , simLed.get());

  object.loop(1002);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  object.loop(1101);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());

  object.loop(1102);
  TEST_ASSERT_EQUAL_UINT8( 123 , simLed.get());
  object.loop(1201);
  TEST_ASSERT_EQUAL_UINT8( 123 , simLed.get());

  object.loop(1202);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());

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
  RUN_TEST(test_LedCtrl_constructor);
  RUN_TEST(test_LedCtrl_init);
  RUN_TEST(test_LedCtrl_aniList);
  RUN_TEST(test_LedCtrl_off);
  RUN_TEST(test_LedCtrl_on);
  RUN_TEST(test_LedCtrl_dim);
  RUN_TEST(test_LedCtrl_blink);
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

