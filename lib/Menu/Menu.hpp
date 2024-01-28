#pragma once
#include <Arduino.h>
#include <MenuBase.hpp>

extern MenuHandler          menuHandler;


//////////////////////////////////////////////////////
// Test Menu
#define MENU_TEST_COUNT 15
extern MenuEntry *          menuTest[MENU_TEST_COUNT];
extern MenuHeaderText       menuTestHeader;
extern MenuEntryBoolToggle  menuTestEntryBool1;
extern MenuEntryBool        menuTestEntryBool2;
extern MenuEntryText        menuTestTime;


/////////////////////////////////////////////////////////////////////
/// main menu
#define MENU_MAIN_COUNT 10
extern MenuEntry * menuMain[MENU_MAIN_COUNT];
extern MenuHeaderText menuMainHeader;
extern MenuEntryList  menuMainSwitch1;
extern MenuEntryList  menuMainSwitch2;
extern MenuEntryList  menuMainSwitch3;
extern MenuEntryList  menuMainSwitch4;
extern MenuEntryList  menuMainRGB1;
extern MenuEntryList  menuMainRGB2;
extern MenuEntryList  menuMainNeo1;
extern MenuEntryList  menuMainNeo2;
extern MenuEntryList  menuMainMatrix1;
extern MenuEntryList  menuMainMatrix2;





