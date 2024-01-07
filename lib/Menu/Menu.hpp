#pragma once
#include <Arduino.h>
#include <MenuBase.hpp>

extern MenuHandler      menuHandler;

extern MenuHeaderText       menuTestHeader;
extern MenuEntryBoolToggle  menuTestEntryBool1;
extern MenuEntryBool        menuTestEntryBool2;
extern MenuEntryText        menuTestTime;

#define MENU_TEST_COUNT 13
extern MenuEntry *      menuTest[MENU_TEST_COUNT];


