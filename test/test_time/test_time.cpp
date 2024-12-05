#include <unity.h>
#include "Time.hpp"

// Test default constructor
void test_default_constructor() {
    Time time;
    TEST_ASSERT_EQUAL(0, time.totalseconds());
    TEST_ASSERT_EQUAL(0, time.hour());
    TEST_ASSERT_EQUAL(0, time.minute());
    TEST_ASSERT_EQUAL(0, time.second());
}

// Test parameterized constructor
void test_parameterized_constructor() {
    Time time(12, 34, 56); // 12:34:56
    TEST_ASSERT_EQUAL((12 * SECONDS_PER_HOUR) + (34 * SECONDS_PER_MINUTE) + 56, time.totalseconds());
    TEST_ASSERT_EQUAL(12, time.hour());
    TEST_ASSERT_EQUAL(34, time.minute());
    TEST_ASSERT_EQUAL(56, time.second());
}

// Test TimeSpan constructor
void test_timespan_constructor() {
    TimeSpan span(0, 12, 34, 56); // 12:34:56
    Time time(span);

    TEST_ASSERT_EQUAL(span.totalseconds() % SECONDS_PER_DAY, time.totalseconds());
    TEST_ASSERT_EQUAL(12, time.hour());
    TEST_ASSERT_EQUAL(34, time.minute());
    TEST_ASSERT_EQUAL(56, time.second());
}

// Test TimeSpanExt constructor
void test_timespan_ext_constructor() {
    TimeSpanExt span(1, 12, 34, 56); // 1 day, 12:34:56
    Time time(span);

    TEST_ASSERT_EQUAL(span.totalseconds() % SECONDS_PER_DAY, time.totalseconds());
    TEST_ASSERT_EQUAL(12, time.hour());
    TEST_ASSERT_EQUAL(34, time.minute());
    TEST_ASSERT_EQUAL(56, time.second());
}

// Test addition operator
void test_addition_operator() {
    Time t1(10, 0, 0); // 10:00:00
    Time t2(2, 30, 0); // 02:30:00

    Time result = t1 + t2;
    TEST_ASSERT_EQUAL((10 * SECONDS_PER_HOUR) + (2 * SECONDS_PER_HOUR) + (30 * SECONDS_PER_MINUTE), result.totalseconds());
    TEST_ASSERT_EQUAL(12, result.hour());
    TEST_ASSERT_EQUAL(30, result.minute());
    TEST_ASSERT_EQUAL(0, result.second());
}

// Test subtraction operator
void test_subtraction_operator() {
    Time t1(10, 0, 0); // 10:00:00
    Time t2(2, 30, 0); // 02:30:00

    Time result = t1 - t2;
    TEST_ASSERT_EQUAL(((10 * SECONDS_PER_HOUR) - (2 * SECONDS_PER_HOUR) - (30 * SECONDS_PER_MINUTE)) % SECONDS_PER_DAY, result.totalseconds());
    TEST_ASSERT_EQUAL(7, result.hour());
    TEST_ASSERT_EQUAL(30, result.minute());
    TEST_ASSERT_EQUAL(0, result.second());
}

// Test multiplication operator
void test_multiplication_operator() {
    Time time(1, 0, 0); // 01:00:00

    Time result = time * 3;
    TEST_ASSERT_EQUAL((1 * SECONDS_PER_HOUR) * 3, result.totalseconds());
    TEST_ASSERT_EQUAL(3, result.hour());
    TEST_ASSERT_EQUAL(0, result.minute());
    TEST_ASSERT_EQUAL(0, result.second());
}

// Test division operator
void test_division_operator() {
    Time time(3, 0, 0); // 03:00:00

    Time result = time / 3;
    TEST_ASSERT_EQUAL((3 * SECONDS_PER_HOUR) / 3, result.totalseconds());
    TEST_ASSERT_EQUAL(1, result.hour());
    TEST_ASSERT_EQUAL(0, result.minute());
    TEST_ASSERT_EQUAL(0, result.second());
}

