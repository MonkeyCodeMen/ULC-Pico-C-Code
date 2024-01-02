#pragma once
#include <Arduino.h>
#include <Debug.hpp>

enum MENU_Event_Type {EVENT_NONE,EVENT_INC,EVENT_DEC,EVENT_ENTER};

class MenuEntry{
    // base entry class .. could be used as static text too
    public:
        MenuEntry():_text("")                           {};
        MenuEntry(String staticText):_text(staticText)  {};
        ~MenuEntry() = default;

        virtual bool    onEvent(MENU_Event_Type event) {return false;};
        virtual void    onMenuOpen()                   {};
        virtual String  getText()                      {return _text;};
    protected:
        String _text;
};

class MenuBoolEntry : public MenuEntry{
    public:
        MenuBoolEntry(  const char * text,
                        bool wrapAround = true,
                        const char * onText = F_CHAR("on"),
                        const char * offText = F_CHAR("off")):
        MenuEntry(text),_value(false),_wrapAround(wrapAround),_onText(onText),_offText(offText) { };
        ~MenuBoolEntry() = default;

        virtual bool    onEvent(MENU_Event_Type event) {
            // handle Event based on selected logic
            if (_wrapAround == true){
                switch(event){
                    case EVENT_INC:  
                    case EVENT_DEC:
                    case EVENT_ENTER:
                        _value = !_value;
                        return true;
                        break;
                    default:
                        /* event not handled by this class but maybe by derived classes*/
                        return false;
                }
            } else {
                switch(event){
                    case EVENT_INC:  
                        _value = true;
                        return true;
                        break;
                    case EVENT_DEC:  
                        _value = false;
                        return true;
                        break;
                    default:
                        /* event not handled by this class but maybe by derived classes*/
                        return false;
                }
            }
            return false;
        };
        virtual void    onMenuOpen()                   {/* keep last state */};
        virtual String  getText() {
            return _text+(_value == true) ? _onText : _offText;
        };
    protected:
        bool    _value,_wrapAround;
        String  _onText,_offText;
};