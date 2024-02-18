
I2C Master & Slave-Stream & Slave-Reg 
=====================================

I2C Frame (protocol) short overview
-----------------------------------
| [ID]   | protocol |  description |  [ix] Paramter Master |  [ix] answer slave|
|:------   | :----    | :----        | :---              | :---   | 
| [0x00] | | reserved |  | |
|  | ==general==|  |  | |
| [0x01] |  general | Status / control | [CTRL]  see table | [STATUS] see table|
| [0x02] |  general | read slave up time [ms] | - | [1..4] up time L byte first |
| [0x03] |  general | read slave error counter | - | see general error struct table |
| [0x04] |  general | read slave ISR info | - | see general ISR struct table |
| [0x0F] |  general | read error counter & clear | - | see general error struct table, errors will be cleared on read |
|  | ==stream==|  |  | |
| [0x12] |  stream | write to salve  | [size][0..255] number of bytes and data write to slave | - 
| [0x13] |  stream | read from salve  | [size] number of bytes to read from slave  | [0.255] data from slave  |
| [0x1E] |  stream | info for stream channel | - | see stream info struct table | 
| [0x1F] |  stream | flush buffers    | clear all stream buffer of slave (IN & OUT) | - |
|  | ==register==||||
| [0x22] | register| write data to slave receive register bank            | [start][count][crc][data]      |  |
|||| [start]:  start index of write operation ||
|||| [count]:  number of register (bytes) in this write operation ||
|||| [CRC]: (new) CRC for complete slave receive register bank, if CRC check is enabled, any change (even partial) will only be accepted (taken over by slave) when CRC of complete slave recieve register bank will match to this value ||
|||| [data]: (new) content for register ||
| [0x23] | register| read data from slave transmit register bank           | [start][count]      | [CRC][data] |
|||| [start]:  start index of read operation ||
|||| [count]:  number of requested register (bytes) for this read operation  ||
||||  |[count]: provided data bytes in this read operation |
||||  |[CRC]: current CRC of complete slave transmit register bank (even if read operation was only for a fraction of register bank the CRC for the complete slave transmit register bank is send here ). If CRC check is switched off 0x55 
||||  |[data]: content for register|
will be send|
| [0x2D] | register| calc and transmit current crc values for receive register and transmit register            | [CRC-receive-bank][CRC-transmit-bank]      |  |
| [0x2F] | register| reset receive and transmit register to default values  | |  |
| [0xAA] | | reserved |||
| [0xFF] | | reserved |||



CTRL Byte
-----------
| CTRL / STATUS |  default | description |
| :----    | :---- | :----        | 
| CHANGE / ACCEPTED|0| 0: no update CTRL register (just read STATUS)  |
|     || 1: update CTRL register with new values  |
| STREAM |1| 1: switch stream service on |
|     || 0: switch stream service off |
| REGISTER |1| 1: switch register service on |
|     || 0: switch register service off |
| REG-CRC |0| 1: switch CRC check for register service on |
|     ||   0: switch CRC check for register service off |
| reserved / CRC-ERROR | 0 | reserved |
| LED | 1 | switch LED blinking 1:on / 0:off |
| LOG | 0 | switch LOG prints on / off |
| LOG_BASE | 0 | print objects logs |
| LOG_CTRL | 0 | print base ctrl register |
| LOG_STATUS | 0 | print base status register |
| LOG_ISR |0 | print ISR statistics |
| LOG_ERROR |0 | print error counters |
| LOG_STREAM | 0 | print stream service infos |
| LOG_REG | 0 |print register service infos |
| USER_1  | 0 | free to use in application | 
| USER_2  | 0 | free to use in application | 
| USER_3  | 0 | free to use in application | 
| USER_4  | 0 | free to use in application | 
| USER_5  | 0 | free to use in application | 
| USER_6  | 0 | free to use in application | 
| LOG_printTime| 5000 | time in ms between two LOG prints |




