#include <Arduino.h>
#include <MenuEntry.hpp>


#include <unity.h>



void test_Menu_staticEntry(void) {
  MenuEntry * pObj=NULL;

  TEST_ASSERT_TRUE(pObj == NULL);
  pObj = new MenuEntry();
  TEST_ASSERT_TRUE(pObj != NULL);

  TEST_ASSERT_EQUAL_STRING("",pObj->getText().c_str());
  TEST_ASSERT_FALSE(pObj->onEvent(EVENT_NONE));
  TEST_ASSERT_FALSE(pObj->onEvent(EVENT_INC));
  TEST_ASSERT_FALSE(pObj->onEvent(EVENT_DEC));
  TEST_ASSERT_FALSE(pObj->onEvent(EVENT_ENTER));
  
  



  // destructor Ctrl
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
  RUN_TEST(test_Menu_staticEntry);
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