// Test comparison operators
void test_comparison_operators() {
    Time t1(12, 0, 0); // 12:00:00
    Time t2(10, 30, 0); // 10:30:00

    TEST_ASSERT_TRUE(t1 > t2);
    TEST_ASSERT_TRUE(t2 < t1);
    TEST_ASSERT_TRUE(t1 >= t2);
    TEST_ASSERT_TRUE(t2 <= t1);
    TEST_ASSERT_FALSE(t1 == t2);
    TEST_ASSERT_TRUE(t1 != t2);
}

// Test setHour, setMinute, setSec
void test_set_methods() {
    Time time;

    time.setHour(15);
    TEST_ASSERT_EQUAL(15, time.hour());
    TEST_ASSERT_EQUAL(0, time.minute());
    TEST_ASSERT_EQUAL(0, time.second());

    time.setMinute(45);
    TEST_ASSERT_EQUAL(15, time.hour());
    TEST_ASSERT_EQUAL(45, time.minute());
    TEST_ASSERT_EQUAL(0, time.second());

    time.setSec(30);
    TEST_ASSERT_EQUAL(15, time.hour());
    TEST_ASSERT_EQUAL(45, time.minute());
    TEST_ASSERT_EQUAL(30, time.second());
}

// Test edge cases
void test_edge_cases() {
    // Test wraparound at SECONDS_PER_DAY
    Time time1(23, 59, 59); // 23:59:59
    Time time2(0, 0, 1);    // 00:00:01

    Time result = time1 + time2; // Should wrap around to 00:00:00
    TEST_ASSERT_EQUAL(0, result.hour());
    TEST_ASSERT_EQUAL(0, result.minute());
    TEST_ASSERT_EQUAL(0, result.second());

    // Test negative seconds handling
    Time time3(-1); // Should wrap around to 23:59:59
    TEST_ASSERT_EQUAL(23, time3.hour());
    TEST_ASSERT_EQUAL(59, time3.minute());
    TEST_ASSERT_EQUAL(59, time3.second());
}

void test_toFixedWidthText() {
    Time time(12, 34, 56);

    TEST_ASSERT_EQUAL_STRING("12:34:56", time.toFixedWidthText().c_str());
    TEST_ASSERT_EQUAL_STRING("12-34-56", time.toFixedWidthText('-',0).c_str());
    TEST_ASSERT_EQUAL_STRING(" 12-34-56", time.toFixedWidthText('-', ' ').c_str());
}

void test_toVariableWidthText() {
    Time time(0, 5, 8);

    TEST_ASSERT_EQUAL_STRING("0:5:8", time.toVariableWidthText().c_str());
    TEST_ASSERT_EQUAL_STRING("0:5:8", time.toVariableWidthText(':', 0).c_str());
    TEST_ASSERT_EQUAL_STRING(" 0:5:8", time.toVariableWidthText(':', ' ').c_str());
}

void test_overflow_handling() {
    Time time1(23, 59, 59);
    Time time2(0, 0, 2); // Add 2 seconds to cause wraparound

    Time result = time1 + time2;
    TEST_ASSERT_EQUAL(0, result.hour());
    TEST_ASSERT_EQUAL(0, result.minute());
    TEST_ASSERT_EQUAL(1, result.second());

    Time negative(-1); // Should wrap to 23:59:59
    TEST_ASSERT_EQUAL(23, negative.hour());
    TEST_ASSERT_EQUAL(59, negative.minute());
    TEST_ASSERT_EQUAL(59, negative.second());
}


int runAllTests() {
    UNITY_BEGIN();

    RUN_TEST(test_default_constructor);
    RUN_TEST(test_parameterized_constructor);
    RUN_TEST(test_timespan_constructor);
    RUN_TEST(test_timespan_ext_constructor);
    RUN_TEST(test_addition_operator);
    RUN_TEST(test_subtraction_operator);
    RUN_TEST(test_multiplication_operator);
    RUN_TEST(test_division_operator);
    RUN_TEST(test_comparison_operators);
    RUN_TEST(test_set_methods);
    RUN_TEST(test_edge_cases);
    RUN_TEST(test_toFixedWidthText);
    RUN_TEST(test_toVariableWidthText);
    RUN_TEST(test_overflow_handling);

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
