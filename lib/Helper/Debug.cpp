#include "Debug.hpp"
#include "helper.hpp"
#include <malloc.h>


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
    _out((char*)F_CONST("LOG("));
    _out((char *)time.c_str());
    _out((char*)F_CONST(")::"));
    _out(text);
    _outEnd();
    _mutex.unlock();
}


void Debug::log(char * file,int line,char * text){
    if (_check() == false) return;
    String time(millis());

    _mutex.lock();
    _out((char*)F_CONST("LOG("));
    _out((char *)time.c_str());
    _out((char*)F_CONST("):"));
    _out(file);
    _out((char*)F_CONST(":"));
    String lineStr(line);
    _out((char *)lineStr.c_str());
    _out((char*)F_CONST("::"));
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
        _out((char*)F_CONST("ASSERT failed("));
        _out((char *)time.c_str());
        _out((char*)F_CONST(")::"));
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
        _out((char*)F_CONST("ASSERT failed("));
        _out((char *)time.c_str());
        _out((char*)F_CONST("):"));
        _out(file);
        _out((char*)F_CONST(":"));
        String lineStr(line);
        _out((char *)lineStr.c_str());
        _out((char*)F_CONST("::"));
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



void Debug::logMem(char * file,int line,char * text){
    if (_check() == false) return;
    int usedHeap = rp2040.getUsedHeap();
    int freeHeap = rp2040.getFreeHeap();
    String temp(millis());

    _mutex.lock();
    
    _out((char*)F_CONST("LOG memory("));
    _out((char *)temp.c_str());
    _out((char*)F_CONST("):"));
    _out(file);
    _out((char*)F_CONST(":"));
    //temp = String(line);
    temp = line;
    _out((char *)temp.c_str());
    _out((char*)F_CONST("::"));
    _out(text);
    temp="-memory status:\n\tused heap: "+String(usedHeap)+"\n\tfree heap: "+String(freeHeap);
    _out((char *)temp.c_str());
    
    _outEnd();
    _mutex.unlock();
}

