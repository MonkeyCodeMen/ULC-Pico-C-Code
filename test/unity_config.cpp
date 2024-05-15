#include <Arduino.h>
#include "unity_config.h"
#define UNITY_PORT 1

#if UNITY_PORT == 0
    void myUnityOutputWait(void)                      { while(!Serial){} }
    void myUnityOutputStart(unsigned long baudrate)   { Serial.begin(baudrate); }
    void myUnityOutputChar(unsigned int c)            { Serial.write(c); }
    void myUnityOutputFlush(void)                     { Serial.flush(); }
    void myUnityOutputComplete(void)                  { Serial.end(); }

#elif UNITY_PORT == 1
    void myUnityOutputWait(void)                      { while(!Serial1){} }
    void myUnityOutputStart(unsigned long baudrate)   { Serial1.begin(baudrate); }
    void myUnityOutputChar(unsigned int c)            { Serial1.write(c); }
    void myUnityOutputFlush(void)                     { Serial1.flush(); }
    void myUnityOutputComplete(void)                  { Serial1.end(); }

#endif



