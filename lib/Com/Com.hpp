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
    enum ComState  {WAIT,START_FRAME,MODULE,INDEX,COMMAND,PAR1,PAR2,PAR3,PAR4,STR_START,STR_DATA,STR_END,LENGTH,DATA,FRAME_DONE};
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
    void getLength();
    void getData();
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