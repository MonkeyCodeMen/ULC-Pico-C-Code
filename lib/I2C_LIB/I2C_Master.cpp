
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


#include <I2C_master.hpp>

I2C_master i2c_master;

I2C_master::I2C_master():_pBus(NULL),_pMutex(NULL),_dummyMutex(false){
    memset(&_stats,0,sizeof(I2C_statistic_struct));
}

I2C_master::~I2C_master(){
    if (_dummyMutex == true)    delete _pMutex;
}


String I2C_master::dumpInstance(){
    String out="";
    return out;
}

String I2C_master::dumpCtrlReg(I2C_slave_CtrlReg_struct * pSrc){
    String out="";
    if (pSrc == NULL){
        out = "ERROR: invalid pointer to ctrl register content ";
        return out;
    }
    return out;
}


//////////////////////////////////////////
// setup / begin / Loop
//////////////////////////////////////////

bool I2C_master::begin(TwoWire * pBus,Mutex * pMutex){
    if (pBus == NULL) return false;
    if (pMutex == NULL){
        _pMutex = new Mutex();
        if (_pMutex == NULL) return false;
        _dummyMutex = true;
    } else {
        _pMutex = pMutex;
    }
    _pBus = pBus;
    _pBus->setTimeout(50);
    return true;
}


//////////////////////////////////////////
// base communication
//////////////////////////////////////////

bool I2C_master::readStatusReg(uint8_t slaveAdr,I2C_slave_CtrlReg_struct * pDest){
    if (_pBus == NULL)      return false;
    if (slaveAdr >= 0x80)   return false;
    if (pDest == NULL)      return false;

    I2C_slave_CtrlReg_struct temp;
    uint8_t size = sizeof(I2C_slave_CtrlReg_struct);
    memset(&temp,0,size);

    uint32_t start = micros();
    _pBus->beginTransmission(slaveAdr);
    _pBus->write(I2C_ID_BASE_CTRL_STATUS);
    _pBus->write((uint8_t *)&temp,size);
    _pBus->endTransmission(false);
    uint8_t res = _readFrame(slaveAdr,size,(uint8_t*) pDest,true);
    _stats.lastTransactionTime_usec = micros()-start;

    _flushLibBuffer();

    if (res != size) return false;
    return true;
}

bool I2C_master::writeCtrlReg(uint8_t slaveAdr,I2C_slave_CtrlReg_struct * pSrc,bool setChangeBit,I2C_slave_CtrlReg_struct * pDest){
    if (_pBus == NULL)      return false;
    if (slaveAdr >= 0x80)   return false;
    if (pSrc  == NULL)      return false;

    uint8_t * p;
    bool tempBuffer;
    uint8_t size = sizeof(I2C_slave_CtrlReg_struct);
    if (pDest == NULL){
        p = new uint8_t[size];
        tempBuffer = true;
    } else {
        p = (uint8_t*) pDest;
        tempBuffer = false;
    }
    if (setChangeBit == true){
        pSrc->EN_WRITE=1;
    }

    uint32_t start = micros();
    _pBus->beginTransmission(slaveAdr);
    _pBus->write(I2C_ID_BASE_CTRL_STATUS);
    _pBus->write((uint8_t *)pSrc,size);
    _pBus->endTransmission(false);
    uint8_t res = _readFrame(slaveAdr,size,(uint8_t*) pDest,true);
    _stats.lastTransactionTime_usec = micros()-start;

    _flushLibBuffer();

    if (tempBuffer == true) delete p;
    if (res != size) return false;
    return true;
}


uint32_t I2C_master::readUpTime(uint8_t slaveAdr){
    if (_pBus == NULL)      return 0xFFFFFFFF;
    if (slaveAdr >= 0x80)   return 0xFFFFFFFF;
    uint32_t start = micros();

    _pBus->beginTransmission(slaveAdr);
    _pBus->write(I2C_ID_BASE_LIFE_TIME);
    _pBus->endTransmission(false);
    uint32_t value;
    uint8_t *p = (uint8_t*)&value;
    uint8_t res = _readFrame(slaveAdr,4,p,true);

    _stats.lastTransactionTime_usec = micros()-start;
    _flushLibBuffer();

    if (res != 4)            return 0xFFFFFFFF;
    return value;
}

bool I2C_master::readErrorCounter(uint8_t slaveAdr,I2C_slave_errorCounter_struct * pDest,bool clearAfterRead){
    if (_pBus == NULL)      return false;
    if (slaveAdr >= 0x80)   return false;
    if (pDest == NULL)      return false;
    uint32_t start = micros();
    uint8_t size = sizeof(I2C_slave_errorCounter_struct);

    _pBus->beginTransmission(slaveAdr);
    if (clearAfterRead == true){
        _pBus->write(I2C_ID_BASE_READ_AND_CLEAR_ERROR);
    } else {
        _pBus->write(I2C_ID_BASE_READ_ERROR);
    }
    _pBus->endTransmission(false);
    uint8_t res = _readFrame(slaveAdr,size,(uint8_t*)pDest,true);

    _stats.lastTransactionTime_usec = micros()-start;
    _flushLibBuffer();

    if (res != size)            return false;
    return true;
}


