#pragma once

#include <Arduino.h>



class Debug
{
public:
	Debug();
	~Debug() = default;
    static void start();

    static void log(char * text);
    static void log(char * file,int line,char * text);

    static void assertTrue(bool cond ,char * text);
    static void assertTrue(bool cond ,char * file,int line,char * text);

private:
    static void out(char * text);
    static void outEnd();
    static void check();

    static HardwareSerial * _pSerial;

};


//extern Debug debug;

#define WITH_DEBUG 1
#ifdef WITH_DEBUG
    #define LOG(text)           Debug::log((char*)__FILE__,__LINE__,(char*)text)
    #define ASSERT(cond,text)   Debug::assertTrue(cond,(char*)__FILE__,__LINE__,(char*)text)
#else
    #define LOG(text)
    #define ASSERT(cond,text)   
#endif

