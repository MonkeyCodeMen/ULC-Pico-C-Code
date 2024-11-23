
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


// MockRTC_DS1307.hpp
#pragma once

#include <Arduino.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_BusIO_Register.h>
#include <RTClib.h>

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
