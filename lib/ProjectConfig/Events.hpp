
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


#pragma once
#include <Arduino.h>

enum EventType {   EVENT_NONE,
                    EVENT_ENTER,EVENT_RIGHT,EVENT_LEFT,EVENT_UP,EVENT_DOWN,
                    EVENT_A1,EVENT_A2,EVENT_A3,EVENT_A4,EVENT_A5,
                    EVENT_B1,EVENT_B2,EVENT_B3,EVENT_B4,EVENT_B5,EVENT_B6,
                    EVENT_A1_DOUBLE,EVENT_A2_DOUBLE,EVENT_A3_DOUBLE,EVENT_A4_DOUBLE,EVENT_A5_DOUBLE,
                    EVENT_B1_DOUBLE,EVENT_B2_DOUBLE,EVENT_B3_DOUBLE,EVENT_B4_DOUBLE,EVENT_B5_DOUBLE,EVENT_B6_DOUBLE,
                    EVENT_A1_LONG,EVENT_A2_LONG,EVENT_A3_LONG,EVENT_A4_LONG,EVENT_A5_LONG,
                    EVENT_B1_LONG,EVENT_B2_LONG,EVENT_B3_LONG,EVENT_B4_LONG,EVENT_B5_LONG,EVENT_B6_LONG
                };


EventType eventNameToNr(const char * name);



