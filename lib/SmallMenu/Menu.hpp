#pragma once
#include <Arduino.h>

#include <Debug.hpp>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include <MenuEntry.hpp>
#include <Mutex.hpp>


class MenuHandler{
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



    public:
        MenuHandler():
            _valid(false),_pTFT(NULL),_pEntryList(NULL),_listCount(-1),
            _header(""),_headerForegndCol(0),_headerBackgndCol(0)  
            {};
        ~MenuHandler() = default;

        bool begin(const char * headerText,MenuEntry ** pEntryList, u8_t listCount, TFT_eSPI * pTFT, Mutex * pDisplaySPImutex);

        void loop();





    private:
        bool        _valid;
        bool        _needsUpdate;
        TFT_eSPI  * _pTFT;    
        MenuEntry** _pEntryList;
        int         _listCount;
        String      _header;
        u16_t       _headerForegndCol,_headerBackgndCol;
        Mutex       _pTFTmutex;
        u8_t        _currentEntry;
        u8_t        _font;
        u8_t        _linesPerPage;
        u16_t       _fontHeight,_screenHeigth;
        u16_t       _ox,_oy;
};

    
