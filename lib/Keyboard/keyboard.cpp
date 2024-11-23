
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


#include <keyboard.hpp>

Keyboard keyboard;

EventType keyboardStdMapping[KEYBOARD_COUNT] = { 
    EVENT_DOWN,EVENT_ENTER,EVENT_B1,EVENT_B2,     EVENT_B3,EVENT_B4,EVENT_B5,EVENT_B6,
    EVENT_UP,EVENT_LEFT,EVENT_RIGHT,EVENT_A1,     EVENT_A2,EVENT_A3,EVENT_A4,EVENT_A5
    };


Keyboard::~Keyboard() { 
    if (_dummyMutex == true) delete _pMutex; 
    delete _pBuffer;
}


void Keyboard::begin(TwoWire *pBus,uint8_t subAdr,Mutex * pMutex,EventType * pMapping){
    if (pBus == NULL){
        LOG(F("invalid bus"));
        _pBus = NULL;
        _deviceAdr = 0xFF;
    }
    _deviceAdr = MCP23X17_BASE_ADR | (subAdr & 0x07);
    _pBus = pBus;
    if (pMutex == NULL){
        _pMutex = new Mutex();
        _dummyMutex = true;
    } else {
        _pMutex = pMutex;
    }

    _pBuffer = new RingBuffer<EventType>(KEYBOARD_BUFFER_SIZE);
    for(int i=0;i < KEYBOARD_COUNT;i++){
        if (pMapping == NULL){
            _pressEvent[i]      = EVENT_NONE;
        } else {
            _pressEvent[i]      = pMapping[i];
        }
        _doublePressEvent[i]    = EVENT_NONE;
        _longPressEvent[i]      = EVENT_NONE;
    }
    _write16(MCP23X17_IODIR,0xFFFF);    // all pin as input
    _write16(MCP23X17_GPPU,0xFFFF);     // pull up for all pins 
}



void Keyboard::loop(uint32_t now){
    uint16_t value = _read16(MCP23X17_GPIO);
    static uint16_t lastValue =0xFFFF;

    // for now very simple edge detection 
    // no debouncing, no long press ... 
    if (lastValue != value){
        uint16_t mask = 0x01;
        EventType event;
        for(int i=0;i < KEYBOARD_COUNT;i++){
            if (((mask & lastValue) != 0) && ((mask & value) == 0)){
                event = _pressEvent[i]; 
                if (event != EVENT_NONE){
                    _pBuffer->push(event);
                }
            }
            mask <<=1;
        }
        lastValue = value;
    }
}


EventType Keyboard::getNextEvent(){
    EventType next;
    if (_pBuffer->pop(&next) == true){
        return next;
    }
    return EVENT_NONE;
}



uint8_t Keyboard::_read(uint8_t regAdr) {
    _pMutex->lock();
	_pBus->beginTransmission(_deviceAdr);
	_pBus->write(regAdr);
	_pBus->endTransmission();
	_pBus->requestFrom(_deviceAdr, 1);
	uint8_t res = _pBus->read();
    _pMutex->free();
    return res;
}

void Keyboard::_write(uint8_t regAdr, uint8_t value) {
    _pMutex->lock();
	_pBus->beginTransmission(_deviceAdr);
	_pBus->write(regAdr);
	_pBus->write(value);
	_pBus->endTransmission();
    _pMutex->free();
}

void Keyboard::_write16(uint8_t regAdr, uint16_t value){
    // MCP23017 BANKS flag must be cleared (= reset value)
    _pMutex->lock();
	_pBus->beginTransmission(_deviceAdr);
	_pBus->write(regAdr);
    _pBus->write(L_BYTE(value));
	_pBus->write(H_BYTE(value));
	_pBus->endTransmission();
    _pMutex->free();

}

uint16_t Keyboard::_read16(uint8_t regAdr) {
    // MCP23017 BANKS flag must be cleared (= reset value)
    _pMutex->lock();
	_pBus->beginTransmission(_deviceAdr);
	_pBus->write(regAdr);
	_pBus->endTransmission();
	_pBus->requestFrom(_deviceAdr, 2);
    //uint16_t_byteAcess res;
    //res.byte.L = (uint8_t)_pBus->read();
    //res.byte.H = (uint8_t)_pBus->read();
    uint16_t res;
    L_BYTE(res) = (uint8_t)_pBus->read();
    H_BYTE(res) = (uint8_t)_pBus->read();
    _pMutex->free();
    return res;
}


