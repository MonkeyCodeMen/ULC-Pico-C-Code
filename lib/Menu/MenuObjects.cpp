#include <Menu.hpp>

MenuHandler menuHandler;

MenuHeaderText menuTestHeader("test menu: (c) MonkeyCodeMen");

MenuEntryBoolToggle menuTestEntryBool1("switch : ");
MenuEntryBool menuTestEntryBool2("==Flag==:",false,false,"TRUE","FALSE","[ "," ]");
MenuEntryText menuTestTime("Time ","000","[ "," sec] ",2,1,TFT_GREEN,TFT_BLACK,0,0);
MenuEntryInt  menuTestInt1("gain (-20,20) : ",1,20,-20,1,true,"{","}");
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


MenuEntry * menuTest[MENU_TEST_COUNT] = {  &menuTestEntryBool1,
                            &menuTestEntryBool2,
                            &menuTestTime,
                            &menuTestInt1,
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
