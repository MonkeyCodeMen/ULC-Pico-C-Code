
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
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include <MenuItem.hpp>
#include <MenuHeaderText.hpp>
#include <MenuEntryText.hpp>

#include <Mutex.hpp>

class MenuHandler{
    public:
        MenuHandler():
            _valid(false),_pTFT(NULL),_pTFTmutex(NULL),
            _pHeader(NULL),_pEntryList(NULL),_entryCountMax(-1),
            _hasHeader(false),_entryViewportChanged(false),_forceHeaderUpdate(false),_placeboMutex(false),
            _activeEntry(-1),_firstVisibleEntry(0),_lastVisibleEntry(0),
            _screenHeight(0),_screenWidth(0),
            _menuX(0),_menuY(0),_menuHeight(0),_menuWidth(0),
            _headerX(0),_headerY(0),_headerHeight(0),_headerWidth(0),
            _entryAreaX(0),_entryAreaY(0),_entryAreaHeight(0),_entryAreaWidth(0),
            _backgroundColor(TFT_BLACK)
            { }

        ~MenuHandler(){
            _valid = false;
            if(_placeboMutex == true)  delete _pTFTmutex;
        }

        bool begin(MenuHeader * pHeader,MenuEntry ** pEntryList, uint8_t listCount, TFT_eSPI * pTFT, Mutex * pDisplaySPImutex=NULL){
            int res;
            _valid = false;
            _pTFT = pTFT;
            if (_pTFT == NULL)                                      { LOG(F("invalid display driver"));                    return false;   }
            if ((res = _initMenuArea())                     != 0)   { LOG_INT(F("invalid menu area line:"),res);           return false;   }
            if ((res = _initHeader(pHeader))                != 0)   { LOG_INT(F("invalid header object line:"),res);       return false;   }
            if ((res = _initEntries(pEntryList,listCount))  != 0)   { LOG_INT(F("invalid entry pointer line:"),res);       return false;   }
            if ((res = _initMutex(pDisplaySPImutex))        != 0)   { LOG_INT(F("MUTEX for SPI access error line:"),res);  return false;   }
            if ((res = _calcCoordinates())                  != 0)   { LOG_INT(F("faile to calc sections line:"),res);      return false;   }
            if ((res = _checkMenuSize())                    != 0)   { LOG_INT(F("invalid menu dimension line:"),res);      return false;   }
            if ((res = _checkHeaderSize())                  != 0)   { LOG_INT(F("invalid header dimension line:"),res);    return false;   }
            if ((res = _checkEntriesSize())                 != 0)   { LOG_INT(F("invalid entry dimension line:"),res);     return false;   }
            if ((_hasHeader == false) && (_entryCountMax == 0))     { LOG(F("WARNING empty menu"));                                        }
            setActiveEntry(0);
            _setViewportFromTop(0);
            _valid = true;
            return true;
        }

        bool setNewBackgroundColor(uint16_t  value){
            if (value != _backgroundColor){
                _backgroundColor = value;
                _entryViewportChanged = true;
                _forceHeaderUpdate = true;
            }
            return true;
        }

        bool onEvent(EventType event){
            if (_valid == false)    { return false; }

            // first try if event is handled by current active menu entry
            if (_pEntryList[_activeEntry]->onEvent(event) == true){
                return true;
            }

            // next check if up or down .. than move view
            if (event == EVENT_DOWN){
                if (_activeEntry < _entryCountMax-1){
                    setActiveEntry(_activeEntry+1);
                    if(_activeEntry > _lastVisibleEntry){
                        _setViewportFromBottom(_activeEntry);
                    }
                }
                return true;
            } else if (event == EVENT_UP){
                if (_activeEntry > 0){
                    setActiveEntry(_activeEntry-1);
                    if(_activeEntry < _firstVisibleEntry){
                        _setViewportFromTop(_activeEntry);
                    }
                }
                return true;
            }

            // now try hotkey

            return false;
        }

        bool loop(uint32_t now){
            // run as fast as possible (no fixed loop time)
            if (_valid == false) { return false; }
            _updateHeader();
            _updateEntrySection();
            return true;
        }
        
        bool setActiveEntry(uint8_t activeEntry){
            if (_entryCountMax == 0)            return false; // no menu entries 
            if (activeEntry == _activeEntry)    return true;  // nothing to do
            if (activeEntry >= _entryCountMax)  return false; // invalid entry selected

            if ((_activeEntry >= 0) && (_activeEntry < _entryCountMax)) _pEntryList[_activeEntry]->clearFocus();
            _activeEntry = activeEntry;
            _pEntryList[_activeEntry]->setFocus();
            return true;
        }

