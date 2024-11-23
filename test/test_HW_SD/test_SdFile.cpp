
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





String createTestData(){

    // create data
    String data="";
    int i,ii;
    for (i=0;i<10;i++){
        for (ii=0; ii<12-i  ;ii++)      data+=' ';
        for (ii=0; ii<1+i*2 ;ii++)      data+='*';
        for (ii=0; ii<12-i  ;ii++)      data+=' ';
        data+='\n';
    }
    for (i=0;i<3;i++){
        for (ii=0; ii<12-1  ;ii++)      data+=' ';
        for (ii=0; ii<1+1*2 ;ii++)      data+='*';
        for (ii=0; ii<12-1  ;ii++)      data+=' ';
        data+='\n';
    }
    data+="merry christams!!\n\n";

    return data;
}



void test_SD_open(void) {

    TEST_ASSERT_TRUE(globalSDcard0.begin(PIN_SD_CS));
    SDFile root = globalSDcard0.open("/");

    TEST_ASSERT_TRUE(root.available());
    TEST_ASSERT_TRUE(root.isDirectory());
}


void test_File_WriteRead(void) {

    String data = createTestData();
    SDFile myFile;
    String out="";
    const char * fileName = "test.txt";

    TEST_ASSERT_TRUE(data.length() > 0);


    // Check to see if the file exists:
    if (globalSDcard0.exists(fileName)) {
        globalSDcard0.remove(fileName);
        // now file should be history
        TEST_ASSERT_FALSE(globalSDcard0.exists(fileName));
    }

    myFile = globalSDcard0.open(fileName, FILE_WRITE);
    TEST_ASSERT_EQUAL_INT(0,myFile.position());
    TEST_ASSERT_FALSE(myFile.isDirectory());
    TEST_ASSERT_EQUAL_STRING(fileName,myFile.name());

    // write data to file
    myFile.printf(data.c_str());
    TEST_ASSERT_EQUAL_INT16(0,myFile.getWriteError());
    
    myFile.close();
    TEST_ASSERT_FALSE(myFile.available());

    TEST_ASSERT_TRUE(globalSDcard0.exists(fileName));

    myFile = globalSDcard0.open(fileName, FILE_READ);
    TEST_ASSERT_EQUAL_INT(0,myFile.position());
    TEST_ASSERT_TRUE(myFile.available());
    TEST_ASSERT_FALSE(myFile.isDirectory());
    TEST_ASSERT_EQUAL_STRING(fileName,myFile.name());
    TEST_ASSERT_EQUAL_INT16(data.length(),myFile.size());


    String res = myFile.readString();
    myFile.close();

    TEST_ASSERT_EQUAL_STRING(data.c_str(),res.c_str());
    TEST_ASSERT_EQUAL_INT16(data.length(),res.length());

}






// now we call here all test collections
int runAllTests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_SD_open);
    RUN_TEST(test_File_WriteRead);
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
