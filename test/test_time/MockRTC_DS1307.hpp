// MockRTC_DS1307.hpp
#pragma once

#include <RTClib.h>
#include <Arduino.h>

class MockRTC_DS1307{
public:
    MockRTC_DS1307() :initialized(false), failInit(false), currentTime(2024, 1, 1, 0, 0, 0) {}

    // Set whether the RTC initialization should fail
    void setFailInit(bool shouldFail) {
        failInit = shouldFail;
    }

    // Mock begin() function to simulate starting communication with the RTC
    bool begin(TwoWire *wire) {
        if (failInit) {
            initialized = false;
            return false;
        }
        initialized = true;
        return true;
    }

    // Mock now() function to simulate getting the current time from the RTC
    DateTime now() {
        return currentTime;
    }

    // Set the simulated current time in the RTC
    void setCurrentTime(const DateTime &time) {
        currentTime = time;
    }

    // Check if the RTC was initialized
    bool isInitialized() const {
        return initialized;
    }

    // Reset the state of the mock
    void reset() {
        initialized = false;
        failInit = false;
        currentTime = DateTime(2024, 1, 1, 0, 0, 0);
    }

private:
    bool initialized;     // Indicates if the RTC was initialized successfully
    bool failInit;        // Determines whether `begin` should fail
    DateTime currentTime; // Simulated current time
};