    private:
        bool _setViewportFromTop(uint8_t firstVisibleEntry){
            if (_entryCountMax > 0){
            // calc visible entries
                if (firstVisibleEntry >= _entryCountMax)   return false;
                _valid = false;

                _firstVisibleEntry = _lastVisibleEntry = firstVisibleEntry;

                uint16_t  dy = _pEntryList[_firstVisibleEntry]->getHeight();
                uint16_t  nextHeight;
                while (_lastVisibleEntry+1 < _entryCountMax){
                    nextHeight = _pEntryList[_lastVisibleEntry+1]->getHeight();
                    if (dy +  nextHeight < _entryAreaHeight){
                        _lastVisibleEntry++;
                        dy+=nextHeight;
                    } else {
                        break; // no more space for next entry
                    }
                }
                _entryViewportChanged = true;
                _valid = true;
            }
            return true;
        }

        bool _setViewportFromBottom(uint8_t lastVisibleEntry){
            if (_entryCountMax > 0){
            // calc visible entries
                if (lastVisibleEntry >= _entryCountMax)   return false;
                _valid = false;

                _firstVisibleEntry = _lastVisibleEntry = lastVisibleEntry;

                uint16_t  dy = _pEntryList[_lastVisibleEntry]->getHeight();
                uint16_t  nextHeight;
                while (_firstVisibleEntry > 1){
                    nextHeight = _pEntryList[_firstVisibleEntry-1]->getHeight();
                    if (dy +  nextHeight < _entryAreaHeight){
                        _firstVisibleEntry--;
                        dy+=nextHeight;
                    } else {
                        break; // no more space for next entry
                    }
                }
                _entryViewportChanged = true;
                _valid = true;
            }
            return true;
        }

        int _initMenuArea(){
            // just like Jack for now:
            // take all and give nothing back
            _screenHeight   = _pTFT->height();
            _screenWidth    = _pTFT->width();
            _menuX          = 0;  // ToDo: offset and limit to dedicated area
            _menuY          = 0;
            _menuHeight     = _screenHeight;
            _menuWidth      = _screenWidth;
            return 0;
        }

        int _initHeader(MenuHeader * pHeader){
            // init header
            if (pHeader==NULL){
                _hasHeader = false;
            } else {
                _hasHeader = true;
                _pHeader= pHeader;
                _pHeader->setup(_pTFT,_menuHeight,_menuWidth);
            }
            return 0;
        }

        int _initEntries(MenuEntry ** pEntryList, uint8_t listCount){
            // init entries
            if ((listCount == 0)||(pEntryList==NULL)){    
                _pEntryList == NULL;
                _entryCountMax = 0;
            } else {
                _pEntryList = pEntryList;
                _entryCountMax = listCount;
                for (int i=0;i < _entryCountMax;i++){
                    if (_pEntryList[i] == NULL)     return __LINE__;
                    _pEntryList[i]->setup(_pTFT,_menuHeight,_menuWidth);
                }
            }
            return 0;
        }

        int _initMutex(Mutex * pDisplaySPImutex){
            if (pDisplaySPImutex == NULL){
                _pTFTmutex = new Mutex;
                if (_pTFTmutex == NULL) return __LINE__;
                _placeboMutex = true;
            } else {
                _pTFTmutex = pDisplaySPImutex;
                _placeboMutex = false;
            }
            return 0;
        }

        int _calcCoordinates(){
            if (_hasHeader){
                _headerX = _menuX;
                _headerY = _menuY;
                _headerHeight = _pHeader->getHeight();
                _headerWidth = _menuWidth;
            } else {
                _headerX = _headerY = 0;
                _headerHeight = _headerWidth  = 0; 
            }

            // rest is reserved as entry section
            _entryAreaX = _menuX;
            _entryAreaY = _menuY + _headerHeight;
            _entryAreaHeight = _menuHeight - _headerHeight;
            _entryAreaWidth  = _menuWidth;
            return 0;
        }

        /*** ToDo for all checks and calcs .. check width .. right now width is complete screen ***/
        int _checkMenuSize(){
            if (_menuY  > 0+_screenHeight-1)                        { return __LINE__; }   // menu area must start inside of screen area
            if (_menuY+_menuHeight-1 > 0+_screenHeight-1 )          { return __LINE__; }   // menu are must end inside of screen area
            return 0;
        }

        int _checkHeaderSize(){
            if (_hasHeader == false)                                { return 0;  }         // no header ==> no check
            if (_headerHeight > _menuHeight)                        { return __LINE__; }   // header must fit into menu area
            if (_headerY      < _menuY)                             { return __LINE__; }   // header start must be inside of menu area 
            if (_headerY      > _menuY+_menuHeight-1)               { return __LINE__; }   // header start must be inside of menu area 
            if (_headerY+_headerHeight-1 > _menuY+_menuHeight-1)    { return __LINE__; }   // header end must be inside of menu area
            return 0;
        }