bool I2C_master::readISRInfo(uint8_t slaveAdr,I2C_slave_ISRcounter_struct * pDest){
    if (_pBus == NULL)      return false;
    if (slaveAdr >= 0x80)   return false;
    if (pDest == NULL)      return false;
    uint32_t start = micros();
    uint8_t size = sizeof(I2C_slave_ISRcounter_struct);

    _pBus->beginTransmission(slaveAdr);
    _pBus->write(I2C_ID_BASE_INFO_ISR);
    _pBus->endTransmission(false);
    uint8_t res = _readFrame(slaveAdr,size,(uint8_t*)pDest,true);

    _stats.lastTransactionTime_usec = micros()-start;
    _flushLibBuffer();

    if (res != size)            return false;
    return true;

}



//////////////////////////////////////////
// stream read write
//////////////////////////////////////////

bool I2C_master::stream_getInfo(uint8_t slaveAdr,I2C_slave_stream_info * pDest){
    if (_pBus == NULL)      return false;
    if (slaveAdr >= 0x80)   return false;

    _pMutex->lock();
    uint32_t start = micros();
    _pBus->beginTransmission(slaveAdr);
    _pBus->write(I2C_ID_STREAM_INFO);
    _pBus->endTransmission(false);
    uint8_t res = _readFrame(slaveAdr,sizeof(I2C_slave_stream_info),(uint8_t*)pDest,true);
    
    _stats.lastTransactionTime_usec = micros()-start;
    _flushLibBuffer();
    _pMutex->free();

    if (res != sizeof(I2C_slave_stream_info)){
        return false;
    }
    return true;
}

bool I2C_master::stream_write(uint8_t slaveAdr,uint8_t* pData,uint8_t length){
    if (_pBus == NULL)      return false;
    if (slaveAdr >= 0x80)   return false;
    if (length == 0){
        _stats.lastTransactionTime_usec = 0;
        return true; // nothing to do 
    }   
    _pMutex->lock();

    uint32_t start = micros();
    _pBus->beginTransmission(slaveAdr);
    _pBus->write(I2C_ID_STREAM_WRITE);
    _pBus->write(length);

    uint8_t count = TWO_WIRE_BUFFER_SIZE - 2;
    uint8_t send = 0,rest;
    while (send < length){
        rest = length-send;
        count = min(rest,count);
        _pBus->write(pData+send,count);
        send+=count;
        if (send >= length){
            _pBus->endTransmission(true);   // last one
        } else {
            _pBus->endTransmission(false);   // more will folllow
            count = TWO_WIRE_BUFFER_SIZE;
            _pBus->beginTransmission(slaveAdr);
        }
    }
    _stats.lastTransactionTime_usec = micros()-start;
    _flushLibBuffer();   // ignore rest of frame
    _pMutex->free();

    return true;
}

int I2C_master::stream_read(uint8_t slaveAdr,uint8_t* pDest,uint8_t length){
    if (_pBus == NULL)      return -1;
    if (slaveAdr >= 0x80)   return -2;
    if (pDest == NULL)      return -3;
    if (length == 0){
        _stats.lastTransactionTime_usec = 0;
        return 0; // nothing to do 
    }   
    _pMutex->lock();
    uint32_t start = micros();
    _pBus->beginTransmission(slaveAdr);
    _pBus->write(I2C_ID_STREAM_READ);
    _pBus->write(length);
    _pBus->endTransmission(false);   // more will folllow

    // read available length
    uint8_t available,size;
    if (_readOneByte(slaveAdr,&available,false) == false)   { _pBus->endTransmission(true); _pMutex->free(); return -10;         }
    if (available == 0)                                     { _pBus->endTransmission(true); _pMutex->free(); return 0;           }
    size = min(length,available);

    uint8_t received,rest,res;
    received = 0;
    while (received < size){
        rest = size-received;
        rest = min(rest,TWO_WIRE_BUFFER_SIZE);
        res = _readFrame(slaveAdr,rest,pDest+received,(int)false);
        received+=res;
        if (res != rest)      { break;   }
    }
    _pBus->endTransmission(true); 
    _stats.lastTransactionTime_usec = micros()-start;
    _pMutex->free();
    if (received != size){
      return -11;
    } 
    return size;
}

bool I2C_master::stream_flushAllBuffers(uint8_t slaveAdr){
    if (_pBus == NULL)      return false;
    if (slaveAdr >= 0x80)   return false;
    _pMutex->lock();
    uint32_t start = micros();
    _pBus->beginTransmission(slaveAdr);
    _pBus->write(I2C_ID_STREAM_FLUSH);
    _pBus->endTransmission(true);
    _stats.lastTransactionTime_usec = micros()-start;
    _flushLibBuffer();
    _pMutex->free();
    return true;
}



//////////////////////////////////////////
// reg read write
//////////////////////////////////////////

