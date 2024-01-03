#pragma once
#include <Arduino.h>

#include <Debug.hpp>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include <MenuEntry.hpp>
#include <Mutex.hpp>


class MenuHandler{
    public:
        MenuHandler():
            _valid(false),_pTFT(NULL),_pTFTmutex(NULL),
            _pHeader(NULL),_pEntryList(NULL),_entryCountMax(-1),
            _hasHeader(false),_entryViewportChanged(false),_forceHeaderUpdate(false),_wrapAround(true),_placeboMutex(false),
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

        bool begin(MenuHeader * pHeader,MenuEntry ** pEntryList, u8_t listCount, TFT_eSPI * pTFT, Mutex * pDisplaySPImutex=NULL){
            // check ;-) pointer to TFT
            if (pTFT == NULL)           STOP(F_CHAR("invalid display driver"));
            _pTFT = pTFT;

            // init entries
            if ((listCount == 0)||(pEntryList==NULL)){    
                _pEntryList == NULL;
                _entryCountMax = 0;
            } else {
                _pEntryList = pEntryList;
                _entryCountMax = listCount;
                for (int i=0;i < _entryCountMax;i++){
                    if (pEntryList[i] == NULL)     STOP(F_CHAR("invalid entry pointer"));
                    pEntryList[i]->setup(_pTFT);
                }
            }

            // init header
            if (pHeader==NULL){
                _hasHeader = false;
            } else {
                _hasHeader = true;
                _pHeader= pHeader;
                _pHeader->setup(_pTFT);
            }

            if (pDisplaySPImutex == NULL){
                _pTFTmutex = new Mutex;
                _placeboMutex = true;
            } else {
                _pTFTmutex = pDisplaySPImutex;
                _placeboMutex = false;
            }

            // calc coordinates
            _screenHeight   = _pTFT->height();
            _screenWidth    = _pTFT->width();
            _menuX          = 0;  // ToDo: offset and limit to dedicated area
            _menuY          = 0;
            _menuHeight     = _screenHeight;
            _menuWidth      = _screenWidth;

            if (_hasHeader){
                _headerX = _menuX;
                _headerY = _menuY;
                _headerHeight = _pHeader->getHeight();
                _headerWidth = _menuWidth;
            } else {
                _headerX = _headerY = 0;
                _headerHeight = _headerWidth  = 0; 
            }
            // rest is reserved as entrie Area
            _entryAreaX = _menuX;
            _entryAreaY = _menuY + _headerHeight;
            _entryAreaHeight = _menuHeight - _headerHeight;
            _entryAreaWidth  = _menuWidth;

            if (_entryCountMax > 0){
            // calc visible entries
                _activeEntry = 0;
                _firstVisibleEntry = 0;
                _lastVisibleEntry = _calcLastVisibleEntry(_firstVisibleEntry);
            }

            return false;
        }


        bool setNewBackgroundColor(u16_t value){
            if (value != _backgroundColor){
                _backgroundColor = value;
                _entryViewportChanged = true;
                _forceHeaderUpdate = true;
            }
            return true;
        }

        bool onEvent(MENU_Event_Type event){

            return false;
        }

        bool loop(){
            if (_valid == false) return false;

            // check header for update
            if (_hasHeader == true){
                if ((_pHeader->needsUpdate() == true) || (_forceHeaderUpdate == true)){
                    _pHeader->draw(_pTFT,_headerX,_headerY,_headerWidth,_headerHeight);
                    _forceHeaderUpdate = false;
                }
            }

            // entry section
            if (_entryCountMax > 0){
                if (_entryViewportChanged == true){
                    // rebuild viewport
                    // lock SPI for TFT
                    _pTFTmutex->lock();
                        _pTFT->fillRect(_entryAreaX,_entryAreaY,_entryAreaWidth,_entryAreaHeight,_backgroundColor);
                        u16_t y = _entryAreaY;
                        for (u8_t line=_firstVisibleEntry; line <= _lastVisibleEntry; line++){
                            _pEntryList[line]->draw(_pTFT,_entryAreaX,y,_entryAreaWidth,_entryAreaHeight);
                        }
                    _pTFTmutex->free();
                } else {
                    // update viewport
                    u16_t y = _entryAreaY;
                    for (u8_t line=_firstVisibleEntry; line <= _lastVisibleEntry; line++){
                        if (_pEntryList[line]->needsUpdate() == true){
                            // lock SPI for TFT
                            _pTFTmutex->lock();
                                _pEntryList[line]->draw(_pTFT,_entryAreaX,y,_entryAreaWidth,_entryAreaHeight);
                            _pTFTmutex->free();
                        }
                    }
                }
            }
        }

    private:
        u8_t        _calcLastVisibleEntry(u8_t startEntry){
            if (startEntry >= _entryCountMax)   STOP(F_CHAR("invaid start index"));
            for (u8_t index = startEntry; index < _entryCountMax; index++){

            }
        }

        bool        _valid;

        // screen
        TFT_eSPI  * _pTFT;    
        Mutex     * _pTFTmutex;

        // Menu objects
        MenuHeader* _pHeader;
        MenuEntry** _pEntryList;
        int         _entryCountMax;

        // base control flags
        bool        _hasHeader,_entryViewportChanged,_forceHeaderUpdate,_wrapAround,_placeboMutex;
        
        // control of entry area
        int         _activeEntry;
        u8_t        _firstVisibleEntry;
        u8_t        _lastVisibleEntry;
        
        // coordinates & dimension
        u16_t       _screenHeight,_screenWidth;                                 // screen  (assumed to be starte at 0,0)
        u16_t       _menuX,_menuY,_menuHeight,_menuWidth;                       // area for menu
        u16_t       _headerX,_headerY,_headerHeight,_headerWidth;               // area for header
        u16_t       _entryAreaX,_entryAreaY,_entryAreaHeight,_entryAreaWidth;   // area for handling / scrolling menu entries

        // colors
        u16_t       _backgroundColor;
};

    


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
        
        features as bullet point list:
            * simple setup of entries and action by List of string (example the test at end of this file)
            * handling of entries/text and actions
            * updateable headline for changing status information 
            * menue tree with submenues possible (connected menues, handlin by class)
            * handling for various entry types included:
                + Sub Menue
                + Text
                + separation line
                + function call
                + Flags
                + INT 
                + LIST 
                * other can be added by derive from base class MenuEntry 
            * Arduino setup / loop style
            * class try to avoid unnecessary updates
    */