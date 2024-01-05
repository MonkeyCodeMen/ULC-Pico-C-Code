#include <Menu.hpp>

MenuHandler menuHandler;

MenuHeaderText menuTestHeader(F_CHAR("test menu: (c) MonkeyCodeMen"));

MenuEntryBool menuTestEntryBool1("switch : ");
MenuEntryBool menuTestEntryBool2("==Flag==:[",false,false,F_CHAR("TRUE"),F_CHAR("FALSE"),F_CHAR("]"));

MenuEntry * menuTest[] = {&menuTestEntryBool1,&menuTestEntryBool2};
