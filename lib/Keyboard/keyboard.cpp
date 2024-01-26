#include <keyboard.hpp>

Keyboard keyboard;


void Keyboard::begin(TwoWire *pBus,uint8_t subAdr,Mutex * pMutex){
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
    _pMutex->lock();
        _pBus->begin();  
    _pMutex->free();    
    
    _write16(MCP23X17_IODIR,0xFFFF);    // all pin as input
    _write16(MCP23X17_GPPU,0xFFFF);     // pull up for all pins 
}


Event_Type Keyboard::loop_getEvent(uint32_t now){
    uint16_t value = _read16(MCP23X17_GPIO);
    static uint16_t lastValue =0;

    if (lastValue != value){
        DUMP(F("new keyboard value"),(uint32_t)value,HEX);
        lastValue = value;

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
    //res.bval.L = (uint8_t)_pBus->read();
    //res.bval.H = (uint8_t)_pBus->read();
    uint16_t res;
    L_BYTE(res) = (uint8_t)_pBus->read();
    H_BYTE(res) = (uint8_t)_pBus->read();
    _pMutex->free();
    return res;
}


