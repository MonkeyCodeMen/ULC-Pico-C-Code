#include <Arduino.h>

#include <Debug.hpp>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include <unity.h>

// prototype here all external test collections 

extern void test_collection_configItem();
extern void test_collection_scenario();



void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}



// now we call here all test collections
int runAllCollections(void) {
  UNITY_BEGIN();
  test_collection_configItem();
  test_collection_scenario();
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
