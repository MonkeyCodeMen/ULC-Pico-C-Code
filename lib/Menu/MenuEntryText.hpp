#pragma once
#include <MenuItem.hpp>
#include <StringList.hpp>
////////////////////////////////////////////////
// implementation for text lines

#define MENU_STD_FONT           2
#define MENU_STD_FONT_SIZE      2
#define MENU_STD_CURSOR         "<*>"
#define MENU_STD_EMPTY_CURSOR   "   "


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
                        const char * value="",
                        const char * valuePre="",
                        const char * valuePost="", 
                        uint8_t font=MENU_STD_FONT, 
                        uint8_t fontSize = MENU_STD_FONT_SCALE,
                        uint16_t  foregndcol = TFT_WHITE, 
                        uint16_t  backgndCol=TFT_BLACK,
                        uint16_t  offsetX = 5,
                        uint16_t  offsetY = 1)
            :MenuEntry(),
            _cursor(MENU_STD_CURSOR),_emptyCursor(MENU_STD_EMPTY_CURSOR),
            _baseText(text),_valuePre(valuePre),_valueText(value),_valuePost(valuePost),
            _foregndCol(foregndcol),_backgndCol(backgndCol),_font(font),_fontSize(fontSize),
            _ox(offsetX),_oy(offsetY)
            {}
        ~MenuEntryText() = default;

        virtual bool setup(TFT_eSPI * pTFT,uint16_t  menuHeight,uint16_t  menuWidth){
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
                res = _cursor+_baseText+_valuePre+_valueText+_valuePost;
            } else {
                res = _emptyCursor+_baseText+_valuePre+_valueText+_valuePost;
            }
            return res;        
        }

        virtual void setNewText(String newValue){
            if (newValue != _baseText){
                _baseText=newValue;
                _dirty = true;
            }
        }

        virtual void setNewPost(String newValue){
            if (newValue != _valuePost){
                _valuePost = newValue;
                _dirty = true;
            }
        }

        virtual void setNewPre(String newValue){
            if (newValue != _valuePre){
                _valuePre = newValue;
                _dirty = true;
            }
        }

        virtual void setNewValueText(String newValue){
            if (newValue != _valueText){
                _valueText = newValue;
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
        String _valuePre;
        String _valueText;
        String _valuePost;
        uint16_t   _foregndCol,_backgndCol;
        uint8_t   _font,_fontSize;
        uint16_t  _ox,_oy;
};

////////////////////////////////////////////////
// based on text line items, some value classes
class MenuEntryBool : public MenuEntryText{
    public:
        MenuEntryBool(  const char * text,
                        bool initValue = false,
                        bool withReset = true,
                        const char * onText = "on",
                        const char * offText = "off",
                        const char * valuePre = "[",
                        const char * valuePost = "]",
                        uint8_t font=MENU_STD_FONT, 
                        uint8_t fontSize = MENU_STD_FONT_SCALE,
                        uint16_t  foregndcol = TFT_WHITE, 
                        uint16_t  backgndCol=TFT_BLACK,
                        uint16_t  offsetX = 5,
                        uint16_t  offsetY = 0)
            :MenuEntryText(text,onText,valuePre,valuePost,font,fontSize,foregndcol,backgndCol,offsetX,offsetY),
            _value(initValue),_resetValue(initValue),_withReset(withReset),
            _onText(onText),_offText(offText)
            {   // init value text 
                setNewValueText((_value == true) ? _onText : _offText); 
                if (_withReset == false){
                    changeCursorOption("< >","   ");
                }
            }
        
        ~MenuEntryBool() = default;

        virtual bool getValue(){ 
            return _value;
        }

        virtual void setValue(bool newValue){
            if (newValue != _value){
                _value = newValue;
                setNewValueText((_value == true) ? _onText : _offText);
            }            
        }

        virtual bool onEvent(Event_Type event) {
            switch(event){
                case EVENT_RIGHT:   setValue(true);         return true;    break;
                case EVENT_LEFT:    setValue(false);        return true;    break;
                case EVENT_ENTER:   
                    if (_withReset == true){
                        setValue(_resetValue);  
                        return true;    
                    } 
                    break;
            }
            return false;
        }

    protected:
        bool    _value,_resetValue,_withReset;
        String  _onText,_offText;
};

class MenuEntryBoolToggle : public MenuEntryText{
    public:
        MenuEntryBoolToggle(  const char * text,
                        bool initValue = false,
                        const char * onText = "on",
                        const char * offText = "off",
                        const char * valuePre = "[",
                        const char * valuePost = "]",
                        uint8_t font=MENU_STD_FONT, 
                        uint8_t fontSize = MENU_STD_FONT_SCALE,
                        uint16_t  foregndcol = TFT_WHITE, 
                        uint16_t  backgndCol=TFT_BLACK,
                        uint16_t  offsetX = 5,
                        uint16_t  offsetY = 0)
            :MenuEntryText(text,onText,valuePre,valuePost,font,fontSize,foregndcol,backgndCol,offsetX,offsetY),
            _value(initValue),
            _onText(onText),_offText(offText)
            {   // init value text 
                setNewValueText((_value == true) ? _onText : _offText); 
            }
        
        ~MenuEntryBoolToggle() = default;

        virtual bool getValue(){ 
            return _value;
        }

        virtual void setValue(bool newValue){
            if (newValue != _value){
                _value = newValue;
                setNewValueText((_value == true) ? _onText : _offText);
            }            
        }

        virtual bool onEvent(Event_Type event) {
            // handle Event based on selected logic
            switch(event){
                case EVENT_RIGHT:  
                case EVENT_LEFT:
                case EVENT_ENTER: 
                    setValue(!_value);
                    return true;    
                    break;
            }
            return false;
        }

    protected:
        bool    _value;
        String  _onText,_offText;
};

class MenuEntryInt : public MenuEntryText{
    public:
        MenuEntryInt(  const char * text,
                        int initValue = 0,
                        int upperLimit = 10,
                        int lowerLimit = -10,
                        int step = 1,
                        bool withReset = true,
                        const char * valuePre = "[",
                        const char * valuePost = "]",
                        uint8_t font=MENU_STD_FONT, 
                        uint8_t fontSize = MENU_STD_FONT_SCALE,
                        uint16_t  foregndcol = TFT_WHITE, 
                        uint16_t  backgndCol=TFT_BLACK,
                        uint16_t  offsetX = 5,
                        uint16_t  offsetY = 0)
            :MenuEntryText(text,String(initValue).c_str(),valuePre,valuePost,font,fontSize,foregndcol,backgndCol,offsetX,offsetY),
            _value(initValue),_resetValue(initValue),
            _upperLimit(upperLimit),_lowerLimit(lowerLimit),_step(step),
            _withReset(withReset)
            {   }
        
        ~MenuEntryInt() = default;

        virtual int getValue(){ 
            return _value;
        }

        virtual void setValue(int newValue){
            newValue = max(_lowerLimit,newValue);
            newValue = min(_upperLimit,newValue);
            if (newValue != _value){
                _value = newValue;
                setNewValueText(String(_value).c_str());
            }            
        }

        virtual bool onEvent(Event_Type event) {
            // handle Event based on selected logic
            switch(event){
                case EVENT_RIGHT: 
                    if (_value + _step <= _upperLimit){
                        setValue(_value+_step);
                    }
                    return true;
                    break;
                case EVENT_LEFT:
                    if (_value - _step >= _lowerLimit){
                        setValue(_value-_step);
                    }
                    return true;
                    break;
                case EVENT_ENTER: 
                    if (_withReset == true){
                        setValue(_resetValue);
                        return true;    
                    }
                    break;
            }
            return false;
        }

    protected:
        int    _value,_resetValue,_upperLimit,_lowerLimit,_step;
        bool   _withReset;
};


class MenuEntryList : public MenuEntryText{
    public:
        MenuEntryList(  const char *text,
                        char **     pValueList=NULL,
                        uint8_t     listSize=0,
                        int         initIndex=0,
                        bool        wrapAround = false,
                        bool        withRest = false,
                        const char * valuePre = "[",
                        const char * valuePost = "]",
                        uint8_t font=MENU_STD_FONT, 
                        uint8_t fontSize = MENU_STD_FONT_SCALE,
                        uint16_t  foregndcol = TFT_WHITE, 
                        uint16_t  backgndCol=TFT_BLACK,
                        uint16_t  offsetX = 5,
                        uint16_t  offsetY = 0)
            :MenuEntryText(text,"",valuePre,valuePost,font,fontSize,foregndcol,backgndCol,offsetX,offsetY),
            _currentIndex(-1),_resetIndex(initIndex),_pValueList(NULL),_listSize(listSize),_wrapAround(wrapAround),_withReset(withRest)      {   
            if (_listSize > 0) {
                // there could not be an old list (constructor)
                _pValueList = new String[_listSize];
                if (_pValueList==NULL)  STOP("failed to allocate buffer list for entries");
                for(int i=0;i < _listSize;i++){
                    if (pValueList[i] != NULL){
                        _pValueList[i] = pValueList[i]; 
                    } else {
                        _pValueList[i] = "";
                    }
                }
                _resetIndex = max(0,_resetIndex);
                _resetIndex = min(_listSize-1,_resetIndex);
                setIndex(_resetIndex);
                _currentIndex=_resetIndex;
            }
        }
        
        ~MenuEntryList(){
            _configMutex.lock();
            if (_pValueList != NULL){
                delete _pValueList;
                _pValueList = NULL;
            }
        }

        virtual const char * getValue(){ 
            _configMutex.lock();
            if ((_listSize > 0) &&
                (_currentIndex < _listSize) &&
                (_pValueList != NULL) ){
                    if (_pValueList[_currentIndex] != NULL){
                        _configMutex.free();
                        return _pValueList[_currentIndex].c_str();
                    }
            }
            _configMutex.free();
            return "";
        }

        virtual int getIndex(){
            if ((_listSize > 0) &&
                (_currentIndex < _listSize) &&
                (_pValueList != NULL) ){
                    return _currentIndex;
                }
            return -1;
        }

        virtual bool setIndex(int newIndex){
            _configMutex.lock();
            if ((newIndex >= 0) && (newIndex < _listSize) && (_pValueList != NULL))  {
                    _currentIndex = newIndex;
                    setNewValueText(_pValueList[_currentIndex]);
                    _configMutex.free();
                    return true;
            }            
            _configMutex.free();
            return false;
        }

        virtual bool setResetIndex(int resetIndex){
            _configMutex.lock();
            if ((resetIndex >= 0) && (resetIndex < _listSize) && (_pValueList != NULL))  {
                if (_pValueList[resetIndex] != NULL){
                        _resetIndex = resetIndex;
                        _withReset = true;
                        _configMutex.free();
                        return true;
                    }
            }            
            _configMutex.free();
            return false;
        }

        virtual bool setWrapAround(bool value){
            _wrapAround = value;
            return true;
        }

        virtual bool setValueList(char ** pValueList=NULL,uint8_t     listSize=0){
            _configMutex.lock();
            if (_pValueList != NULL){
                delete _pValueList;
                _pValueList = NULL;
            }  
            if ((listSize > 0) &&
                (pValueList != NULL) ){
                _listSize = listSize;
                _pValueList = new String[_listSize];
                if (_pValueList==NULL)  STOP("failed to allocate buffer list for entries");
                for(int i=0;i < _listSize;i++){
                    if (pValueList[i] != NULL){
                        _pValueList[i] = pValueList[i]; 
                    } else {
                        _pValueList[i] = "";
                    }
                }

                _resetIndex = max(0,_resetIndex);
                _resetIndex = min(_resetIndex,_listSize);
                _currentIndex = _resetIndex;
                setNewValueText(_pValueList[_currentIndex]);
                _configMutex.free();
                return true;
            }        
            _configMutex.free();
            return false;
        }

        virtual bool setValueList(String connectedList,char sep=','){
            uint8_t count=0;
            uint32_t i,length;
            length = connectedList.length();
            if (length == 0)    return false;

            for(i=0;i < length;i++){
                if (connectedList[i] == sep){
                    count++;
                }
            }
            count++; // entry ; entry   ==> one sep and two elements
            StringList list(connectedList.c_str(),sep);
            
            _configMutex.lock();
            if (_pValueList != NULL){
                delete _pValueList;
                _pValueList = NULL;
            }  

            _listSize = count;
            _pValueList = new String[_listSize];
            if (_pValueList==NULL)  STOP("failed to allocate buffer list for entries");
            for(int i=0;i < _listSize;i++){
                _pValueList[i] = list.getNextListEntry();
            }
            _resetIndex = max(0,_resetIndex);
            _resetIndex = min(_resetIndex,_listSize);
            _currentIndex = _resetIndex;
            setNewValueText(_pValueList[_currentIndex]);
            _configMutex.free();
            return true;            
        }


        virtual bool onEvent(Event_Type event) {
            _configMutex.lock();
            // check if object is ready
            if ((_listSize <= 0) ||  (_pValueList == NULL) )             event = EVENT_NONE;  // jump over default path to end
            if ((_currentIndex >= _listSize) || (_currentIndex < 0))    event = EVENT_NONE;  // jump over default path to end
            // if ready handle Event based on selected logic
            bool res = false;
            switch(event){
                case EVENT_RIGHT: 
                    if (_currentIndex < (_listSize-1)){
                        _currentIndex++;
                        setNewValueText(_pValueList[_currentIndex]);
                    } else if ((_wrapAround == true) && (_currentIndex == (_listSize-1))){
                        _currentIndex = 0;
                        setNewValueText(_pValueList[_currentIndex]);
                    }
                    res = true;
                    break;
                case EVENT_LEFT:
                    if (_currentIndex > 0){
                        _currentIndex--;
                        setNewValueText(_pValueList[_currentIndex]);
                    } else if ((_wrapAround == true) && (_currentIndex == 0)){
                        _currentIndex = _listSize-1;
                        setNewValueText(_pValueList[_currentIndex]);
                    }
                    res = true;
                    break;
                case EVENT_ENTER: 
                    if ((_withReset == true) && (_resetIndex >= 0) && (_resetIndex < _listSize)){
                        _currentIndex = _resetIndex;
                        setNewValueText(_pValueList[_currentIndex]);
                        res = true;    
                    }
                    break;
            }
            _configMutex.free();
            return res;
        }

    protected:
        int         _currentIndex,_resetIndex;
        String  *   _pValueList;
        uint8_t     _listSize;
        bool        _wrapAround,_withReset;
        Mutex       _configMutex;
};