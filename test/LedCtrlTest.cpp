#include <Arduino.h>
#include "Led.hpp"
#include "LedCtrl.hpp"
#include "StringList.hpp"

#include "unity.h"

class LedSim : public Led{
        public:
        LedSim()                {_value=0;};
        ~LedSim() = default;
        void set(u8_t value)    {_value = value;};
        u8_t _value;
};


void test_LedCtrl_constructor(void) {
    LedSim simLed;
    LedCtrl object((Led*)simLed);

    TEST_ASSERT_EQUAL_STRING("off",object.getName().c_str());
    TEST_ASSERT_EQUAL_UINT8( 0 , simLed._value);
    String aniList = object.getNameList();
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32( 5 , aniList.length());

    StringList list(aniList,',');



}


// collect all tests of this file to one collection
void test_collection_LedCtrl(void) {
  RUN_TEST(test_LedCtrl_constructor);
  RUN_TEST();
  RUN_TEST();
  RUN_TEST();
}
