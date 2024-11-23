
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


#pragma once
#include <Arduino.h>
#include <Debug.hpp>
#include <TFT_eSPI.h>
#include <Events.hpp>


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

        virtual bool    onEvent(EventType event)                          { return false; }

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

