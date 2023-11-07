#include <Arduino.h>

#include <Debug.hpp>
#include <helper.hpp>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library


#include <unity.h>

#include <globalObjects.hpp>

/* basic file tests */



#ifdef WITH_SD_CARD


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

    TEST_ASSERT_TRUE(SD.begin(PIN_SPI0_CS_SD));
    SDFile root = SD.open("/");

    TEST_ASSERT_TRUE(root.available());
    TEST_ASSERT_TRUE(root.isDirectory());
}


void test_File_WriteRead(void) {

    String data = createTestData();
    SDFile myFile;
    String out="";
    const char * fileName = F_CONST("test.txt");

    TEST_ASSERT_TRUE(data.length() > 0);


    // Check to see if the file exists:
    if (SD.exists(fileName)) {
        SD.remove(fileName);
        // now file should be history
        TEST_ASSERT_FALSE(SD.exists(fileName));
    }

    myFile = SD.open(fileName, FILE_WRITE);
    TEST_ASSERT_EQUAL_INT(0,myFile.position());
    TEST_ASSERT_FALSE(myFile.isDirectory());
    TEST_ASSERT_EQUAL_STRING(fileName,myFile.name());

    // write data to file
    myFile.printf(data.c_str());
    TEST_ASSERT_EQUAL_INT16(0,myFile.getWriteError());
    
    myFile.close();
    TEST_ASSERT_FALSE(myFile.available());

    TEST_ASSERT_TRUE(SD.exists(fileName));

    myFile = SD.open(fileName, FILE_READ);
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
int runAllCollections(void) {
    UNITY_BEGIN();
    RUN_TEST(test_SD_open);
    RUN_TEST(test_File_WriteRead);
    return UNITY_END();
}

#else


// if project is without SD card
int runAllCollections(void) {
    UNITY_BEGIN();
    // nothing to test for this module
    return UNITY_END();
}

#endif


void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
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





