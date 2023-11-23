#pragma once

#include <Arduino.h>
#include "ComFrame.hpp"
#include "ComDispatch.hpp"

class Com
{
public:
	Com();
	~Com() = default;

    void setup();
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

    bool getByte(u8_t * pBuffer);
    bool getBytes(u32_t count,u8_t * pBuffer);
    bool collectField();
    
    u8_t    _byte;
    String  _field;
    u32_t   _maxFieldLength;
    u32_t   _dataReceived;
    bool    _endFound;

    ComFrame _frame;
    ComDispatch _dispatcher;
};