stream info struct
-----------
| byte | name | cleared on Read | description |
|:------   | :----    | :----    | :----        | 
| 0 | in size | N | buffer size in bytes [0..255]|
| 1 | in count | N | bytes available to read in Buffer [0..size]|
| 2 | in free | N | bytes available to write in Buffer [0..size]|
| 3 | in underrun | Y | data polled by application when no bytes where available [0..255] wrap arround|
| 4 | in overrun | Y | bytes received on I2C stream when IN buffer was full [0..255] wrap arround|
| 5 | in max | Y | reserved for max fill level of IN buffer [0..size]  |
| 6 | out size | N | buffer size in bytes [0..255]|
| 7 | out count | N | bytes available to read in Buffer [0..size]|
| 8 | out free | N | bytes available to write in Buffer [0..size]|
| 9 | out underrun | Y | data requested by I2C master when no bytes where available [0..255] wrap arround|
| 10 | out overrun | Y | bytes received from application when OUT buffer was full [0..255] wrap arround|
| 11 | out max | Y | reserved for max fill level of IN buffer [0..size]  |




Sequence chart examples
-----------------------

### 0x01 read slave up time [ms]

this one with some more details .. later only byte transfers (and calls)
:::mermaid
sequenceDiagram
    I2C_Master->>I2C_Slave: [0x01] read up time [us]
    I2C_Master-->>I2C_Master: keep bus active
    I2C_Slave->>I2C_SlaveISR: onReceive()
    I2C_SlaveISR->>I2C_SlaveISR: millis()
    I2C_SlaveISR->>I2C_Slave: TwoWire.write([4 bytes up time])
    I2C_Master-->>I2C_Slave: request 4 Bytes
    I2C_Slave->>I2C_Master: data([4 bytes up time])
    I2C_Master-->>I2C_Master: release bus 
:::


### CMD 0x10: stream OUT available  &  CMD 0x11: stream read data 

:::mermaid
sequenceDiagram
    I2C_Master-->I2C_Slave:  <<< fet available bytes>>>
    I2C_Master->>I2C_Slave: [0x11] get out ready count
    I2C_Slave->>I2C_Slave: _outBuffer.size()
    I2C_Slave->>I2C_Slave: TwoWire.write([1 byte number of available data])
    I2C_Master-->>I2C_Slave: request 1 Bytes
    I2C_Slave->>I2C_Master: [1 byte count] count of available bytes in slave out buffer
    I2C_Master-->>I2C_Master: release bus 
    I2C_Master-->I2C_Slave:  <<< read data>>>
    I2C_Master-->>I2C_Master: master decides to read 8 bytes as example
    I2C_Master->>I2C_Slave: [0x13,0x08] read 8 bytes from out buffer
    I2C_Slave-->>I2C_Slave: pop 8 bytes from out buffer and write to TwoWire
    I2C_Master-->>I2C_Slave: request 8 Bytes
    I2C_Slave->I2C_Master: [8 data bytes]
    I2C_Master-->>I2C_Master: release bus 
:::




