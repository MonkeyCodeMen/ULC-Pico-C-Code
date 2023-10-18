#include <Arduino.h>

#define LED_MAX 255
#define LED_OFF 0
#define LED_MIN 0


#define LED_LOGIC_INVERS    false
#define LED_PWM_RANGE       255
#define LED_DIM_ACCURACY    LED_PWM_RANGE


class Led
{
    public:
        Led(int pin)            {_value = 0;};
        ~Led() = default;

        void set(u8_t value)    {_value = value;};
        u8_t get()              {return _value;};
    private:
        u8_t    _value;
};

#define LED_H  // avoid definition of standard LED class

#include "LedCtrl.hpp"
#include "StringList.hpp"

#include "unity.h"

void test_LedCtrl_constructor(void) {
  Led simLed(-1);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());

  LedCtrl object(&simLed);

  return;

  String res;


  // check sim mode & value
  res = object.getName();
  TEST_ASSERT_EQUAL_STRING("off",res.c_str());


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

void test_LedCtrl_on(void){
  Led simLed(-1);
  LedCtrl object(&simLed);

  // check sim mode & value
  TEST_ASSERT_EQUAL_STRING("off",object.getName().c_str());
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());



  // change mode
  object.setup(1);
  TEST_ASSERT_EQUAL_STRING("on",object.getName().c_str());
  object.setup(0);
  TEST_ASSERT_EQUAL_STRING("off",object.getName().c_str());
  object.setup((const char *) "on");
  TEST_ASSERT_EQUAL_STRING("on",object.getName().c_str());

  return;

  object.loop(70);
  TEST_ASSERT_EQUAL_UINT8( LED_MAX , simLed.get());
  object.loop(200);
  TEST_ASSERT_EQUAL_UINT8( LED_MAX , simLed.get());
  object.loop(1000);
  TEST_ASSERT_EQUAL_UINT8( LED_MAX , simLed.get());
}

void test_LedCtrl_dim(void){
  Led simLed(-1);
  LedCtrl object(&simLed);

  // check sim mode & value
  TEST_ASSERT_EQUAL_STRING("off",object.getName().c_str());
  object.loop(50);
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());



  // change mode
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  object.setup((const char *) "dim");
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  TEST_ASSERT_EQUAL_STRING("dim",object.getName().c_str());

  return;

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
  Led simLed(-1);
  LedCtrl object(&simLed);

  // check sim mode & value
  TEST_ASSERT_EQUAL_STRING("off",object.getName().c_str());
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());

  // change mode
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());
  object.setup((const char *) "blink");
  TEST_ASSERT_EQUAL_UINT8( 0 , simLed.get());

  return;

  object.loop(1);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());
  object.loop(3);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());
  object.loop(3);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());
  object.loop(10);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());
  object.loop(100);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());
  object.loop(249);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());
  object.loop(250);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());

  object.loop(251);
  TEST_ASSERT_EQUAL_UINT8( 0x00 , simLed.get());
  object.loop(300);
  TEST_ASSERT_EQUAL_UINT8( 0x00 , simLed.get());
  object.loop(500);
  TEST_ASSERT_EQUAL_UINT8( 0x00 , simLed.get());

  object.loop(501);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());
  object.loop(750);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());

  object.loop(751);
  TEST_ASSERT_EQUAL_UINT8( 0x00 , simLed.get());
  object.loop(1000);
  TEST_ASSERT_EQUAL_UINT8( 0x00 , simLed.get());

  object.loop(1001);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());

  object.setup(123,100,100,100,100,NULL);
  TEST_ASSERT_EQUAL_UINT8( 0x80 , simLed.get());

  object.loop(1002);
  TEST_ASSERT_EQUAL_UINT8( 123 , simLed.get());

#
}


// collect all tests of this file to one collection
void test_collection_LedCtrl(void) {
  RUN_TEST(test_LedCtrl_constructor);
  //RUN_TEST(test_LedCtrl_on);
  //RUN_TEST(test_LedCtrl_dim);
  //RUN_TEST(test_LedCtrl_blink);
}
