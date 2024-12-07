
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


#define I2C_ADR_SLAVE       0x58
#define PWM_CHANNELS        12
#define CHAR_ARRAY_LENGTH   5
/////////////////////////////////////////////////////
// slave receive register Bank
struct I2C_SlaveReceiveReg_Struct{
    uint8_t PWM_soll[PWM_CHANNELS];
};

/////////////////////////////////////////////////////
// slave transmit register Bank
struct I2C_SlaveTransmitReg_Struct{
    uint8_t PWM_ist[PWM_CHANNELS];
};
extern I2C_SlaveReceiveReg_Struct  I2C_slaveSoll;
extern I2C_SlaveTransmitReg_Struct I2C_slaveIst;
