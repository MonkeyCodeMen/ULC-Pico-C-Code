// MockMutex.hpp
#pragma once

#include <Arduino.h>

class MockMutex {
public:
    MockMutex() : locked(false) {}

    // Mock lock function to simulate locking a resource
    void lock() {
        locked = true;
    }

    // Mock unlock function to simulate unlocking a resource
    void free() {
        locked = false;
    }

    // Mock tryLock function to simulate attempting to lock a resource
    bool tryLock() {
        if (!locked) {
            locked = true;
            return true;
        }
        return false;
    }

    // Check if the mutex is currently locked
    bool isLocked() const {
        return locked;
    }

    // Reset the state of the mock
    void reset() {
        locked = false;
    }

private:
    bool locked;  // Tracks the lock state
};
