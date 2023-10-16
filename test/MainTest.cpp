#include <Arduino.h>

#include <Debug.hpp>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <cube.hpp>
#include <WS2812FX.h>
#include <RgbLedCtrl.hpp>
#include <LedCtrl.hpp>
#include <NeoStripeCtrl.hpp>
#include <NeoMatrixCtrl.hpp>
#include "Com.hpp"


#include "unity_config.h"
#include <unity.h>



// prototype here all external test collections 

void test_collection_SimpleTest();
void test_collection_StringList();




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
  test_collection_StringList();
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
