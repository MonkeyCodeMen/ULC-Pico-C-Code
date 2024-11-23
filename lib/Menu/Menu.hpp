
/*
 * MIT License
 * 
 * Copyright (c) 2024 MonkeyCodeMen@GitHub
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * provided to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


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





