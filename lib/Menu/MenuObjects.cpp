
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


#include <Menu.hpp>


MenuHandler menuHandler;


/////////////////////////////////////////////////////////////////////
/// Test menu

MenuHeaderText menuTestHeader("test menu: (c) MonkeyCodeMen");
MenuEntryBoolToggle menuTestEntryBool1("switch : ");
MenuEntryBool menuTestEntryBool2("==Flag==:",false,false,"TRUE","FALSE","[ "," ]");
MenuEntryText menuTestTime("Time ","000","[ "," sec] ",2,1,TFT_GREEN,TFT_BLACK,0,0);
MenuEntryInt  menuTestInt1("gain (-20,20) : ",1,20,-20,1,true,"{","}");


char entry1[] = "entry 1";
char entry2[] = "entry 2";
char entry3[] = "entry 3";
char entry4[] = "entry 4";

char * pEntries[4] = {entry1,entry2,entry3,entry4};

MenuEntryList menuTestList("select: ",pEntries,4,0,true,false,"[","]",2,2,TFT_BLUE,TFT_BLACK,0,0);
MenuEntryText menuTestLine0("== Line 0 ==","","","",2,1,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine1("== Line 1 ==","","","",2,1,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine2("== Line 2 ==","","","",4,1,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine3("== Line 3 ==","","","",2,1,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine4("== Line 4 ==","","","",2,2,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine5("== Line 5 ==","","","",2,3,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine6("== Line 6 ==","","","",2,4,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine7("== Line 7 ==","","","",2,5,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine8("== Line 8 ==","","","",2,4,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine9("== Line 9 ==","","","",2,3,TFT_WHITE,TFT_BLACK,0,0);

typedef MenuEntry* MenuEntryPtr;

MenuEntryPtr menuTest[MENU_TEST_COUNT] = {  &menuTestEntryBool1,
                            &menuTestEntryBool2,
                            &menuTestTime,
                            &menuTestInt1,
                            &menuTestList,
                            &menuTestLine0,
                            &menuTestLine1,
                            &menuTestLine2,
                            &menuTestLine3,
                            &menuTestLine4,
                            &menuTestLine5,
                            &menuTestLine6,
                            &menuTestLine7,
                            &menuTestLine8,
                            &menuTestLine9
                            };


/////////////////////////////////////////////////////////////////////
/// main menu



MenuHeaderText menuMainHeader("main menu: (c) MonkeyCodeMen",true,true,2,2,TFT_BLUE,TFT_WHITE);
MenuEntryList  menuMainSwitch1("LED 0: ",NULL,0,0,true,false,"[","]",2,2);
MenuEntryList  menuMainSwitch2("LED 1: ",NULL,0,0,true,false,"[","]",2,2);
MenuEntryList  menuMainSwitch3("LED 2: ",NULL,0,0,true,false,"[","]",2,2);
MenuEntryList  menuMainSwitch4("LED 3: ",NULL,0,0,true,false,"[","]",2,2);
MenuEntryList  menuMainRGB1(   "RGB 0: ",NULL,0,0,true,false,"[","]",2,2);
MenuEntryList  menuMainRGB2(   "RGB 1: ",NULL,0,0,true,false,"[","]",2,2);
MenuEntryList  menuMainNeo1(   "NEO 0: ",NULL,0,0,true,false,"[","]",2,2);
MenuEntryList  menuMainNeo2(   "NEO 1: ",NULL,0,0,true,false,"[","]",2,2);
MenuEntryList  menuMainMatrix1("MAT 0: ",NULL,0,0,true,false,"[","]",2,2);
MenuEntryList  menuMainMatrix2("MAT 1: ",NULL,0,0,true,false,"[","]",2,2);

MenuEntryPtr menuMain[MENU_MAIN_COUNT] = { &menuMainSwitch1,&menuMainSwitch2,&menuMainSwitch3,&menuMainSwitch4,
                             &menuMainRGB1,&menuMainRGB2,&menuMainNeo1,&menuMainNeo2,&menuMainMatrix1,&menuMainMatrix2};



