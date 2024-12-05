
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
