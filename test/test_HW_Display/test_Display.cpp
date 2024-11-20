#include <Arduino.h>

#include <PinMapping.h>
#include <Debug.hpp>
#include <helper.h>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <SDcard.hpp>
#include <Display.hpp>


#include <unity.h>
/* basic file tests */






void test_display(){
    display.begin();

}


// now we call here all test collections
int runAllTests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_display);
    return UNITY_END();
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
