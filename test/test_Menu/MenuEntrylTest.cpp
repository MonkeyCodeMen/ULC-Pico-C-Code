#include <Arduino.h>

#include <Debug.hpp>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include <Menu.hpp>
#include <unity.h>



void test_Menu_staticEntry(void) {
  MenuEntryText * pObj=NULL;

  TEST_ASSERT_TRUE(pObj == NULL);
  pObj = new MenuEntryText();
  TEST_ASSERT_TRUE(pObj != NULL);

  for(int i=0;i < 10; i++){
    // should be stable
    TEST_ASSERT_EQUAL_STRING(MENU_STD_EMPTY_CURSOR,pObj->getText().c_str());
    TEST_ASSERT_FALSE(pObj->onEvent(EVENT_NONE));
    TEST_ASSERT_FALSE(pObj->onEvent(EVENT_RIGHT));
    TEST_ASSERT_FALSE(pObj->onEvent(EVENT_LEFT));
    TEST_ASSERT_FALSE(pObj->onEvent(EVENT_ENTER));
    TEST_ASSERT_TRUE(pObj->needsUpdate());
  }

  // test focus
  pObj->setNewText("new text");
  TEST_ASSERT_EQUAL_STRING("   new text",pObj->getText().c_str());
  pObj->setFocus();
  TEST_ASSERT_EQUAL_STRING("<*>new text",pObj->getText().c_str());
  pObj->clearFocus();
  TEST_ASSERT_EQUAL_STRING("   new text",pObj->getText().c_str());
  TEST_ASSERT_TRUE(pObj->needsUpdate());
  // draw
  //TEST_ASSERT_FALSE(pObj->needsUpdate());
  //TEST_ASSERT_FALSE(pObj->needsUpdate());

  pObj->changeCursorOption("","");
  TEST_ASSERT_EQUAL_STRING("new text",pObj->getText().c_str());
  TEST_ASSERT_EQUAL_STRING("new text",pObj->getText().c_str());
  TEST_ASSERT_TRUE(pObj->needsUpdate());


  MenuEntryText obj;
  TEST_ASSERT_EQUAL_STRING(MENU_STD_EMPTY_CURSOR,obj.getText().c_str());
  TEST_ASSERT_FALSE(obj.onEvent(EVENT_NONE));
  TEST_ASSERT_FALSE(obj.onEvent(EVENT_RIGHT));
  TEST_ASSERT_FALSE(obj.onEvent(EVENT_LEFT));
  TEST_ASSERT_FALSE(obj.onEvent(EVENT_ENTER));
  

  obj = *pObj;
  TEST_ASSERT_TRUE(obj.needsUpdate());
  TEST_ASSERT_EQUAL_STRING("new text",obj.getText().c_str());
  TEST_ASSERT_EQUAL_STRING("new text",obj.getText().c_str());


  // destructor Ctrl
  delete pObj;
  pObj = NULL;
  TEST_ASSERT_EQUAL(NULL,pObj);
  TEST_ASSERT_TRUE(obj.needsUpdate());
  TEST_ASSERT_EQUAL_STRING("new text",obj.getText().c_str());
  TEST_ASSERT_EQUAL_STRING("new text",obj.getText().c_str());

}



void test_Menu_boolEntry(){
  MenuEntryBoolToggle obj1("switch : ");
  MenuEntryBool       obj2("==Flag==:",false,true,F_CHAR("TRUE"),F_CHAR("FALSE"),F_CHAR("("),F_CHAR(")"));

  // we focus now only on text results
  obj1.changeCursorOption("","");
  obj2.changeCursorOption("","");
  
  // test init
  TEST_ASSERT_FALSE(obj1.getValue());
  TEST_ASSERT_FALSE(obj2.getValue());

  // test text output
  TEST_ASSERT_EQUAL_STRING("switch : [off]",obj1.getText().c_str());
  TEST_ASSERT_EQUAL_STRING("==Flag==:(FALSE)",obj2.getText().c_str());

  // test access of deviated virtual member functions over base class
  MenuEntryText *p = &obj1;
  TEST_ASSERT_EQUAL_STRING("switch : [off]",p->getText().c_str());

  // test toggle logic  wrap arround
  TEST_ASSERT_TRUE(p->onEvent(EVENT_RIGHT));
  TEST_ASSERT_TRUE(obj1.getValue());
  TEST_ASSERT_EQUAL_STRING("switch : [on]",p->getText().c_str());
  
  TEST_ASSERT_TRUE(obj1.onEvent(EVENT_RIGHT));
  TEST_ASSERT_FALSE(obj1.getValue());

  TEST_ASSERT_TRUE(obj1.onEvent(EVENT_LEFT));
  TEST_ASSERT_TRUE(obj1.getValue());

  TEST_ASSERT_TRUE(obj1.onEvent(EVENT_ENTER));
  TEST_ASSERT_FALSE(obj1.getValue());

  TEST_ASSERT_FALSE(obj1.onEvent(EVENT_NONE));
  TEST_ASSERT_FALSE(obj1.getValue());



  // test toggle logic without wrap
  TEST_ASSERT_FALSE(obj2.onEvent(EVENT_NONE));
  TEST_ASSERT_FALSE(obj2.getValue());
  TEST_ASSERT_FALSE(obj2.onEvent(EVENT_NONE));
  TEST_ASSERT_FALSE(obj2.getValue());
  TEST_ASSERT_EQUAL_STRING("==Flag==:(FALSE)",obj2.getText().c_str());

  TEST_ASSERT_TRUE(obj2.onEvent(EVENT_RIGHT));
  TEST_ASSERT_TRUE(obj2.getValue());
  TEST_ASSERT_TRUE(obj2.onEvent(EVENT_RIGHT));
  TEST_ASSERT_TRUE(obj2.getValue());
  TEST_ASSERT_EQUAL_STRING("==Flag==:(TRUE)",obj2.getText().c_str());

  TEST_ASSERT_TRUE(obj2.onEvent(EVENT_ENTER));
  TEST_ASSERT_FALSE(obj2.getValue());
  TEST_ASSERT_TRUE(obj2.onEvent(EVENT_ENTER));
  TEST_ASSERT_FALSE(obj2.getValue());
  TEST_ASSERT_EQUAL_STRING("==Flag==:(FALSE)",obj2.getText().c_str());

  TEST_ASSERT_TRUE(obj2.onEvent(EVENT_RIGHT));
  TEST_ASSERT_TRUE(obj2.getValue());
  TEST_ASSERT_TRUE(obj2.onEvent(EVENT_RIGHT));
  TEST_ASSERT_TRUE(obj2.getValue());
  TEST_ASSERT_EQUAL_STRING("==Flag==:(TRUE)",obj2.getText().c_str());

  TEST_ASSERT_TRUE(obj2.onEvent(EVENT_LEFT));
  TEST_ASSERT_FALSE(obj2.getValue());
  TEST_ASSERT_TRUE(obj2.onEvent(EVENT_LEFT));
  TEST_ASSERT_FALSE(obj2.getValue());
  TEST_ASSERT_EQUAL_STRING("==Flag==:(FALSE)",obj2.getText().c_str());
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
  RUN_TEST(test_Menu_boolEntry);
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