        int _checkEntriesSize(){
            if (_entryCountMax == 0)                                { return 0;  }         // no entry section ==> no check
            if (_entryAreaHeight > _menuHeight-_headerHeight)       { return __LINE__; }   // must fit into (rest) area
            if (_entryAreaY      < _menuY)                          { return __LINE__; }   // must start in menu area
            if (_entryAreaY      > _menuY+_menuHeight-1)            { return __LINE__; }   // must start in menu area
            if (_entryAreaY      <= _headerY+_headerHeight-1)       { return __LINE__; }   // must start after header section
            if (_entryAreaY+_entryAreaHeight-1>_menuY+_menuHeight-1){ return __LINE__; }    // must end inside menu area
            // idea every element must fit in _entryArea
            for(uint8_t i=0; i < _entryCountMax;i++){
                if (_pEntryList[i]->getHeight() > _entryAreaHeight){
                    return __LINE__;
                }
            }
            return 0;
        }

        void _updateHeader(){
            // check header for update
            if (_hasHeader == true){
                if ((_pHeader->needsUpdate() == true) || (_forceHeaderUpdate == true)){
                    _pTFT->setViewport(_headerX,_headerY,_pHeader->getWidth(),_pHeader->getHeight()); // until now we trust the the object for height and width
                    _pHeader->draw(_pTFT,0,0);
                    _pTFT->resetViewport();
                    _forceHeaderUpdate = false;
                }
            }
        }

        void _updateEntrySection(){
            if (_entryCountMax > 0){
                if (_entryViewportChanged == true){
                    // rebuild viewport
                    _pTFTmutex->lock();
                        uint16_t  y = _entryAreaY;
                        for (uint8_t line=_firstVisibleEntry; line <= _lastVisibleEntry; line++){
                            _pTFT->setViewport(_entryAreaX,y,_entryAreaWidth,_pEntryList[line]->getHeight()); // until now we trust the the object for height and width
                            _pEntryList[line]->draw(_pTFT,0,0);
                            _pTFT->resetViewport();
                            y+=_pEntryList[line]->getHeight();
                        }
                        // check for half visible entries
                        if (y < _entryAreaY+_entryAreaHeight-1) {
                            uint16_t  restHeight =  _entryAreaY+_entryAreaHeight - y;
                            if (_lastVisibleEntry+1 < _entryCountMax){
                                // draw a part of the next entry_
                                _pTFT->setViewport(_entryAreaX,y,_entryAreaWidth,restHeight);
                                _pEntryList[_lastVisibleEntry+1]->draw(_pTFT,0,0);
                                _pTFT->resetViewport();
                            } else {
                                // just erase the area, there is no more entry
                                _pTFT->fillRect(_entryAreaX,y,_entryAreaWidth,restHeight,_backgroundColor);
                            }
                        }
                    _pTFTmutex->free();
                    _entryViewportChanged = false;
                } else {
                    // update viewport (did not moved)
                    uint16_t  y = _entryAreaY;
                    for (uint8_t line=_firstVisibleEntry; line <= _lastVisibleEntry; line++){
                        if (_pEntryList[line]->needsUpdate() == true){
                            _pTFTmutex->lock();
                                _pTFT->setViewport(_entryAreaX,y,_entryAreaWidth,_pEntryList[line]->getHeight()); // until now we trust the the object for height and width
                                _pEntryList[line]->draw(_pTFT,0,0);
                                _pTFT->resetViewport();
                            _pTFTmutex->free();
                        }
                        y+=_pEntryList[line]->getHeight();
                    }
                    // check update for half visible entries
                    if ((y < _entryAreaY+_entryAreaHeight-1) && (_lastVisibleEntry+1 < _entryCountMax)){
                        if (_pEntryList[_lastVisibleEntry+1]->needsUpdate() == true){
                            uint16_t  restHeight =  _entryAreaY+_entryAreaHeight - y;
                            _pTFTmutex->lock();
                                _pTFT->setViewport(_entryAreaX,y,_entryAreaWidth,restHeight);
                                _pEntryList[_lastVisibleEntry+1]->draw(_pTFT,0,0);
                                _pTFT->resetViewport();
                            _pTFTmutex->free();
                        }
                    }
                }
            }
        }


        /////////////////////////////////////////////////////////////////////
        // member attributes
        bool        _valid;

        // screen
        TFT_eSPI  * _pTFT;    
        Mutex     * _pTFTmutex;

        // Menu objects
        MenuHeader* _pHeader;
        MenuEntry ** _pEntryList;
        int         _entryCountMax;

        // base control flags
        bool        _hasHeader,_entryViewportChanged,_forceHeaderUpdate,_placeboMutex;
        
        // control of entry area
        int         _activeEntry;
        uint8_t        _firstVisibleEntry;
        uint8_t        _lastVisibleEntry;
        
        // coordinates & dimension
        uint16_t        _screenHeight,_screenWidth;                                 // screen  (assumed to be starte at 0,0)
        uint16_t        _menuX,_menuY,_menuHeight,_menuWidth;                       // area for menu
        uint16_t        _headerX,_headerY,_headerHeight,_headerWidth;               // area for header
        uint16_t        _entryAreaX,_entryAreaY,_entryAreaHeight,_entryAreaWidth;   // area for handling / scrolling menu entries

        // colors
        uint16_t        _backgroundColor;
};



