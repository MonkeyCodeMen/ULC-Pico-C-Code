#pragma once
#include <MenuItem.hpp>
////////////////////////////////////////////////
// implementation for text lines

#define MENU_STD_FONT           2
#define MENU_STD_FONT_SIZE      2
#define MENU_STD_CURSOR         "<*>"
#define MENU_STD_EMPTY_CURSOR   "   "

#define WITH_VIEWPORT           1

/*
    a class to process and render simple text menu on small display
    
    a simple text line based menu like this:
        
    ~~~~~~~~~~~~~~~~~~
    headline
    ----------------
        item 1 
    #<>#item#2######
        item 3
    ~~~~~~~~~~~~~~~~~~
*/
class MenuEntryText: public MenuEntry{
    // base entry class .. could be used as static text too
    public:
        MenuEntryText(  const char * text="",
                        const char * endText="", 
                        u8_t font=MENU_STD_FONT, 
                        u8_t fontSize = MENU_STD_FONT_SCALE,
                        u16_t foregndcol = TFT_WHITE, 
                        u16_t backgndCol=TFT_BLACK,
                        u16_t offsetX = 5,
                        u16_t offsetY = 0)
            :MenuEntry(),
            _cursor(MENU_STD_CURSOR),_emptyCursor(MENU_STD_EMPTY_CURSOR),_baseText(text),_valueText(""),_endText(endText),
            _foregndCol(foregndcol),_backgndCol(backgndCol),_font(font),_fontSize(fontSize),
            _ox(offsetX),_oy(offsetY)
            {}
        ~MenuEntryText() = default;

        virtual bool setup(TFT_eSPI * pTFT,u16_t menuHeight,u16_t menuWidth){
            pTFT->setTextSize(_fontSize);
            _objectHeight = pTFT->fontHeight(_font) + (2*_oy); 
            _objectWidth = menuWidth;
            return true;
        }

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

        virtual String getText(){
            String res;
            if (_hasFocus == true) {
                res = _cursor+_baseText+_valueText+_endText;
            } else {
                res = _emptyCursor+_baseText+_valueText+_endText;
            }
            return res;        
        }

        virtual void setNewText(String newValue){
            if (newValue != _baseText){
                _baseText=newValue;
                _dirty = true;
            }
        }

        virtual void setNewEndText(String newValue){
            if (newValue != _endText){
                _endText = newValue;
                _dirty = true;
            }
        }

        virtual void setNewValueText(String newValue){
            if (newValue != _valueText){
                _valueText = newValue;
                _dirty = true;
            }
        }

        virtual void setNewBackgroundColor(u16_t newValue){
            if (newValue != _backgndCol){
                _backgndCol=newValue;
                _dirty = true;
            }
        }

        virtual bool draw(TFT_eSPI * pTFT,u16_t x,u16_t y){
            pTFT->setTextSize(_fontSize);
            if (_hasFocus == true){
                pTFT->fillRect(x,y,_objectWidth,_objectHeight,_foregndCol);
                pTFT->setTextSize(_fontSize);
                pTFT->setTextColor(_backgndCol,_foregndCol);
                pTFT->drawString(getText(),x+_ox,y+_oy,_font);
            } else {
                pTFT->fillRect(x,y,_objectWidth,_objectHeight,_backgndCol);
                pTFT->setTextSize(_fontSize);
                pTFT->setTextColor(_foregndCol,_backgndCol);
                pTFT->drawString(getText(),x+_ox,y+_oy,_font);
            }
            _dirty = false;
            return true;
        }

    protected:
        String _cursor,_emptyCursor;
        String _baseText;
        String _valueText;
        String _endText;
        u16_t  _foregndCol,_backgndCol;
        u8_t   _font,_fontSize;
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
            }
        
        ~MenuEntryBool() = default;

        virtual bool getValue(){ 
            return _value;
        }

        virtual void setValue(bool newValue){
            if (newValue != _value){
                _value = newValue;
                setNewValueText((_value == true) ? _onText : _offText);
                _dirty = true;
            }            
        }

        virtual bool onEvent(MENU_Event_Type event) {
            // handle Event based on selected logic
            if (_wrapAround == true){
                switch(event){
                    case EVENT_RIGHT:  
                    case EVENT_LEFT:
                    case EVENT_ENTER: 
                        setValue(!_value);
                        return true;    break;
                }
            } else {
                switch(event){
                    case EVENT_RIGHT:   setValue(true);         return true;    break;
                    case EVENT_LEFT:    setValue(false);        return true;    break;
                    case EVENT_ENTER:   setValue(_resetValue);  return true;    break;
                }
            }
            return false;
        }

    protected:
        bool    _value,_resetValue,_wrapAround;
        String  _onText,_offText,_endText;
};