#pragma once
#include <Arduino.h>
#include <MenuBase.hpp>

extern MenuHandler          menuHandler;


//////////////////////////////////////////////////////
// Test Menu
#define MENU_TEST_COUNT 14
extern MenuEntry *          menuTest[MENU_TEST_COUNT];
extern MenuHeaderText       menuTestHeader;
extern MenuEntryBoolToggle  menuTestEntryBool1;
extern MenuEntryBool        menuTestEntryBool2;
extern MenuEntryText        menuTestTime;


