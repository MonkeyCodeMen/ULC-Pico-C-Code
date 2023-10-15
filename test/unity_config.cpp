#include <Arduino.h>
#include "unity_config.h"
void myUnityOutputStart(unsigned long baudrate)   { Serial1.begin(baudrate); }
void myUnityOutputChar(unsigned int c)            { Serial1.write(c); }
void myUnityOutputFlush(void)                     { Serial1.flush(); }
void myUnityOutputComplete(void)                  { Serial1.end(); }

