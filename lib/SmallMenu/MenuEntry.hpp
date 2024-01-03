#pragma once
#include <Arduino.h>
#include <Debug.hpp>
#include <TFT_eSPI.h>

enum MENU_Event_Type {EVENT_NONE,EVENT_INC,EVENT_DEC,EVENT_ENTER};
#define MENU_STD_FONT 2
#define MENU_STD_CURSOR "<*>"
#define MENU_STD_EMPTY_CURSOR "   "

////////////////////////////////////////////////////////
// base classes 
class MenuItem{
    public:
        MenuItem()
            :_dirty(true),_objectHeight(1) 
            {};
        
        ~MenuItem() = default;

        virtual bool    onMenuOpen()            { return false; /*** overwrite this to manipulate/reset object state in very menu open ***/ };
        virtual bool    needsUpdate()           { return _dirty;                                                                            };
        virtual u16_t   getHeight()             { return _objectHeight;                                                                     };
        virtual bool    setup(TFT_eSPI * pTFT)  { return false; /** calc dimension in correlation to display **/                            };

    protected:
        bool    _dirty;
        u16_t   _objectHeight;
};

class MenuHeader:public MenuItem{
    public:
        MenuHeader()
            :MenuItem() 
            {};
        
        ~MenuHeader() = default;

        virtual bool    draw(TFT_eSPI * pTFT,u16_t x,u16_t y,u16_t dx,u16_t dy) { return false; };
};

class MenuEntry:public MenuItem{
    public:
        MenuEntry()
            :MenuItem() 
            {};
        
        ~MenuEntry() = default;

        virtual bool    draw(TFT_eSPI * pTFT,u16_t x,u16_t y,u16_t dx,u16_t dy,bool asActive)   { return false; };
        virtual bool    onEvent(MENU_Event_Type event)                                          { return false; };        
    protected:
};

////////////////////////////////////////////////
// implementation for text lines
class MenuHeaderText: public MenuHeader{
    // base entry class .. could be used as static text too
    public:
        MenuHeaderText(const char * staticText="",bool invers=true, bool underline=true, u8_t font=MENU_STD_FONT,u16_t foregndcol = TFT_WHITE, u16_t backgndCol=TFT_BLACK)
            :MenuHeader(),
            _text(staticText),
            _foregndCol(foregndcol),_backgndCol(backgndCol),_font(font),
            _ox(0),_oy(0),
            _invers(invers),_underline(underline)  
            { };

        ~MenuHeaderText() = default;

        virtual bool setup(TFT_eSPI * pTFT){
          _objectHeight = pTFT->fontHeight(_font);  
          if (_underline == true)   _objectHeight+=2;
          return true;
        };

        virtual String getText(){ 
            return _text;        
        };

        virtual void setNewText(String newValue){
            if (newValue != _text){
                _text=newValue;
                _dirty = true;
            }
        };

        virtual void setNewForegroundColor(u16_t newValue){
            if (newValue != _foregndCol){
                _foregndCol=newValue;
                _dirty = true;
            }
        };

        virtual void setNewBackgroundColor(u16_t newValue){
            if (newValue != _backgndCol){
                _backgndCol=newValue;
                _dirty = true;
            }
        };


        virtual bool draw(TFT_eSPI * pTFT,u16_t x,u16_t y,u16_t dx,u16_t dy){
            if (_invers == true){
                pTFT->fillRect(x,y,dx,dy,_foregndCol);
                pTFT->setTextColor(_backgndCol,_foregndCol);
                pTFT->drawString(getText(),x+_ox,y+_oy,_font);
                if (_underline == true){
                        pTFT->drawFastVLine(x,y+dy,1,_backgndCol);
                }
            } else {
                pTFT->fillRect(x,y,dx,dy,_backgndCol);
                pTFT->setTextColor(_foregndCol,_backgndCol);
                pTFT->drawString(getText(),x+_ox,y+_oy,_font);
                if (_underline == true){
                        pTFT->drawFastVLine(x,y+dy,1,_foregndCol);
                }
            }
            _dirty = false;
            return true;
        };

    protected:
        String  _text;
        u16_t   _foregndCol,_backgndCol;
        u16_t   _ox,_oy;
        u8_t    _font;
        bool    _invers,_underline;

};

