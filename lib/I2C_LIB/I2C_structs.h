
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


///////////////////////////////////////////////////
// base regs
struct I2C_slave_CtrlReg_struct {
        unsigned int  EN_WRITE:1;
        unsigned int  STREAM:1;
        unsigned int  REG:1;
        unsigned int  REG_CRC_CHECK:1;
        unsigned int  REG_CRC_ERROR:1;
        unsigned int  LED_BLINK:1;
        unsigned int  LOGS:1;
        unsigned int  LOGS_BASE:1;
        unsigned int  LOGS_CTRL:1;
        unsigned int  LOGS_ISR:1;
        unsigned int  LOGS_ERROR:1;
        unsigned int  LOGS_STREAM:1;
        unsigned int  LOGS_REG:1;
        unsigned int  USER_1:1;
        unsigned int  USER_2:1;
        unsigned int  USER_3:1;
        unsigned int  USER_4:1;
        unsigned int  USER_5:1;
        unsigned int  USER_6:1;
        uint32_t LOGS_TIME;
};


///////////////////////////////////////////////////
// info structs
struct I2C_statistic_struct{
    uint32_t lastTransactionTime_usec;
};


// slave error counters
struct I2C_slave_errorCounter_struct{
    uint8_t invalidCmd;
    uint8_t invalidMode;
    uint8_t sequence;
    uint8_t internal;
};

// slave ISR counter
struct I2C_slave_ISRcounter_struct{
    uint8_t receiveEvents;
    uint8_t requestEvents;
};



// stream buffer info'S
struct I2C_slave_stream_info{
    uint8_t inBufferSize;
    uint8_t inBufferCount;
    uint8_t inBufferFree;
    uint8_t inBufferUnderrun;        // cleared on read over I2C
    uint8_t inBufferOverrun;         // cleared on read over I2C
    uint8_t inBufferMax;             // cleared on read over I2C

    uint8_t outBufferSize;
    uint8_t outBufferCount;
    uint8_t outBufferFree;
    uint8_t outBufferUnderrun;       // cleared on read over I2C
    uint8_t outBufferOverrun;        // cleared on read over I2C
    uint8_t outBufferMax;            // cleared on read over I2C
};

// stream buffer info's part that will be stored
// we need only to store this in the slave .. rest will be generatd on the fly
struct I2C_slave_stream_info_memory{
    uint8_t inBufferUnderrun;        // cleared on read over I2C
    uint8_t inBufferOverrun;         // cleared on read over I2C
    uint8_t inBufferMax;             // cleared on read over I2C

    uint8_t outBufferUnderrun;       // cleared on read over I2C
    uint8_t outBufferOverrun;        // cleared on read over I2C
    uint8_t outBufferMax;            // cleared on read over I2C
};

