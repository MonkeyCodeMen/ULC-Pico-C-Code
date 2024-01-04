#pragma once
#include <Arduino.h>
#include <Debug.hpp>
#include <TFT_eSPI.h>

enum MENU_Event_Type {EVENT_NONE,EVENT_RIGHT,EVENT_LEFT,EVENT_UP,EVENT_DOWN,EVENT_ENTER};


////////////////////////////////////////////////////////
// base classes 
class MenuItem{
    public:
        MenuItem()
            :_dirty(true),_objectHeight(0),_objectWidth(0) 
            {}
        
        ~MenuItem() = default;

        virtual bool    onMenuOpen()                                            { return false;         } // overwrite this to manipulate/reset object state in very menu open 
        virtual bool    needsUpdate()                                           { return _dirty;        }
        virtual u16_t   getHeight()                                             { return _objectHeight; }
        virtual u16_t   getWidth()                                              { return _objectWidth;  }
        virtual bool    setup(TFT_eSPI * pTFT,u16_t menuHeight,u16_t menuWidth) { return false;         } // calc dimension in correlation to display 
        virtual bool    draw(TFT_eSPI * pTFT,u16_t x,u16_t y)                   { return false;         }

    protected:
        bool    _dirty;
        u16_t   _objectHeight,_objectWidth;
};

class MenuHeader:public MenuItem{
    public:
        MenuHeader()
            :MenuItem() 
            {}
        
        ~MenuHeader() = default;
};

class MenuEntry:public MenuItem{
    public:
        MenuEntry()
            :MenuItem(),
            _hasFocus(false)
            {}
        
        ~MenuEntry() = default;

        virtual bool    onEvent(MENU_Event_Type event)                          { return false; }

        virtual void    setFocus(){
            if (_hasFocus == true)  return;
            _hasFocus = true;
            _dirty = true;
        }

        virtual void    clearFocus(){
            if (_hasFocus == false)  return;
            _hasFocus = false;
            _dirty = true;
        }

    protected:
        bool _hasFocus;
};

