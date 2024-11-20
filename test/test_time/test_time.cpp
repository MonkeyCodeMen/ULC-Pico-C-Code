#include <Arduino.h>
#include <MainConfig.h>
#include <unity.h>
#include <Blink.hpp>
BlinkingLED  blink = BlinkingLED(LED_BUILTIN);
std::vector<uint32_t> testBlinkSeq = BLINK_SEQ_TEST;

#include <SPI.h>
#include <BufferedClock.hpp>
#include <Adafruit_GFX.h>  // why ??
#include <LittleFS.h>

// Mock classes for testing
#include "MockTwoWire.hpp"
#include "Mutex.hpp"
#include "MockRTC_DS1307.hpp"


// Global variables for mock setup
MockTwoWire     mockWire;
Mutex           mockMutex;
MockRTC_DS1307  mockRTC;

// Global variable for the `Time` object
BufferdClock testClock;


/* Test: Default Initialization */
void test_default_initialization(void) {
    TEST_ASSERT_FALSE(testClock.getLoopDateTime().isValid());        // Ensure _loopDate is invalid by default
    TEST_ASSERT_EQUAL(0, testClock.getErrorCounterInit());           // Error counter should be 0
}

/* Test: Begin with Valid Bus and RTC */
void test_begin_with_valid_bus_and_rtc(void) {
    testClock.begin(&mockWire, &mockMutex);                     // Call begin with valid mocks
    TEST_ASSERT_TRUE(testClock.getLoopDateTime().isValid());         // _loopDate should be valid
    TEST_ASSERT_EQUAL(0, testClock.getErrorCounterInit());           // Error counter should remain 0
}

/* Test: Failed RTC Initialization Increments Error Counter */
void test_failed_rtc_initialization_increments_error_counter(void) {
    mockRTC.setFailInit(true);                                  // Set RTC to fail initialization
    testClock.begin( &mockWire, &mockMutex);                     // Call begin with failing RTC
    TEST_ASSERT_EQUAL(1, testClock.getErrorCounterInit());           // Error counter should increment by 1
    mockRTC.setFailInit(false);                                 // Reset RTC to succeed on next init
}

/* Test: Synchronization After Interval */
void test_sync_after_interval(void) {
    uint32_t now = millis();
    testClock.begin(&mockWire, &mockMutex, MIN_SYNC_INTERVAL_MSEC); // Begin with minimum sync interval

    delay(MIN_SYNC_INTERVAL_MSEC + 100);                        // Wait for interval to elapse
    testClock.loop(now + MIN_SYNC_INTERVAL_MSEC + 100);              // Call loop to trigger sync

    TEST_ASSERT_TRUE(testClock.getLoopDateTime().isValid());         // Ensure _loopDate is updated
}

/* Test: Software testClock Update Without Sync */
void test_software_testClock_update_without_sync(void) {
    uint32_t now = millis();
    testClock.begin(&mockWire, &mockMutex, NO_SYNC);            // Begin with NO_SYNC to prevent syncing

    delay(1000);                                                // Wait 1 second
    testClock.loop(now + 1000);                                      // Update software testClock

    DateTime updatedTime = testClock.getLoopDateTime();
    TEST_ASSERT_EQUAL(2024, updatedTime.year());                // Confirm base year is still correct
    TEST_ASSERT_EQUAL(1, updatedTime.month());                  // Confirm base month
    TEST_ASSERT_EQUAL(1, updatedTime.day());                    // Confirm base day
}

/* Test: Error Counter Increases on Consecutive Failures */
void test_error_counter_increments_on_consecutive_failures(void) {
    mockRTC.setFailInit(true);                                  // Cause RTC to fail init
    uint32_t now = millis();

    testClock.begin(&mockWire, &mockMutex);                     // First failure
    TEST_ASSERT_EQUAL(1, testClock.getErrorCounterInit());

    testClock.loop(now + MIN_SYNC_INTERVAL_MSEC + 100);              // Retry init in loop
    TEST_ASSERT_EQUAL(2, testClock.getErrorCounterInit());           // Error counter should increment on retry failure
}

void runAllTests() {
    UNITY_BEGIN();

    RUN_TEST(test_default_initialization);
    RUN_TEST(test_begin_with_valid_bus_and_rtc);
    RUN_TEST(test_failed_rtc_initialization_increments_error_counter);
    RUN_TEST(test_sync_after_interval);
    RUN_TEST(test_software_testClock_update_without_sync);
    RUN_TEST(test_error_counter_increments_on_consecutive_failures);

    UNITY_END();
}


/**
  * For Arduino framework
  */
#include <MainConfig.h>
#include <Blink.hpp>
BlinkingLED  blink = BlinkingLED(LED_BUILTIN);
std::vector<uint32_t> testBlinkSeq = BLINK_SEQ_TEST;

void setUp(void) {
    // Reset the time object and initialize now
    testClock = BufferdClock();
}

void tearDown(void) {
    // Clean up code after each test if needed
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
