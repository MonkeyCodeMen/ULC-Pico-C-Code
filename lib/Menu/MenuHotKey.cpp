#include "MenuHotKey.hpp"


MenuHotkey::MenuHotkey()
{
}

int MenuHotkey::eventNameToNr(String name){
    name.toUpperCase();
    if      (name == "NONE")    return EVENT_NONE;
    else if (name == "ENTRE")   return EVENT_ENTER;
    else if (name == "RIGHT")   return EVENT_RIGHT;
    else if (name == "LEFT")    return EVENT_LEFT;
    else if (name == "UP")      return EVENT_UP;
    else if (name == "DOWN")    return EVENT_DOWN;

    else if (name == "A1")      return EVENT_A1;
    else if (name == "A2")      return EVENT_A2;
    else if (name == "A3")      return EVENT_A3;
    else if (name == "A4")      return EVENT_A4;
    else if (name == "A5")      return EVENT_A5;
    else if (name == "B1")      return EVENT_B1;
    else if (name == "B2")      return EVENT_B2;
    else if (name == "B3")      return EVENT_B3;
    else if (name == "B4")      return EVENT_B4;
    else if (name == "B5")      return EVENT_B5;
    else if (name == "B6")      return EVENT_B6;

    else if (name == "A1_DOUBLE")      return EVENT_A1_DOUBLE;
    else if (name == "A2_DOUBLE")      return EVENT_A2_DOUBLE;
    else if (name == "A3_DOUBLE")      return EVENT_A3_DOUBLE;
    else if (name == "A4_DOUBLE")      return EVENT_A4_DOUBLE;
    else if (name == "A5_DOUBLE")      return EVENT_A5_DOUBLE;
    else if (name == "B1_DOUBLE")      return EVENT_B1_DOUBLE;
    else if (name == "B2_DOUBLE")      return EVENT_B2_DOUBLE;
    else if (name == "B3_DOUBLE")      return EVENT_B3_DOUBLE;
    else if (name == "B4_DOUBLE")      return EVENT_B4_DOUBLE;
    else if (name == "B5_DOUBLE")      return EVENT_B5_DOUBLE;
    else if (name == "B6_DOUBLE")      return EVENT_B6_DOUBLE;

    else if (name == "A1_LONG")      return EVENT_A1_LONG;
    else if (name == "A2_LONG")      return EVENT_A2_LONG;
    else if (name == "A3_LONG")      return EVENT_A3_LONG;
    else if (name == "A4_LONG")      return EVENT_A4_LONG;
    else if (name == "A5_LONG")      return EVENT_A5_LONG;
    else if (name == "B1_LONG")      return EVENT_B1_LONG;
    else if (name == "B2_LONG")      return EVENT_B2_LONG;
    else if (name == "B3_LONG")      return EVENT_B3_LONG;
    else if (name == "B4_LONG")      return EVENT_B4_LONG;
    else if (name == "B5_LONG")      return EVENT_B5_LONG;
    else if (name == "B6_LONG")      return EVENT_B6_LONG;


    return -1;
}