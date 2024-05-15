#include <Arduino.h>

#include <Debug.hpp>
#include <MainConfig.h>

#include <helper.h>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <SDcard.hpp>
#include <TFT_eSPI.h> // Hardware-specific library
#include "Led.hpp"
#include "Ani.hpp"
#include "StringList.hpp"

#include "unity.h"

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}


void test_dimCtrl_init(void){
  Ani obj("testObjANi");
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  TEST_ASSERT_FALSE(obj.hasChanged());
  obj.loop(0);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  TEST_ASSERT_FALSE(obj.hasChanged());
  obj.loop(10);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  TEST_ASSERT_FALSE(obj.hasChanged());
  obj.loop(100);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  TEST_ASSERT_FALSE(obj.hasChanged());
  obj.loop(1000);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  TEST_ASSERT_FALSE(obj.hasChanged());
  obj.loop(10*1000);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  TEST_ASSERT_FALSE(obj.hasChanged());
}


void test_dimCtrl_set(void){
  Ani obj("testObjANi");
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  obj.loop(0);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  obj.loop(1000);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM | 0x77,0,0,0,""));
  // value will be taken over at next loop
  TEST_ASSERT_EQUAL_UINT8( 0x0 , obj.getDim());
  TEST_ASSERT_FALSE(obj.hasChanged());
  obj.loop(1001);  
  TEST_ASSERT_TRUE(obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT8( 0x77 , obj.getDim());


  // test config without WR bit
  obj.config(AniCfg(0x66,0,0,0,""));
  // must be still the old value
  TEST_ASSERT_EQUAL_UINT8( 0x77 , obj.getDim());  
  obj.loop(2000);
  TEST_ASSERT_EQUAL_UINT8( 0x77 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM | 0x11,0,0,0,""));
  TEST_ASSERT_EQUAL_UINT8( 0x77 , obj.getDim());
  obj.loop(3000);
  TEST_ASSERT_EQUAL_UINT8( 0x11 , obj.getDim());
}

void test_dimCtrl_dimUp(void){
  Ani obj("testObjANi");
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  obj.loop(0);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
 

  obj.config(AniCfg(ANI_WR_DIM |  0x60,0,0,0,""));
  // value will be taken over at next loop
  TEST_ASSERT_EQUAL_UINT8( 0x0 , obj.getDim());
  obj.loop(1001);  
  TEST_ASSERT_EQUAL_UINT8( 0x60 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 0x70 , obj.getDim());
  
  obj.config(AniCfg(ANI_WR_DIM |  0x2000,0,0,0,""));
  obj.loop(2005);  
  TEST_ASSERT_EQUAL_UINT8( 0x90 , obj.getDim());

  obj.config(AniCfg(0x2000,0,0,0,""));
  obj.loop(2010);  
  TEST_ASSERT_EQUAL_UINT8( 0x90 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3000);  
  TEST_ASSERT_EQUAL_UINT8( 0xA0 , obj.getDim());
  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3001);  
  TEST_ASSERT_EQUAL_UINT8( 0xB0 , obj.getDim());
  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3002);  
  TEST_ASSERT_EQUAL_UINT8( 0xC0 , obj.getDim());
  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3003);  
  TEST_ASSERT_EQUAL_UINT8( 0xD0 , obj.getDim());
  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3004);  
  TEST_ASSERT_EQUAL_UINT8( 0xE0 , obj.getDim());
  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3005);  
  TEST_ASSERT_EQUAL_UINT8( 0xF0 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3010);  
  TEST_ASSERT_EQUAL_UINT8( 0xFF , obj.getDim());  // limit on max 0xFF

  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3011);  
  TEST_ASSERT_EQUAL_UINT8( 0xFF , obj.getDim());  // limit on max 0xFF

}

void test_dimCtrl_dimDown(void){
  Ani obj("testObjANi");
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  obj.loop(0);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
 

  obj.config(AniCfg(ANI_WR_DIM |  51,0,0,0,""));
  // value will be taken over at next loop
  TEST_ASSERT_EQUAL_UINT8( 0x0 , obj.getDim());
  obj.loop(1001);  
  TEST_ASSERT_EQUAL_UINT8( 51 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM |  ((-10)<<8),0,0,0,""));
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 41 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM |  ((-10)<<8),0,0,0,""));
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 31 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM |  11,0,0,0,""));
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 11 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM |  ((-10)<<8),0,0,0,""));
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 1 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM |  ((-10)<<8),0,0,0,""));
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());

}

