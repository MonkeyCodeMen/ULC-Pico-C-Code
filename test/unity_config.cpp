
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


#include <Arduino.h>
#include "unity_config.h"
#define UNITY_PORT 1

#if UNITY_PORT == 0
    void myUnityOutputWait(void)                      { while(!Serial){} }
    void myUnityOutputStart(unsigned long baudrate)   { Serial.begin(baudrate); }
    void myUnityOutputChar(unsigned int c)            { Serial.write(c); }
    void myUnityOutputFlush(void)                     { Serial.flush(); }
    void myUnityOutputComplete(void)                  { Serial.end(); }

#elif UNITY_PORT == 1
    void myUnityOutputWait(void)                      { while(!Serial1){} }
    void myUnityOutputStart(unsigned long baudrate)   { Serial1.begin(baudrate); }
    void myUnityOutputChar(unsigned int c)            { Serial1.write(c); }
    void myUnityOutputFlush(void)                     { Serial1.flush(); }
    void myUnityOutputComplete(void)                  { Serial1.end(); }

#endif



