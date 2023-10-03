#include "Debug.hpp"

HardwareSerial * Debug::_pSerial = NULL;
Mutex            Debug::_mutex;
Debug                   debug;




Debug::Debug()
{

}

void Debug::start()
{
    _pSerial=&Serial1;
    _pSerial->begin(115200);
}

void Debug::log(char * text){
    String time(millis());

    _mutex.lock();
    out((char*)"LOG(");
    out((char *)time.c_str());
    out((char*)")::");
    out(text);
    outEnd();
    _mutex.unlock();
}


void Debug::log(char * file,int line,char * text){
    String time(millis());

    _mutex.lock();
    out((char*)"LOG(");
    out((char *)time.c_str());
    out((char*)"):");
    out(file);
    out((char*)":");
    String lineStr(line);
    out((char *)lineStr.c_str());
    out((char*)"::");
    out(text);
    outEnd();
    _mutex.unlock();
}

void Debug::assertTrue(bool cond ,char * text){
    if (cond == true){
        return;
    } else {
        String time(millis());

        _mutex.lock();
        out((char*)"ASSERT failed(");
        out((char *)time.c_str());
        out((char*)")::");
        out(text);
        outEnd();
        _mutex.unlock();
    }
}

void Debug::assertTrue(bool cond ,char * file,int line,char * text){
    if (cond == true){
        return;
    } else {
        String time(millis());
        
        _mutex.lock();
        out((char*)"ASSERT failed(");
        out((char *)time.c_str());
        out((char*)"):");
        out(file);
        out((char*)":");
        String lineStr(line);
        out((char *)lineStr.c_str());
        out((char*)"::");
        out(text);
        outEnd();
        _mutex.unlock();
    }
}


void Debug::out(char * text){
    // at the moment only simply dump to terminal
    _pSerial->print(text);
}

void Debug::outEnd(){
    // at the moment only simply dump to terminal
    _pSerial->println("");
    _pSerial->flush();
}
