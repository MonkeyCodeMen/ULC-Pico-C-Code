#pragma once

#include <Arduino.h>
// #include "MCP23017.h" not used .. only single pin access read one pin needs one write and one read command 
// for 16 buttons it would need 48 bytes transfers per loop (adr+regAdr+data) x 16
// no interrupt support at my class 
#include <Wire.h>
#include <Mutex.hpp>
#include <Debug.hpp>
#include <Events.h>
#include <helper.h>


#define MCP23X17_BASE_ADR   0x20

// define register in 16 Bit mode (Bit BANKS = 0 in REG IOCON = reset value  )
// first byte on bus for port A (assigned to low byte), second byte for pot B (assigned to high byte)
#define MCP23X17_IODIR      0x00  
#define MCP23X17_GPPU       0x0C
#define MCP23X17_GPIO       0x12

// the rest of register we do not support / need in this basic driver (not interrupt, no compare, no invers logic)
// just 16 bit polling




class Keyboard{

    public:
        Keyboard():_deviceAdr(0xFF),_pBus(NULL),_pMutex(NULL),_dummyMutex(false)  {}
        ~Keyboard() { if (_dummyMutex == true) delete _pMutex; }

        void begin(TwoWire *pBus,uint8_t subAdr,Mutex * pMutex=NULL);
        Event_Type loop_getEvent(uint32_t now);

    private:
        uint8_t     _deviceAdr;
        TwoWire *   _pBus;
        Mutex   *   _pMutex;
        bool        _dummyMutex;

        uint8_t  _read(uint8_t regAdr);    
	    void     _write(uint8_t regAdr, uint8_t value);
        uint16_t _read16(uint8_t regAdr);
        void     _write16(uint8_t regAdr, uint16_t value);


};

extern Keyboard keyboard;
