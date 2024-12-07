#include <Arduino.h>
#include <Debug.hpp>
#include <helper.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include "TimeSpanExt.hpp"

#include <unity.h>

// Test default constructor
void test_default_constructor() {
    TimeSpanExt timeSpan;

    TEST_ASSERT_EQUAL(0, timeSpan.totalseconds());
}

// Test parameterized constructor
void test_parameterized_constructor() {
    TimeSpanExt timeSpan(1, 12, 30, 45); // 1 day, 12 hours, 30 minutes, 45 seconds

    TEST_ASSERT_EQUAL((1 * 86400) + (12 * 3600) + (30 * 60) + 45, timeSpan.totalseconds());
}

// Test comparison operators
void test_comparison_operators() {
    TimeSpanExt t1(3600); // 1 hour
    TimeSpanExt t2(7200); // 2 hours

    TEST_ASSERT_TRUE(t1 < t2);
    TEST_ASSERT_TRUE(t2 > t1);
    TEST_ASSERT_TRUE(t1 <= t2);
    TEST_ASSERT_TRUE(t2 >= t1);
    TEST_ASSERT_TRUE(t1 != t2);

    TimeSpanExt t3(3600); // Another 1 hour
    TEST_ASSERT_TRUE(t1 == t3);
}

// Test addition
void test_addition_operator() {
    TimeSpanExt t1(3600); // 1 hour
    TimeSpanExt t2(1800); // 30 minutes

    TimeSpanExt result = t1 + t2;
    TEST_ASSERT_EQUAL(5400, result.totalseconds()); // 1 hour 30 minutes = 5400 seconds
}

// Test subtraction
void test_subtraction_operator() {
    TimeSpanExt t1(3600); // 1 hour
    TimeSpanExt t2(1800); // 30 minutes

    TimeSpanExt result = t1 - t2;
    TEST_ASSERT_EQUAL(1800, result.totalseconds()); // 30 minutes = 1800 seconds

    // Test negative result handling
    TimeSpanExt negative_result = t2 - t1;
    TEST_ASSERT_EQUAL(0, negative_result.totalseconds());
}

// Test multiplication
void test_multiplication_operator() {
    TimeSpanExt t1(1800); // 30 minutes

    TimeSpanExt result = t1 * 2;
    TEST_ASSERT_EQUAL(3600, result.totalseconds()); // 1 hour = 3600 seconds

    // Test multiplication by invalid factor
    TimeSpanExt invalid_result = t1 * -1;
    TEST_ASSERT_EQUAL(0, invalid_result.totalseconds());
}

// Test division
void test_division_operator() {
    TimeSpanExt t1(3600); // 1 hour

    TimeSpanExt result = t1 / 2;
    TEST_ASSERT_EQUAL(1800, result.totalseconds()); // 30 minutes = 1800 seconds

    // Test division by zero or negative scalar
    TimeSpanExt invalid_result = t1 / 0;
    TEST_ASSERT_EQUAL(INT32_MAX, invalid_result.totalseconds());

    invalid_result = t1 / -2;
    TEST_ASSERT_EQUAL(1800, invalid_result.totalseconds());
}

// Test negative results handling
void test_negative_results() {
    TimeSpanExt t1(0); // 0 seconds
    TimeSpanExt t2(1800); // 30 minutes

    TimeSpanExt result = t1 - t2;
    TEST_ASSERT_EQUAL(0, result.totalseconds()); // Negative results reset to 0

    result = t2 * -1;
    TEST_ASSERT_EQUAL(0, result.totalseconds()); // Negative multiplication resets to 0
}

// Test the print() method
void test_print_method() {
    TimeSpanExt t1(1, 2, 37, 42); // 1 day, 2 hours, 37 minutes, 42 seconds
    String result = t1.print();
    TEST_ASSERT_EQUAL_STRING("P1D2H37M42S", result.c_str()); // Correct ISO format

    TimeSpanExt t2; // 0 seconds
    result = t2.print();
    TEST_ASSERT_EQUAL_STRING("P0S", result.c_str()); // "0S" for zero

    TimeSpanExt t3(0, 0, 45, 0); // 45 minutes
    result = t3.print();
    TEST_ASSERT_EQUAL_STRING("P45M", result.c_str());
}

// Test integration with TimeSpan
void test_integration_with_TimeSpan() {
    TimeSpan timeSpan(1, 12, 30, 45); // TimeSpan instance
    TimeSpanExt timeSpanExt(timeSpan);

    TEST_ASSERT_EQUAL(timeSpan.totalseconds(), timeSpanExt.totalseconds());
    TEST_ASSERT_TRUE(timeSpanExt == timeSpan); // Compare TimeSpan and TimeSpanExt
}