* The I2C_slave and I2C Master provides some wrapper class to establish an I2C connection between 
 * two Arduino devices. Where one Arduino system take the master role and the (one or more) other Arduino
 * systems take the slave role
 * 
 * Two "connection modes" are implemented :
 * 1) stream
 * 2) Registerbank
 * 
 * features:
 * +) Thread safe implementation 
 * +) Classes designed in Arduino begin/end  and loop style
 *     no need for user to deal with ISR and/or callbacks
 * +) statistics implemented 
 * +) up to three I2C bus per slave supported at same time 
 *     no limits on master side
 *     mix on of slave / master possible for one Arduino system
 * +) mainly based on TwoWire ==> portable
 * +) 0 Bytes / Transfer rqeuest (in stream mode of no new data from slave)
 * 
 * negatives:
 * -) double (stream) / trible (Register) buffering consums a lot of memory
 * -) not performance optmizied 
 * -) limited to 240 data bytes (buffer and transfer)
 * -) ring buffer size for stream mode must be adjusted in driver header
 * ==> in the applications where I'm short on memory or the focus is on high datarate, I use for the µController spezalised 
 *       drivers (Without two wire lib) and utilze DMA transfers
 * 
 * utilized Arduino LIB's:
 * +) communication based on TwoWire  (portable)
 * +) RingBuf (for stream connection)
 * 
 * 
 * basic architecture:
 * 
 * Stream:
 * =======
 * 
 * your application is dealing with I2C_master / I2C_slaveStream objects only
 * 
 * on master side the complete communication chain is been blocking
 * this means if you call a read or write action, the class method will
 * only return after all data is send / received or an error (timeout) has been occured
 * 
 *  +------------+      +------------+      #################
 *  |            |      |            |      #               #---------SCL----------   
 *  | I2C_master |<====>| TwoWire::  |<====># Arduino HW    #
 *  | ---------- |      | Wire       |      #               #---------SDA----------
 *  |            |      | ----       |      #################
 *  +------------+      +------------+
 * 
 *  so it is an pretty easy straight forward handling
 *  the stream is now designed to simply transfer bytes (unsigned char / uint8_t) to and from the slave
 *   
 *  like above mentioned on master side it is blocking procedure, nevertheless the TowWire Lib
 *  works with a buffer (typically 32 bytes). So the I2C frames are limited to 32 Bytes. But the I2C_master 
 *  object split longer frames to smaller chuncks and send them all without releasing the bus (for possible oher master)
 * 
 * on slave side the data is been double buffered.  


 /**
 *  \class I2C_Slave_8Bit
 *  \ingroup I2C
 *  \brief implements a thread save I2C slave with IN / Write and OUT / Read register ... restricted by wire lib, there can be only one I2C_Slave_XBit per Arduino I2C interface
 *
 *  this class implements the communication part of an I2C slave device 
 *  it provides IN (master write) and OUT (master read) regsiter (with shadowbuffers) as bus<=>application interface   
 *  class takes care if register set is above 32 bytes (buffer size of wire.h) ans split transfers than to multiple frames
 *  
 *  it utilize the arduino Wire lib 
 *  it is written in the Arduino "Style"  with setup and loop function 
 *
 *  1) somewhere you should craete an object of this class:
 *     \snippet  I2C_Slave slave;
 *     \snippet  #define I2C_SLAVE_ADR 0x33
 *
 *   2) configure your slave 
 *      \snippet void setup() {
 *      \snippet    delay(2000);
 *      \snippet    Serial.begin(115200);
 *      \snippet    
 *      \snippet    slave.setup(I2C_SLAVE_ADR,REG_WIDTH_32_BIT,2,2);
 *
 *
 *
 *
 *  the class provides "dirty" functionality, means you can ask if any data or a special register has been modified since th elast loop run
 *  memory consumption 
class I2C_SlavesStream
{
    public:
        I2C_SlavesStream();
        ~I2C_SlavesStream();

        bool setup(TwoWire * pBus,uint8_t adr,uint8_t inRingCount,uint8_t outRingCount);

        // loop functionionality  
        bool bufferInReg();
        void bufferOutReg();
        uint8_t getInRegValue(uint8_t adr);
        void setOutRegValue(uint8_t adr,uint8_t value);
        uint8_t * getInRegField()            {return _pInReg;};
        uint8_t * getOutRegField()           {return _pOutReg;};

        uint32_t getReceivedByteCount()      {return _bytesReceived;};
        uint32_t getSendByteCount()          {return _bytesSend;};

    private:


        uint8_t  _I2Cadr;
        uint32_t _bytesReceived,_bytesSend;
 
        ring<uint8_t> *_pInBuffer;
        ring<uint8_t> *_pOutBuffer;

        volatile uint8_t _ISR_bytesSend,_ISR_bytesReceived;
        volatile uint16_t _ISR_regCounter;  // uint16_t to detect overrun (handled by shadow buffer)

        uint8_t  _busIndex;
        static I2C_SlavesStream * s_pInstance[3];
        static TwoWire *        s_pBus[3];

        void _receiveEvent(int count);
        void _requestEvent();

        void _deleteAllBuffer();

};


 */
