#pragma once

#include <NeoMatrixGif.hpp>
#include <BufferedClock.hpp>
#include <vector.hpp>

/*
    class to implement an advents calendar based on NeoMatrixGif

    for each day (1-24) there is a subdirectory "./advent/01/" to ./advent/24/" holding the GIF's animation for the day
    there is common directory "./advent/add/" holding additional animations that will be mixed with the dailies one on a rare base
    there is a default (fallback/all other) directory that will be used if the daily directory is not available

    the base, additonal and default subdirectories must at least exist an the default must at least contain one file, 
       if not the the object will remain in stop mode
       one gif in default is needed !! even if all day directories are available and filled

    this class now take the current date in a loop, select the coresponding animation directory and shows the content (various if animatoins for the day on the screen)

*/

class MatrixGifCalendarAni : public MatrixGifFileAni{
    public:
        MatrixGifCalendarAni()  {
            NeoMatrixAni("calendar");
            _clearAll();    // function instead of init list, because will be used several time
        };    

        void reset() { 
            config(AniCfg( ANI_WR_ALL | 0x40,0,0,0,"advent/" )); 
        }

        int  config(AniCfg cfg){
            StringList *    pList;
            pList = new StringList(cfg.str.c_str(),"#~#");
            _dirBase = pList->getNextListEntry();
            _dirBase = removeLeadingCharacters(' ');
            _dirBase = removeTrailingCharacters(' ');

            _dirAdditional = _dirBase + "add/";
            _dirDefault    = _dirBase + "def/";

            // test required path
            if ( (_testDirectory(_dirBase) == false)       ||
                 (_testDirectory(_dirAdditional) == false) ||
                 (_testDirectory(_dirDefault) == false)       ){
                _clearAll();
                return ANI_ERROR_FILE_NOT_FOUND;
            }

            // test if default directory is not empty
            _selectDirectory(_dirDefault);
            if (_currentFileList.empty()){
                return ANI_ERROR_FILE_NOT_FOUND;
            }

            _selectDirectory();


            return ANI_OK;
        }

    protected:      
        String                      _dirBase,_dirAdditional,_dirDefault;
        String                      _currentDir,_currentFile;
        DateTime                    _currentDate;
        const std::vector<String>   _currentFileList;


        void _clearAll(){
            _dirBase        = "";
            _dirAdditional  = "";
            _dirDefault     = "";
            _currentDir     = "";
            _currentFile    = "";
            _currentDate    = bufferedClock.getLoopDateTime();
            _state          = stop;
        }

        bool _testDirectory(String path){
            return false;
        }

        void _selectDirectory(){
            uint8_t day = _currentDate.day();
            if (day < 10){
                _currentDir = _dirBase + "0" + 
            }

        }

        void _selectDirector(String& path){

        }
        



};
