#pragma once
#include <Arduino.h>
#include <Debug.hpp>
#include <TFT_eSPI.h>

enum MENU_Event_Type {EVENT_NONE,EVENT_INC,EVENT_DEC,EVENT_ENTER};

class MenuHeader{
    // base entry class .. could be used as static text too
    public:
        MenuHeader():_text(""),_dirty(false),_foregndCol(TFT_WHITE),_backgndCol(TFT_BLACK)                            {};
        MenuHeader(String staticText):_text(staticText),_dirty(false),_foregndCol(TFT_WHITE),_backgndCol(TFT_BLACK)   {};
        ~MenuHeader() = default;

        virtual void    onMenuOpen()                    {/*** keep state on menu change / reopen ***/};
        virtual String  getText()                       { return _text;         };
        virtual u32_t   getForegndCol()                 { return _foregndCol;   };
        virtual u32_t   getBackgndCol()                 { return _backgndCol;   };
        
        virtual void    setNewText(String newValue){
            if (newValue != _text){
                _text=newValue;
                _dirty = true;
            }
        };
        virtual bool    hasChanged(){
            bool temp = _dirty;
            _dirty = false;
            return temp;
        };
    protected:
        String _text;
        bool   _dirty;
        u16_t  _foregndCol,_backgndCol;

}

class MenuEntry{
    // base entry class .. could be used as static text too
    public:
        MenuEntry():_text(""),_dirty(false),_foregndCol(TFT_WHITE),_backgndCol(TFT_BLACK)                            {};
        MenuEntry(String staticText):_text(staticText),_dirty(false),_foregndCol(TFT_WHITE),_backgndCol(TFT_BLACK)   {};
        ~MenuEntry() = default;

        virtual bool    onEvent(MENU_Event_Type event)  {return false;};
        virtual void    onMenuOpen()                    {/*** keep state on menu change / reopen ***/};
        virtual String  getText()                       {return _text;};
        virtual u32_t   getForegndCol()                 { return _foregndCol;};
        virtual u32_t   getBackgndCol()                 { return _backgndCol;};
        
        virtual void    setNewText(String newValue){
            if (newValue != _text){
                _text=newValue;
                _dirty = true;
            }
        };
        virtual bool    hasChanged(){
            bool temp = _dirty;
            _dirty = false;
            return temp;
        };
    protected:
        String _text;
        bool   _dirty;
        u16_t  _foregndCol,_backgndCol;
};

class MenuHeader : public MenuEntry{

}

class MenuBoolEntry : public MenuEntry{
    public:
        MenuBoolEntry(  const char * text,
                        bool wrapAround = true,
                        const char * onText = F_CHAR("on"),
                        const char * offText = F_CHAR("off"),
                        const char * endText = F_CHAR("")):
        MenuEntry(text),_value(false),_wrapAround(wrapAround),_onText(onText),_offText(offText),_endText(endText) { };
        ~MenuBoolEntry() = default;


        virtual void    onMenuOpen()                    {/* keep last state */};
        virtual bool    getValue()                      { return _value;};
        virtual String  getText()                       { 
            String res = _text;
            if (_value == true){
                res += _onText;
            } else {
                res += _offText;
            }
            res += _endText;
            return res;   
        };

        virtual void    setNewEndText(String newValue){
            if (newValue != _endText){
                _endText=newValue;
                _dirty = true;
            }
        };

        virtual bool    onEvent(MENU_Event_Type event) {
            // handle Event based on selected logic
            bool handled = true;
            bool before = _value;
            if (_wrapAround == true){
                switch(event){
                    case EVENT_INC:  
                    case EVENT_DEC:
                    case EVENT_ENTER:
                        _value = !_value;
                        break;
                    default:
                        /* event not handled by this class but maybe by derived classes*/
                        handled = false;
                }
            } else {
                switch(event){
                    case EVENT_INC:  
                        _value = true;
                        break;
                    case EVENT_DEC:  
                        _value = false;
                        break;
                    default:
                        /* event not handled by this class but maybe by derived classes*/
                        handled = false;
                }
            }
            if (before != _value){
                _dirty = true;
            }
            return handled;
        };

    protected:
        bool    _value,_wrapAround;
        String  _onText,_offText,_endText;
};