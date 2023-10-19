#include "Debug.hpp"

volatile bool    Debug::_initDone = false;
HardwareSerial * Debug::_pSerial = NULL;
Mutex            Debug::_mutex;
Debug                   debug;


Debug::Debug()
{
}

extern bool setupStarted;
bool Debug::_check(){
    if (_initDone == false){
            _mutex.lock();
            _pSerial=&DEBUG_PORT;      
            _pSerial->begin(115200);
            _initDone = true;
            _mutex.unlock();
    }
    return true;
}

void Debug::log(char * text){
    if (_check() == false) return;
    String time(millis());

    _mutex.lock();
    _out((char*)F("LOG("));
    _out((char *)time.c_str());
    _out((char*)F(")::"));
    _out(text);
    _outEnd();
    _mutex.unlock();
}


void Debug::log(char * file,int line,char * text){
    if (_check() == false) return;
    String time(millis());

    _mutex.lock();
    _out((char*)F("LOG("));
    _out((char *)time.c_str());
    _out((char*)F("):"));
    _out(file);
    _out((char*)F(":"));
    String lineStr(line);
    _out((char *)lineStr.c_str());
    _out((char*)F("::"));
    _out(text);
    _outEnd();
    _mutex.unlock();
}

void Debug::assertTrue(bool cond ,char * text){
    if (_check() == false) return;
    if (cond == true){
        return;
    } else {
        String time(millis());

        _mutex.lock();
        _out((char*)F("ASSERT failed("));
        _out((char *)time.c_str());
        _out((char*)F(")::"));
        _out(text);
        _outEnd();
        _mutex.unlock();
    }
}

void Debug::assertTrue(bool cond ,char * file,int line,char * text){
    if (_check() == false) return;
    if (cond == true){
        return;
    } else {
        String time(millis());
        
        _mutex.lock();
        _out((char*)F("ASSERT failed("));
        _out((char *)time.c_str());
        _out((char*)F("):"));
        _out(file);
        _out((char*)F(":"));
        String lineStr(line);
        _out((char *)lineStr.c_str());
        _out((char*)F("::"));
        _out(text);
        _outEnd();
        _mutex.unlock();
    }
}


void Debug::_out(char * text){
    // at the moment only simply dump to terminal
    _pSerial->print(text);
}

void Debug::_outEnd(){
    // at the moment only simply dump to terminal
    _pSerial->println();
    _pSerial->flush();
}
