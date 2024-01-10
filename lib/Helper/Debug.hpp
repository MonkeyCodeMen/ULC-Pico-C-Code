#pragma once

#include <Arduino.h>
#include <Mutex.hpp>

#ifndef F_CHAR
    #define F_CHAR (const char *) F
#endif


class Debug
{
public:
	Debug();
	~Debug() = default;

    void begin(Stream * pOut);

    static void log(char * text);
    static void log(char * file,int line,char * text);
    static void log(char * file,int line,char * text,int value);

    static void logMem(char * file,int line,char * text);
    
    static void assertTrue(bool cond ,char * text);
    static void assertTrue(bool cond ,char * file,int line,char * text);

    static void stop(const char * file,int line,char * message);

    static void dump(const char * pName,void *p, uint8_t length);
    static void dump(const char * pName,uint32_t value);
    static void dump(const char * pName,uint32_t value, int base);
    static void dump(const char * pName,String value);


    String hexDump(uint8_t  * p,uint8_t length,const char * sep=" ", const char * prefix="");


private:
    static void _out(char * text);
    static void _outEnd();
    static bool _check();

    volatile static bool    _initDone;
    static Stream *         _pOut;
    static Mutex            _mutex;

};


extern Debug debug;

#define WITH_DEBUG 1
#ifdef WITH_DEBUG
    #define LOG(text)           Debug::log((char*)__FILE__,__LINE__,(char*)text)
    #define LOG_INT(text,value) Debug::log((char*)__FILE__,__LINE__,(char*)text,(int)value)
    #define LOG_MEM(text)       Debug::logMem((char*)__FILE__,__LINE__,(char*)text)
    #define ASSERT(cond,text)   Debug::assertTrue(cond,(char*)__FILE__,__LINE__,(char*)text)
    #define STOP(text)          Debug::stop((char*)__FILE__,__LINE__,(char*)text)
    #define DUMP(...)           Debug::dump( __VA_ARGS__)
#else
    #define LOG(text)
    #define LOG_MEM(text)
    #define ASSERT(cond,text)   
    #define STOP()              Debug::stop((char*)__FILE__,__LINE__,(char*)text)
    #define DUMP(...)
#endif

