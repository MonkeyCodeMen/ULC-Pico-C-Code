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
  
  for (int i=0; i < 10;i++)  
    TEST_ASSERT_FALSE(pObj->hasChanged());

  pObj->setNewText("new text");
  TEST_ASSERT_EQUAL_STRING("new text",pObj->getText().c_str());
  TEST_ASSERT_TRUE(pObj->hasChanged());
  TEST_ASSERT_FALSE(pObj->hasChanged());
  TEST_ASSERT_FALSE(pObj->hasChanged());


  MenuEntry obj;
  TEST_ASSERT_EQUAL_STRING("",obj.getText().c_str());
  TEST_ASSERT_FALSE(obj.onEvent(EVENT_NONE));
  TEST_ASSERT_FALSE(obj.onEvent(EVENT_INC));
  TEST_ASSERT_FALSE(obj.onEvent(EVENT_DEC));
  TEST_ASSERT_FALSE(obj.onEvent(EVENT_ENTER));
  
  for (int i=0; i < 10;i++)  
    TEST_ASSERT_FALSE(obj.hasChanged());

  obj = *pObj;
  TEST_ASSERT_FALSE(obj.hasChanged());
  TEST_ASSERT_EQUAL_STRING("new text",obj.getText().c_str());


  // destructor Ctrl
  delete pObj;
  pObj = NULL;
  TEST_ASSERT_EQUAL(NULL,pObj);
  TEST_ASSERT_FALSE(obj.hasChanged());
  TEST_ASSERT_EQUAL_STRING("new text",obj.getText().c_str());

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

