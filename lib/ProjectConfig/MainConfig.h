#pragma once
#include <Arduino.h>

// Project switches
#define WAIT_FOR_TERMINAL   4000    // my VS-Code terminal needs some time after download to open
#define WITH_DSIPLAY_STARTUP_TEST
//#define PRINT_LOOP_STATS    10000


#define CTRL_OBJECT_COUNT   10
class Ctrl;
typedef Ctrl* CtrlPtr;

extern CtrlPtr ctrlObjectList[CTRL_OBJECT_COUNT];
extern String  ctrlNameList[CTRL_OBJECT_COUNT];
