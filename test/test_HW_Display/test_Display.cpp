
/*
 * MIT License
 * 
 * Copyright (c) 2024 MonkeyCodeMen@GitHub
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * provided to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


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
