#include <Menu.hpp>

MenuHandler menuHandler;

MenuHeaderText menuTestHeader(F_CHAR("test menu: (c) MonkeyCodeMen"));

MenuEntryBool menuTestEntryBool1(F_CHAR("switch : "));
MenuEntryBool menuTestEntryBool2(F_CHAR("==Flag==:["),false,false,F_CHAR("TRUE"),F_CHAR("FALSE"),F_CHAR("]"));
MenuEntryText menuTestLine0(F_CHAR("== Line 0 =="),"",1,1,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine1(F_CHAR("== Line 1 =="),"",2,1,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine2(F_CHAR("== Line 2 =="),"",3,1,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine3(F_CHAR("== Line 3 =="),"",4,1,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine4(F_CHAR("== Line 4 =="),"",2,1,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine5(F_CHAR("== Line 5 =="),"",2,2,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine6(F_CHAR("== Line 6 =="),"",2,3,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine7(F_CHAR("== Line 7 =="),"",2,4,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine8(F_CHAR("== Line 8 =="),"",2,3,TFT_WHITE,TFT_BLACK,0,0);
MenuEntryText menuTestLine9(F_CHAR("== Line 9 =="),"",2,2,TFT_WHITE,TFT_BLACK,0,0);


MenuEntry * menuTest[] = {  &menuTestEntryBool1,
                            &menuTestEntryBool2,
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