class MenuEntryText: public MenuEntry{
    // base entry class .. could be used as static text too
    public:
        MenuEntryText(const char * text="",const char * endText="", u8_t font=MENU_STD_FONT,u16_t foregndcol = TFT_WHITE, u16_t backgndCol=TFT_BLACK)
            :MenuEntry(),
            _cursor(MENU_STD_CURSOR),_emptyCursor(MENU_STD_EMPTY_CURSOR),_baseText(text),_valueText(""),_endText(endText),
            _foregndCol(foregndcol),_backgndCol(backgndCol),_font(font),  
            _ox(0),_oy(0)
            {};
        ~MenuEntryText() = default;

        virtual bool setup(TFT_eSPI * pTFT){
          _objectHeight = pTFT->fontHeight(_font);  
          return true;
        };

        virtual bool showNoCursorOptions(){
            _cursor = "";
            _emptyCursor = "";
            return true;
        }

        virtual bool changeCursorOption(String activeChars,String inactiveChars){
            if ((activeChars.length() != 3)   && (activeChars.length() != 0))   return false;
            if ((inactiveChars.length() != 3) && (inactiveChars.length() != 0)) return false;
            _cursor = activeChars;
            _emptyCursor = inactiveChars;
            return true;
        }

        virtual String getText(bool active){
            String res;
            if (active == true) {
                res = _cursor+_baseText+_valueText+_endText;
            } else {
                res = _emptyCursor+_baseText+_valueText+_endText;
            }
            return res;        
        };

        virtual void setNewText(String newValue){
            if (newValue != _baseText){
                _baseText=newValue;
                _dirty = true;
            }
        };

        virtual void setNewEndText(String newValue){
            if (newValue != _endText){
                _endText = newValue;
                _dirty = true;
            }
        };

        virtual void setNewValueText(String newValue){
            if (newValue != _valueText){
                _valueText = newValue;
                _dirty = true;
            }
        };

        virtual void setNewBackgroundColor(u16_t newValue){
            if (newValue != _backgndCol){
                _backgndCol=newValue;
                _dirty = true;
            }
        };

        virtual bool draw(TFT_eSPI * pTFT,u16_t x,u16_t y,u16_t dx,u16_t dy,bool active){
            if (active == true){
                pTFT->fillRect(x,y,dx,dy,_foregndCol);
                pTFT->setTextColor(_backgndCol,_foregndCol);
                pTFT->drawString(getText(active),x+_ox,y+_oy,_font);
            } else {
                pTFT->fillRect(x,y,dx,dy,_backgndCol);
                pTFT->setTextColor(_foregndCol,_backgndCol);
                pTFT->drawString(getText(active),x+_ox,y+_oy,_font);
            }
            _dirty = false;
            return true;
        };

    protected:
        String _cursor,_emptyCursor;
        String _baseText;
        String _valueText;
        String _endText;
        u16_t  _foregndCol,_backgndCol;
        u8_t   _font;
        u16_t _ox,_oy;
};

////////////////////////////////////////////////
// based on text line items, some value classes
class MenuEntryBool : public MenuEntryText{
    public:
        MenuEntryBool(  const char * text,
                        bool initValue = false,
                        bool wrapAround = true,
                        const char * onText = F_CHAR("on"),
                        const char * offText = F_CHAR("off"),
                        const char * endText = F_CHAR(""))
            :MenuEntryText(text,endText),
            _value(initValue),_resetValue(initValue),_wrapAround(wrapAround),
            _onText(onText),_offText(offText)
            {   // init value text 
                setNewValueText((_value == true) ? _onText : _offText); 
            };
        ~MenuEntryBool() = default;

        virtual bool getValue(){ 
            return _value;
        };

        virtual void setValue(bool newValue){
            if (newValue != _value){
                _value = newValue;
                setNewValueText((_value == true) ? _onText : _offText);
                _dirty = true;
            }            
        };

        virtual bool onEvent(MENU_Event_Type event) {
            // handle Event based on selected logic
            bool handled = true;
            if (_wrapAround == true){
                switch(event){
                    case EVENT_INC:  
                    case EVENT_DEC:
                    case EVENT_ENTER: 
                        setValue(!_value);
                        break;
                    default:
                        /* event not handled by this class but maybe by derived classes*/
                        handled = false;
                }
            } else {
                switch(event){
                    case EVENT_INC:     setValue(true);         break;
                    case EVENT_DEC:     setValue(false);        break;
                    case EVENT_ENTER:   setValue(_resetValue);  break;
                    default:
                        /* event not handled by this class but maybe by derived classes*/
                        handled = false;
                }
            }
            return handled;
        };

    protected:
        bool    _value,_resetValue,_wrapAround;
        String  _onText,_offText,_endText;
};