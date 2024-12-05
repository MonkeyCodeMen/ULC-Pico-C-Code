#include <unity.h>
#include "TimerManager.hpp"


// Mock classes for Time and BufferedClock
// you must move this clock by hand ... hey you can manipulate the time ;-)
class MockBufferedClock {
public:
    DateTime value;
    void set(DateTime newValue) {value=newValue;}
    DateTime getLoopDateTime() {return value;}
};

// Override the global instance used in TimerManager
MockBufferedClock bufferedClock;

// Test initialization with an empty TimerManager
void test_initialization_empty() {
    TimerManager manager;
    TEST_ASSERT_TRUE(manager.switchedOff());
    TEST_ASSERT_EQUAL(0, manager.getTimerList().size());
}

// Test initialization with a string of timers
void test_initialization_string() {
    TimerManager manager("8:30-120,18:45-180");
    TEST_ASSERT_EQUAL(2, manager.getTimerList().size());
}

// Test adding timers via vector
void test_add_timer_vector() {
    std::vector<TimerDef> timers = {
        TimerDef(8, 30, 120),
        TimerDef(18, 45, 180)
    };

    TimerManager manager;
    manager.newEventList(timers);

    TEST_ASSERT_EQUAL(2, manager.getTimerList().size());
    TEST_ASSERT_FALSE(manager.switchedOn());
}

// Test adding a single timer
void test_add_single_timer() {
    TimerManager manager;
    manager.addEvent(8, 30, 120);

    TEST_ASSERT_EQUAL(1, manager.getTimerList().size());
    TEST_ASSERT_FALSE(manager.switchedOn());
}

// Test switchedOn and switchedOff states
void test_switched_states() {
    TimerManager manager("8:30-120,18:45-180");

    // Mock current time
    bufferedClock.set(DateTime(2022,12,24,8,45,0));
    //bufferedClock.getLoopDateTime = []() { return Time(8, 45, 0); }; // During the first timer
    manager.loop();
    TEST_ASSERT_TRUE(manager.switchedOn());

    bufferedClock.set(DateTime(2022,12,24,10,45,0));
    //bufferedClock.getLoopDateTime = []() { return Time(10, 45, 0); }; // After the first timer ends
    manager.loop();
    TEST_ASSERT_FALSE(manager.switchedOn());
    TEST_ASSERT_TRUE(manager.switchedOff());
}

// Test timer list parsing from a string
void test_parse_timer_list() {
    TimerManager manager;
    int result = manager.newEventList("8:30-120,18:45-180");

    TEST_ASSERT_EQUAL(TIMER_MANAGER_OK, result);
    TEST_ASSERT_EQUAL(2, manager.getTimerList().size());
}

// Test invalid configurations
void test_invalid_timer_list() {
    TimerManager manager;
    int result = manager.newEventList("25:30-120,18:61-180");

    TEST_ASSERT_EQUAL(TIMER_MANAGER_INVALID_CONFIG, result);
    TEST_ASSERT_EQUAL(0, manager.getTimerList().size());
}

// Test the loop function with a full timer schedule
void test_full_timer_loop() {
    TimerManager manager("8:30-120,12:00-90");

    bufferedClock.set(DateTime(2022,12,24,8,45,0));
    //bufferedClock.getLoopDateTime = []() { return Time(8, 45, 0); }; // During the first timer
    manager.loop();
    TEST_ASSERT_TRUE(manager.switchedOn());

    bufferedClock.set(DateTime(2022,12,24,12,45,0));
    //bufferedClock.getLoopDateTime = []() { return Time(12, 45, 0); }; // During the second timer
    manager.loop();
    TEST_ASSERT_TRUE(manager.switchedOn());

    bufferedClock.set(DateTime(2022,12,24,14,0,0));
    //bufferedClock.getLoopDateTime = []() { return Time(14, 0, 0); }; // After all timers
    manager.loop();
    TEST_ASSERT_TRUE(manager.switchedOff());
}

int runAllTests() {
    UNITY_BEGIN();

    RUN_TEST(test_initialization_empty);
    RUN_TEST(test_initialization_string);
    RUN_TEST(test_add_timer_vector);
    RUN_TEST(test_add_single_timer);
    RUN_TEST(test_switched_states);
    RUN_TEST(test_parse_timer_list);
    RUN_TEST(test_invalid_timer_list);
    RUN_TEST(test_full_timer_loop);

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