// now we call here all test collections
int runAllCollections(void) {
  UNITY_BEGIN();
  RUN_TEST(test_dimCtrl_init);
  RUN_TEST(test_dimCtrl_set);
  RUN_TEST(test_dimCtrl_dimUp);
  RUN_TEST(test_dimCtrl_dimDown);
  return UNITY_END();
}



/**
  * For Arduino framework
  */
void setup() {
  // Wait ~2 seconds before the Unity test runner
  // establishes connection with a board Serial interface
  delay(6000);
  
  runAllCollections();

  pinMode(LED_BUILTIN,OUTPUT);
}


void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);

}




#ifdef old
void test_LedCtrl_constructor(void) {
  // create Sim LED object and test it
  Led *pSimLed;
  pSimLed = new Led;
  pSimLed->begin(LED_BUILTIN,true);
  TEST_ASSERT_TRUE( pSimLed != NULL);
  TEST_ASSERT_EQUAL_UINT8( 0 , pSimLed->get());

  // now let's create a LedCtrl object
  LedCtrl *pObject;
  pObject = new LedCtrl;
  pObject->begin(pSimLed);
  TEST_ASSERT_TRUE( pObject != NULL);
  delete pObject;
  TEST_ASSERT_TRUE( pSimLed != NULL);
  TEST_ASSERT_EQUAL_UINT8( 0 , pSimLed->get()); // Ctrl object should not haved touched LED (loop not called)

}

void test_LedCtrl_init(void) {
  // create Sim LED object and test it
  Led *pSimLed;
  pSimLed = new Led;
  pSimLed->begin(LED_BUILTIN,true);
  TEST_ASSERT_TRUE( pSimLed != NULL);
  TEST_ASSERT_EQUAL_UINT8( 0 , pSimLed->get());

  // now let's create a LedCtrl object
  LedCtrl *pObject;
  pObject = new LedCtrl;
  pObject->begin(pSimLed);
  TEST_ASSERT_TRUE( pObject != NULL);
  TEST_ASSERT_TRUE( pSimLed != NULL);
  TEST_ASSERT_EQUAL_UINT8( 0 , pSimLed->get()); // Ctrl object should not haved touched LED (loop not called)

  // check sim mode & value
  TEST_ASSERT_EQUAL_STRING("off",pObject->getName());
}

void test_LedCtrl_aniList(void) {
  // create Sim LED object and test it
  Led *pSimLed;
  pSimLed = new Led;
  pSimLed->begin(LED_BUILTIN,true);

  TEST_ASSERT_TRUE( pSimLed != NULL);
  TEST_ASSERT_EQUAL_UINT8( 0 , pSimLed->get());

  // now let's create a LedCtrl object
  LedCtrl object;
  object.begin(pSimLed);

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
  Led simLed;
  simLed.begin(LED_BUILTIN,true);
  LedCtrl object;
  object.begin(&simLed);

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
  Led simLed;
  simLed.begin(LED_BUILTIN,true);
  LedCtrl object;
  object.begin(&simLed);

  // check sim mode & value
  TEST_ASSERT_EQUAL_STRING("off",object.getName());
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());

  // change mode
  object.select(1);
  TEST_ASSERT_EQUAL_STRING("on",object.getName());
  object.select(0);
  TEST_ASSERT_EQUAL_STRING("off",object.getName());
  object.select((const char *) "on");
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
  Led simLed;
  simLed.begin(LED_BUILTIN,true);
  LedCtrl object;
  object.begin(&simLed);

  // check sim mode & value
  TEST_ASSERT_EQUAL_STRING("off",object.getName());
  object.loop(50);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());

  // change mode
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  object.select((const char *) "dim");
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  TEST_ASSERT_EQUAL_STRING("dim",object.getName());


  object.loop(70);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());
  object.loop(200);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());
  object.loop(1000);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());

  object.select(123,0,0,0,"",0,NULL);
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
  Led simLed;
  simLed.begin(LED_BUILTIN,true);
  LedCtrl object;
  object.begin(&simLed);

  // check sim mode & value
  TEST_ASSERT_EQUAL_STRING("off",object.getName());
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());

  // change mode
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  object.select((const char *) "blink");
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

  object.select(123,100,100,100,"",0,NULL);
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

#endif