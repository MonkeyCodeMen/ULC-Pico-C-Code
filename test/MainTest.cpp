#include <Arduino.h>

#include <Debug.hpp>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include "unity_config.h"
#include <unity.h>



// prototype here all external test collections 

extern void test_collection_SimpleTest();
extern void test_collection_StringList();
extern void test_collection_helper();
extern void test_collection_LedCtrl();


void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}



// now we call here all test collections
int runAllCollections(void) {
  UNITY_BEGIN();
  test_collection_SimpleTest();
  test_collection_helper();
  test_collection_StringList();
  test_collection_LedCtrl();
  return UNITY_END();
}



/**
  * For Arduino framework
  */
void setup() {
  // Wait ~2 seconds before the Unity test runner
  // establishes connection with a board Serial interface
  delay(2000);

  /*  
  in case of issues with test results (outputs) 
  try the next commands to catch on a simple ternminal
  Serial1.begin(115200);
  Serial1.println(" serial 1 begin 115200 ");
  */
  
  
  runAllCollections();

  pinMode(LED_BUILTIN,OUTPUT);
}


void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);

}
