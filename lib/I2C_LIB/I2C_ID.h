
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

////////////////////////////////////////////////////
#define TWO_WIRE_BUFFER_SIZE    32  // buffer size of utilized TwoWire LIB



/////////////////////////////////////////////////////////////#
// I2C protocoll ID
// CMD ID .. do not use enum to have more control about real values 
//   ==> stabalize Interface between master and slave
#define I2C_ID_RESERVED_1       0x00

#define I2C_ID_BASE_CTRL_STATUS 0x01
#define I2C_ID_BASE_LIFE_TIME   0x02
#define I2C_ID_BASE_READ_ERROR  0x03 
#define I2C_ID_BASE_INFO_ISR    0x04
#define I2C_ID_BASE_READ_AND_CLEAR_ERROR 0x0F

#define I2C_ID_STREAM_WRITE     0x12
#define I2C_ID_STREAM_READ      0x13
#define I2C_ID_STREAM_INFO      0x1E
#define I2C_ID_STREAM_FLUSH     0x1F

#define I2C_ID_REG_WRITE        0x22
#define I2C_ID_REG_READ         0x23
#define I2C_ID_REG_CRC_VALUES   0x2D
#define I2C_ID_REG_RESET        0x2F

#define I2C_ID_RESERVED_2       0xFF


