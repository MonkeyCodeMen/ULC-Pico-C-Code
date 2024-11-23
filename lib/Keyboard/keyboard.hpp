
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
// #include "MCP23017.h" not used .. only single pin access read one pin needs one write and one read command 
// for 16 buttons it would need 48 bytes transfers per loop (adr+regAdr+data) x 16
// no interrupt support at my class 
#include <Wire.h>
#include <Mutex.hpp>
#include <Debug.hpp>
#include <Events.hpp>
#include <helper.h>
#include <RingBuffer.hpp>


#define MCP23X17_BASE_ADR   0x20

// define register in 16 Bit mode (Bit BANKS = 0 in REG IOCON = reset value  )
// first byte on bus for port A (assigned to low byte), second byte for pot B (assigned to high byte)
#define MCP23X17_IODIR      0x00  
#define MCP23X17_GPPU       0x0C
#define MCP23X17_GPIO       0x12

// the rest of register we do not support / need in this basic driver (not interrupt, no compare, no invers logic)
// just 16 bit polling


#define KEYBOARD_BUFFER_SIZE    10
#define KEYBOARD_COUNT          16

extern EventType keyboardStdMapping[KEYBOARD_COUNT];

class Keyboard{

    public:
        Keyboard():_deviceAdr(0xFF),_pBus(NULL),_pMutex(NULL),_dummyMutex(false)  {}
        ~Keyboard();

        void begin(TwoWire *pBus,uint8_t subAdr,Mutex * pMutex=NULL,EventType * pMapping=NULL);
        void loop(uint32_t now);
        EventType getNextEvent();

    private:
        uint8_t     _deviceAdr;
        TwoWire *   _pBus;
        Mutex   *   _pMutex;
        bool        _dummyMutex;

        uint8_t  _read(uint8_t regAdr);    
	    void     _write(uint8_t regAdr, uint8_t value);
        uint16_t _read16(uint8_t regAdr);
        void     _write16(uint8_t regAdr, uint16_t value);

        RingBuffer<EventType> * _pBuffer;
        EventType  _pressEvent[KEYBOARD_COUNT];
        EventType  _doublePressEvent[KEYBOARD_COUNT];
        EventType  _longPressEvent[KEYBOARD_COUNT];
};

extern Keyboard keyboard;