// access reg slaves with connected register set
bool I2C_master::write_regSet(uint8_t slaveAdr,void * pReg,uint8_t length,bool CRC){
    if (_pBus == NULL)      return false;
    if (slaveAdr >= 0x80)   return false;
    if (length == 0){
        _stats.lastTransactionTime_usec = 0;
        return true; // nothing to do 
    }   
    if (pReg == NULL)       return false;

    uint32_t start = micros();
    uint8_t * p = (uint8_t*) pReg;
    uint8_t crc;
    if (CRC == true){
        crc = calcCRC8(p,length);
    } else {
        crc = 0x55;
    }
    _pBus->beginTransmission(slaveAdr);
    _pBus->write(I2C_ID_REG_WRITE);
    _pBus->write(0);        // start index (for partial update later)
    _pBus->write(length);
    _pBus->write(crc);

    uint8_t frameLength = TWO_WIRE_BUFFER_SIZE - 4;
    uint8_t send = 0,rest;
    while (send < length){
        rest = length-send;
        if (rest < frameLength)  {
            frameLength = rest;
        }
        _pBus->write(p+send,frameLength);
        send+=frameLength;
        if (send == length){
            _pBus->endTransmission(false);   // last one but keep bus open for CRC
        } else {
            _pBus->endTransmission(false);   // more will folllow
            frameLength = TWO_WIRE_BUFFER_SIZE;
            _pBus->beginTransmission(slaveAdr);
        }
    }
    _pBus->endTransmission(true);
    _stats.lastTransactionTime_usec = micros()-start;
    _flushLibBuffer();   // ignore rest of frame
    return true;
}

int  I2C_master::read_regSet(uint8_t slaveAdr,void* pDest,uint8_t length,bool CRC){
    if (_pBus == NULL)          return -1;
    if (slaveAdr >= 0x80)       return -2;
    if (pDest == NULL)          return -3;
    if (length == 0){
        _stats.lastTransactionTime_usec = 0;
        return 0; // nothing to do 
    }   
    uint8_t * p;
    p = new uint8_t[length];
    if (p == NULL)              return -4;

    uint32_t start = micros();
    _pBus->beginTransmission(slaveAdr);
    _pBus->write(I2C_ID_REG_READ);
    _pBus->write(0);        // start index 
    _pBus->write(length);   // length
    _pBus->endTransmission(false);

    // read available length
    uint8_t available;
    uint8_t size;  
    uint8_t received,rest,res;
    uint8_t crcReceived,crcCalc;

    if (_readOneByte(slaveAdr,&available,false) == false)   goto error; 
    if (_readOneByte(slaveAdr,&crcReceived,false) == false) goto error; 
    length = min(length,available);
    received = 0;
    while (received < length){
        rest = length-received;
        size = min(TWO_WIRE_BUFFER_SIZE,rest);
        res = _readFrame(slaveAdr,size,p+received,(int)false);
        received+=res;
        //if (res != size)   goto error;
    }
    _pBus->endTransmission(true); 
    _stats.lastTransactionTime_usec = micros()-start;

    if (CRC == false) { 
        // do not perform CRC check
        memcpy((uint8_t*)pDest,p,length);
        delete p;
        return length;   
    } else {
        crcCalc = calcCRC8(p,length);
        if (crcCalc == crcReceived){
            // CRC OK: copy to dest 
            memcpy((uint8_t*)pDest,p,length);
            delete p;
            return length;
        } else {
            // CRC NOK: do not update data at destination
            delete p;
            return -11;
        }
    }

    error:
        _pBus->endTransmission(true); 
        _stats.lastTransactionTime_usec = micros()-start;
        delete p;
        return -10;
}


//////////////////////////////////////////
// private member functions
//////////////////////////////////////////
void I2C_master::_flushLibBuffer(){
    while(_pBus->available())   _pBus->read();
    //_pBus->clearWireTimeoutFlag();
}

bool I2C_master::_readOneByte(uint8_t slaveAdr,uint8_t* pData,bool sendStop){
    _pBus->requestFrom((int)slaveAdr,(int)1,(int)sendStop); // keep bus busy
    int res = _pBus->available();
    if(res < 1)                         return false;
    int value = _pBus->read();
    if ((value < 0) || (value > 255))   return false;
    *pData = (uint8_t)value;
    return true;
}


uint8_t I2C_master::_readFrame(uint8_t slaveAdr,uint8_t count,uint8_t* pDest,bool sendStop){
    // private function .. no parameter check !!
    // but count must be smaller than TwoWire buffer size
    int res,value;
    uint8_t i;
    _pBus->requestFrom((int)slaveAdr,(int)count,(int)sendStop);
    res = _pBus->available();
    if(res < count){
        res = count;
    } 
    if (res == 0){
        return 0;
    }

    for(i=0;i < res;i++){
        value = _pBus->read();
        if ((value <0 ) || (value > 255)){
            // error during buffer access
            return i;
        } else {
            *pDest = (uint8_t) value;
            pDest++;
        }
    }
    return res;
}

