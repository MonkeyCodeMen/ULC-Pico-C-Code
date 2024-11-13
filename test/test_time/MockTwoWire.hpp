// MockTwoWire.hpp
#pragma once

#include <Arduino.h>
#include <Wire.h>

class MockTwoWire :public TwoWire {
public:
    MockTwoWire() : TwoWire(nullptr,0,0), beginCalled(false) {}

    // Mock the `begin()` function to simulate initializing the I2C bus
    void begin() {
        beginCalled = true;
    }

    // Check if `begin` was called
    bool isBeginCalled() const {
        return beginCalled;
    }

    // Reset the state of the mock
    void reset() {
        beginCalled = false;
    }

private:
    bool beginCalled;  // Indicates if `begin` was called
};
