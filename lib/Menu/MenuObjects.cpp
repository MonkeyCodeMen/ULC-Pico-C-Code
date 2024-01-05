#include <Menu.hpp>

MenuHandler menuHandler;

MenuHeaderText menuTestHeader(F_CHAR("test menu: (c) MonkeyCodeMen"));

MenuEntryBool menuTestEntryBool1(F_CHAR("switch : "));
MenuEntryBool menuTestEntryBool2(F_CHAR("==Flag==:["),false,false,F_CHAR("TRUE"),F_CHAR("FALSE"),F_CHAR("]"));
MenuEntryText menuTestLine0(F_CHAR("== Line 0 =="));
MenuEntryText menuTestLine1(F_CHAR("== Line 1 =="));
MenuEntryText menuTestLine2(F_CHAR("== Line 2 =="));
MenuEntryText menuTestLine3(F_CHAR("== Line 3 =="));
MenuEntryText menuTestLine4(F_CHAR("== Line 4 =="));
MenuEntryText menuTestLine5(F_CHAR("== Line 5 =="));
MenuEntryText menuTestLine6(F_CHAR("== Line 6 =="));
MenuEntryText menuTestLine7(F_CHAR("== Line 7 =="));
MenuEntryText menuTestLine8(F_CHAR("== Line 8 =="));
MenuEntryText menuTestLine9(F_CHAR("== Line 9 =="));


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
