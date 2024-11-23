
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
#include <Wire.h>
#include <Debug.hpp>

#include <I2C_ID.h>
#include <I2C_structs.h>

#include <RingBuffer.hpp>
//#include <Mutex.hpp>
#include <CRC8.h>
#include <CRC.h>
#include <Mutex.hpp>



class I2C_master
{
    public:
        I2C_master();
        ~I2C_master();
        bool begin(TwoWire * pBus,Mutex * pMutex=NULL);

        // base communication
        bool readStatusReg(uint8_t slaveAdr,I2C_slave_CtrlReg_struct * pDest);
        bool writeCtrlReg(uint8_t slaveAdr,I2C_slave_CtrlReg_struct * pSrc,bool setChangeBit=true,I2C_slave_CtrlReg_struct * pDest=NULL);
        uint32_t readUpTime(uint8_t slaveAdr);
        
        bool readErrorCounter(uint8_t slaveAdr,I2C_slave_errorCounter_struct * pDest,bool clearAfterRead=false);
        bool readISRInfo(uint8_t slaveAdr,I2C_slave_ISRcounter_struct * pDest);
        
        // access stream slaves
        bool stream_getInfo(uint8_t slaveAdr,I2C_slave_stream_info * pDest);
        bool stream_write(uint8_t slaveAdr,uint8_t* pData,uint8_t length);
        int stream_read(uint8_t slaveAdr,uint8_t* pDest,uint8_t length);
        bool stream_flushAllBuffers(uint8_t slaveAdr);

        // access reg slaves with connected register set
        bool write_regSet(uint8_t slaveAdr,void * pReg,uint8_t length,bool CRC=false);
        int  read_regSet(uint8_t slaveAdr,void * pReg,uint8_t length,bool CRC=false);

        // access statistics
        uint32_t getLastDuration()       {return _stats.lastTransactionTime_usec;}
        String dumpInstance();
        String dumpCtrlReg(I2C_slave_CtrlReg_struct * pSrc);
   
    private:
        TwoWire *   _pBus;
        Mutex *     _pMutex;
        bool        _dummyMutex;
        I2C_statistic_struct _stats;

        void _flushLibBuffer();
        uint8_t _readFrame(uint8_t slaveAdr,uint8_t count,uint8_t* pDest,bool sendStop);
        bool _readOneByte(uint8_t slaveAdr,uint8_t* pData,bool sendStop);


};

//        uint8_t _writeData(uint8_t* pData,uint8_t length,uint8_t sendOverhead=0);
//        uint8_t _readData(uint8_t slaveAdr,uint8_t* pData,uint8_t length);