// Test extremely large time spans
void test_large_values() {
    // Very large time span (10 million seconds)
    int32_t large_seconds = 10'000'000;
    TimeSpanExt largeTimeSpan(large_seconds);

    TEST_ASSERT_EQUAL(large_seconds, largeTimeSpan.totalseconds());
    TEST_ASSERT_EQUAL(large_seconds / 86400, largeTimeSpan.days());
    TEST_ASSERT_EQUAL((large_seconds % 86400) / 3600, largeTimeSpan.hours());
    TEST_ASSERT_EQUAL((large_seconds % 3600) / 60, largeTimeSpan.minutes());
    TEST_ASSERT_EQUAL(large_seconds % 60, largeTimeSpan.seconds());

    // Add another large time span
    TimeSpanExt result = largeTimeSpan + TimeSpanExt(large_seconds);
    TEST_ASSERT_EQUAL(large_seconds * 2, result.totalseconds());

    // Subtract a smaller time span
    result = largeTimeSpan - TimeSpanExt(1'000'000);
    TEST_ASSERT_EQUAL(large_seconds - 1'000'000, result.totalseconds());
}

// Test near integer overflow
void test_near_overflow() {
    int32_t max_seconds = INT32_MAX / 2; // Large value but within range
    TimeSpanExt largeTimeSpan(max_seconds);

    TEST_ASSERT_EQUAL(max_seconds, largeTimeSpan.totalseconds());

    // Adding large values to approach overflow
    TimeSpanExt nearOverflow = largeTimeSpan + TimeSpanExt(max_seconds - 1);
    TEST_ASSERT_EQUAL( (long)(INT32_MAX / 2)*2  - 1, nearOverflow.totalseconds());

    // Overflow-safe multiplication
    TimeSpanExt multiplied = largeTimeSpan * 2;
    TEST_ASSERT_EQUAL((long)(INT32_MAX / 2)*2, multiplied.totalseconds());

    // Prevent overflow by clipping
    TimeSpanExt overflowResult = multiplied + TimeSpanExt(1);
    TEST_ASSERT_EQUAL(INT32_MAX, overflowResult.totalseconds());
}

// Test division by large scalars
void test_large_scalar_division() {
    int32_t large_seconds = 10'000'000;
    TimeSpanExt largeTimeSpan(large_seconds);

    TimeSpanExt result = largeTimeSpan / 1'000'000;
    TEST_ASSERT_EQUAL(10, result.totalseconds()); // 10 million seconds / 1 million = 10

    // Division resulting in fractional seconds
    result = largeTimeSpan / 3;
    TEST_ASSERT_EQUAL(large_seconds / 3, result.totalseconds());
}

// Test robustness with edge values
void test_edge_cases() {
    // Minimum value (0 seconds)
    TimeSpanExt zeroTimeSpan(0);
    TEST_ASSERT_EQUAL(0, zeroTimeSpan.totalseconds());
    TEST_ASSERT_TRUE(zeroTimeSpan == TimeSpanExt(0));

    // Maximum valid value
    int32_t max_seconds = INT32_MAX;
    TimeSpanExt maxTimeSpan(max_seconds);
    TEST_ASSERT_EQUAL(max_seconds, maxTimeSpan.totalseconds());

    // Operations with maximum values
    TimeSpanExt result = maxTimeSpan + TimeSpanExt(1);
    TEST_ASSERT_EQUAL(INT32_MAX, result.totalseconds());

    // Negative values (negative time span no issue, there is only no negative time)
    TimeSpanExt negativeTimeSpan = TimeSpanExt(-1);
    TEST_ASSERT_EQUAL(-1, negativeTimeSpan.totalseconds());
}


int runAllTests() {
    UNITY_BEGIN();

    // Run tests
    RUN_TEST(test_default_constructor);
    RUN_TEST(test_parameterized_constructor);
    RUN_TEST(test_comparison_operators);
    RUN_TEST(test_addition_operator);
    RUN_TEST(test_subtraction_operator);
    RUN_TEST(test_multiplication_operator);
    RUN_TEST(test_division_operator);
    RUN_TEST(test_negative_results);
    RUN_TEST(test_print_method);
    RUN_TEST(test_integration_with_TimeSpan);
    RUN_TEST(test_large_values);
    RUN_TEST(test_near_overflow);
    RUN_TEST(test_large_scalar_division);
    RUN_TEST(test_edge_cases);

    UNITY_END();

    return 0;
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