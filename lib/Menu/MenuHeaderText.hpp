#pragma once
#include <MenuItem.hpp>
////////////////////////////////////////////////
// implementation for text lines

#define MENU_STD_FONT           2
#define MENU_STD_FONT_SCALE     1
#define WITH_VIEWPORT           1

class MenuHeaderText: public MenuHeader{
    // base entry class .. could be used as static text too
    public:
        MenuHeaderText( const char * staticText="",
                        bool invers=true, 
                        bool underline=true, 
                        uint8_t font=MENU_STD_FONT, 
                        uint8_t fontSize = MENU_STD_FONT_SCALE,
                        uint16_t  foregndcol = TFT_RED, 
                        uint16_t  backgndCol=TFT_BLUE,
                        uint16_t  offsetX = 5,
                        uint16_t  offsetY = 0,
                        uint8_t  lineWidth = MENU_STD_FONT_SCALE)
            :MenuHeader(),
            _text(staticText),
            _foregndCol(foregndcol),_backgndCol(backgndCol),_font(font),_fontSize(fontSize),
            _ox(offsetX),_oy(offsetY),
            _invers(invers),_underline(underline),
            _lineWidth(lineWidth)
            { }

        ~MenuHeaderText() = default;

        virtual bool setup(TFT_eSPI * pTFT,uint16_t  menuHeight,uint16_t  menuWidth){
            pTFT->setTextSize(_fontSize);
            _objectHeight = pTFT->fontHeight(_font) + (2 * _oy);  
            if (_underline == true){
                if (_invers == true){
                    _objectHeight+=2*_lineWidth;
                } else {
                    _objectHeight+=_lineWidth;
                }
            }   
            _objectWidth = menuWidth;
            return true;
        }

        virtual String getText(){ 
            return _text;        
        }

        virtual void setNewText(String newValue){
            if (newValue != _text){
                _text=newValue;
                _dirty = true;
            }
        }

        virtual void setNewForegroundColor(uint16_t  newValue){
            if (newValue != _foregndCol){
                _foregndCol=newValue;
                _dirty = true;
            }
        }

        virtual void setNewBackgroundColor(uint16_t  newValue){
            if (newValue != _backgndCol){
                _backgndCol=newValue;
                _dirty = true;
            }
        }


        virtual bool draw(TFT_eSPI * pTFT,uint16_t  x,uint16_t  y){
            if (_invers == true){
                pTFT->fillRect(x,y,_objectWidth,_objectHeight,_foregndCol);
                pTFT->setTextSize(_fontSize);
                pTFT->setTextColor(_backgndCol,_foregndCol);
                pTFT->drawString(getText(),x+_ox,y+_oy,_font);
                if (_underline == true){
                        pTFT->fillRect(x,y+_objectHeight-(2*_lineWidth),_objectWidth,_lineWidth,_backgndCol);
                }
            } else {
                pTFT->fillRect(x,y,_objectWidth,_objectHeight,_backgndCol);
                pTFT->setTextSize(_fontSize);
                pTFT->setTextColor(_foregndCol,_backgndCol);
                pTFT->drawString(getText(),x+_ox,y+_oy,_font);
                if (_underline == true){
                        pTFT->fillRect(x,y+_objectHeight-_lineWidth,_objectWidth,_lineWidth,_foregndCol);
                }
            }
            _dirty = false;
            return true;
        }

    protected:
        String  _text;
        uint16_t    _foregndCol,_backgndCol;
        uint16_t    _ox,_oy;
        uint8_t    _font,_fontSize;
        bool    _invers,_underline;
        uint8_t    _lineWidth;

};

