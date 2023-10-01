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

private:
    enum ComState  {WAIT,START_FRAME,MODULE,INDEX,COMMAND,PAR1,PAR2,PAR3,PAR4,LENGTH,DATA,END_FRAME,FRAME_DONE};
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
    void getLength();
    void getData();
    void getEndOfFrame();
    void frameDone();

    bool getByte(u8_t * pBuffer);
    bool getBytes(u32_t count,u8_t * pBuffer);
    bool collectField();
    
    u8_t    _byte;
    String  _field;
    bool    _endFound;

    ComFrame _frame;
    ComDispatch _dispatcher;
};
