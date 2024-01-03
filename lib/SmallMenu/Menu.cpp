#include <Menu.hpp>

bool MenuHandler::begin(const char * headerText,MenuEntry ** pEntryList, u8_t listCount, TFT_eSPI * pTFT, Mutex * pDisplaySPImutex){
    if (strlen(headerText)== 0) STOP(F_CHAR("nobody is only allowed for Odysseus"));
    if (pTFT == NULL)           STOP(F_CHAR("invalid display driver"));
    if (pEntryList == NULL)     STOP(F_CHAR("invalid menu entry list"));
    if (listCount == 0)         STOP(F_CHAR("empty entry list"));

    for (int i=0;i < listCount;i++){
        if (pEntryList[i] == NULL)     STOP(F_CHAR("invalid entry pointer"));
    }

    _needsUpdate = true;
    _header = headerText;
    _headerBackgndCol = TFT_BLACK;
    _headerForegndCol = TFT_WHITE;
    _listCount = listCount;
    _pEntryList = pEntryList;
    _currentEntry = 0;
    _pTFT = pTFT;
    _font = 2;
    _currentEntry = 0; 

 
    _pTFT->setTextColor(_headerForegndCol,_headerBackgndCol); 
    _pTFT->setTextFont(_font); 
    _pTFT->setTextSize(1);
 
    _fontHeight = pTFT->fontHeight();
    _screenHeigth = _pTFT->height();
    _linesPerPage = _screenHeigth / _fontHeight;
 

    if (_linesPerPage < 2) return false;
    if (_listCount < 2) return false;
    _valid = true;

    return true;
}

