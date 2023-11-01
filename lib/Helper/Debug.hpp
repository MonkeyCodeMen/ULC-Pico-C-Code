#pragma once

#include <Arduino.h>
#include <Mutex.hpp>

// USB serial port
//#define DEBUG_PORT  Serial  
// same like debugger
#define DEBUG_PORT  Serial1

class Debug
{
public:
	Debug();
	~Debug() = default;

    static void log(char * text);
    static void log(char * file,int line,char * text);

    static void logMem(char * file,int line,char * text);
    
    static void assertTrue(bool cond ,char * text);
    static void assertTrue(bool cond ,char * file,int line,char * text);

    static inline void stop() {while(1){delay(1);}};

private:
    static void _out(char * text);
    static void _outEnd();
    static bool _check();

    volatile static bool    _initDone;
    static HardwareSerial * _pSerial;
    static Mutex            _mutex;

};


extern Debug debug;

#define WITH_DEBUG 1
#ifdef WITH_DEBUG
    #define LOG(text)           Debug::log((char*)__FILE__,__LINE__,(char*)text)
    #define LOG_MEM(text)       Debug::logMem((char*)__FILE__,__LINE__,(char*)text)
    #define ASSERT(cond,text)   Debug::assertTrue(cond,(char*)__FILE__,__LINE__,(char*)text)
    #define STOP()              Debug::stop()
#else
    #define LOG(text)
    #define LOG_MEM(text)
    #define ASSERT(cond,text)   
    #define STOP()              Debug::stop()
#endif

