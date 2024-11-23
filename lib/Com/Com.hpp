
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
#include "ComFrame.hpp"
#include "ComDispatch.hpp"

class Com
{
public:
	Com(){}
	~Com() = default;

    void begin(HardwareSerial * pPort, int baudRate = 115200, uint16_t config = SERIAL_8N1 );
    void loop();
    void reset();
    void sendAnswer(bool res,ComFrame * pFrame);

private:
    enum ComState  {WAIT,START_FRAME,MODULE,INDEX,COMMAND,PAR1,PAR2,PAR3,PAR4,STR_START,STR_DATA,STR_END,FRAME_DONE};
    HardwareSerial * _pPort;
    ComState _state; 

    void doWaiting();
    void getStartOfFrame();
    void getModule();
    void getIndex();
    void getCommand();
    void getPar1();
    void getPar2();
    void getPar3();
    void getPar4();
    void getStrStart();
    void getStrData();
    void getStrEnd();
    void frameDone();

    bool getByte(uint8_t * pBuffer);
    bool getBytes(uint32_t count,uint8_t * pBuffer);
    bool collectField();
    
    uint8_t     _byte;
    String      _field;
    uint32_t    _maxFieldLength;
    uint32_t    _dataReceived;
    bool        _endFound;

    ComFrame    _frame;
    ComDispatch _dispatcher;
};


extern Com com;   