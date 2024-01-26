#pragma once
#include <Arduino.h>
#include <Debug.hpp>
#include <TFT_eSPI.h>
#include <Events.h>


////////////////////////////////////////////////////////
// base classes 
class MenuItem{
    public:
        MenuItem()
            :_dirty(true),_objectHeight(0),_objectWidth(0) 
            {}
        
        ~MenuItem() = default;

        virtual bool    onMenuOpen()                                                    { return false;         } // overwrite this to manipulate/reset object state in very menu open 
        virtual bool    needsUpdate()                                                   { return _dirty;        }
        virtual uint16_t    getHeight()                                                 { return _objectHeight; }
        virtual uint16_t    getWidth()                                                  { return _objectWidth;  }
        virtual bool    setup(TFT_eSPI * pTFT,uint16_t  menuHeight,uint16_t  menuWidth) { return false;         } // calc dimension in correlation to display 
        virtual bool    draw(TFT_eSPI * pTFT,uint16_t  x,uint16_t  y)                   { return false;         }

    protected:
        bool    _dirty;
        uint16_t    _objectHeight,_objectWidth;
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

        virtual bool    onEvent(Event_Type event)                          { return false; }

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

