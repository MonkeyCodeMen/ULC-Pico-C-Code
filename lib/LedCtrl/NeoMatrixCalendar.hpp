#pragma once
#include <Arduino.h>

#include "helper.h"
#include "Debug.hpp"
#include "BufferedClock.hpp"
#include "NeoMatrixGif.hpp"

#include <vector>

/*
    class to implement an advents calendar based on NeoMatrixGif

    for each day (1-24) there is a subdirectory "./advent/01/" to ./advent/24/" holding the GIF's animation for the day
    there is common directory "./advent/add/" holding additional animations that will be mixed with the dailies one on a rare base
    there is a default (fallback/all other) directory that will be used if the daily directory is not available

    the base, additonal and default subdirectories must at least exist an the default must at least contain one file, 
       if not the the object will remain in stop mode
       one gif in default is needed !! even if all day directories are available and filled

    this class now take the current date in a loop, select the coresponding animation directory and shows the content (various if animatoins for the day on the screen)

    SDFile is reetrant safe 
    SD use a separate bus (special ussage of SPI ) ==> no MUTEX needed  for SD access


    dir structure:


    +--/calendar 
       +--add/
       |  +-- pic1.gif
       |  +-- pic2.gif
       +--def/
       |  +--add/
       |  |  +-- pic3.gif
       |  |  +-- pic4.gif
       |  +-- pic6.gif
       |  +-- pic7.gif
       |  +-- pic8.gif
       +--01/
       |  +--add/
       |  |  +-- 01add1.gif
       |  |  +-- 01add2.gif
       |  +-- day_01a.gif
       |  +-- day_01b.gif
       +--02/
       |  +--add/
       |  |  +-- 02add1.gif
       |  |  +-- 02add2.gif
       |  +-- day_02a.gif
       |  +-- day_02b.gif


    base: calendar/        
        base directory of calendar
    
    daily: 01/ 02/  def/
        one sub dir for every day
        in the daily directory will be shown with same probability (1- all other probability)

        additional for this day:  01/add/   02/add/   def/add/
            animations from this subdirectories will be shown with probability : DAILY_ADD_PROBABILITY  (0-100)

    global additional:  add/
        animations from this subdirectories will be shown with probability : GLOBAL_ADD_PROBABILITY  (0-100)

        in each directory there can be an additional subdirectory, 
            the files from this directory will be selected with a certain probability
        

*/

#define NEO_MATRIX_CALENDAR_WEIGHT_DAILY        100
#define NEO_MATRIX_CALENDAR_WEIGHT_DAILY_ADD    10
#define NEO_MATRIX_CALENDAR_WEIGHT_GLOBAL_ADD   1


class MatrixGifCalendarAni : public MatrixGifFileAni{
    public:
        MatrixGifCalendarAni() :
            _weightDaily(100),
            _weightDailyAdd(10),
            _weightGlobalAdd(1) {
            _pName = "calendar";
            _weightTotal = _weightDaily+_weightDailyAdd+_weightGlobalAdd;
            _clearAll();    // function instead of init list, because will be used several time
        };    

        void reset() { 
            config(AniCfg( ANI_WR_ALL | 0x40,0,0,0,"advent/" )); 
        }

        int  config(AniCfg cfg){
            _clearAll();

            // setup dir structure names
            // until now only one list entry .. but prepared for further extension use a StringList
            StringList *    pList;
            pList = new StringList(cfg.str.c_str(),"#~#"); 
            _dirBase = pList->getNextListEntry();
            _dirBase = removeLeadingCharacters(_dirBase,' ');
            _dirBase = removeTrailingCharacters(_dirBase,' ');
            _dirGlobalAdd  = _dirBase + "ADD/";
            _dirDefault    = _dirBase + "DEF/";
            _dirDefaultAdd = _dirBase + "DEF/ADD/";

            // test required path
            String msg = " directory test failed for :";
            if (_testDirectory(_dirBase) == false) {
                msg += "base directory : " + _dirBase;
                LOG(msg.c_str());
                return ANI_ERROR_FILE_NOT_FOUND;
            }
            if (_testDirectory(_dirGlobalAdd) == false) {
                msg += "_dirGlobalAdd directory : " + _dirGlobalAdd;
                LOG(msg.c_str());
                return ANI_ERROR_FILE_NOT_FOUND;
            }
            if (_testDirectory(_dirDefault) == false) {
                msg += "_dirDefault directory : " + _dirDefault;
                LOG(msg.c_str());
                return ANI_ERROR_FILE_NOT_FOUND;
            }
            _addFilesToList(_dirDefault    ,".GIF", _defaultFileList   ,false ,100);
            _addFilesToList(_dirDefaultAdd ,".GIF", _defaultAddFileList,false ,100);
            _addFilesToList(_dirGlobalAdd  ,".GIF", _globalAddFileList ,true  ,100);

            // test if default directory is not empty
            if (_defaultFileList.empty()){
                _clearAll();
                LOG(F("there is no file in the default (fallback) directory"));
                return ANI_ERROR_FILE_NOT_FOUND;
            }
            // do base class config 
            cfg.str = "";   // list completly handled in this instance .. so delete list for base class init
            Ani::config(cfg); 

            _selectNextFile(true);  // select start file with force of directory update
            _state = init;
            return ANI_OK;
        }

        void loop(uint32_t now,Adafruit_NeoMatrix * pMatrix){
            u16_t color;
            u8_t dim;
            int nr;

            switch (_state){
                case stop:
                    // do nothing parameters are loocked by other thread
                    // the setup will move forward to state init
                    break;

                case init:
                    Ani::loop(now);
                    pMatrix->fillScreen(toColor565(getColor()));
                    pMatrix->setBrightness(getDim());

                    _gif.close();
                    _gif.open((const char *)_currentFile.c_str(),_GIFOpenFile, _GIFCloseFile, _GIFReadFile, _GIFSeekFile, _GIFDraw);

                    _lastFrame = now;
                    _wait = 1;

                    _state = run;
                    break;
                
                case run:
                    Ani::loop(now);
                    if (now-_lastFrame >= _wait){
                        _lastFrame = now;
                        if (hasChanged()){
                            pMatrix->setBrightness(getDim());
                        }
                        int res = _gif.playFrame(false,&_wait,pMatrix);
                        if(res == 0){
                            // last gif frame has been played 
                            _gif.close();
                            _gif.open((const char *)_currentFile.c_str(),_GIFOpenFile, _GIFCloseFile, _GIFReadFile, _GIFSeekFile, _GIFDraw);
                            //pMatrix->fillScreen(toColor565(getColor()));
                            _gif.playFrame(false,&_wait,pMatrix);
                            // now it's time to select new/next file  (with possible update of filelists)
                            _selectNextFile();
                        } else if (res == -1){
                            _frameError();
                        }
                    }
                    break;
            }
        }

        String dump(){
            String out = Ani::dump();
            out += "current date: " + _currentDate.timestamp() + "; last date : " + _lastDate.timestamp() + "\n";
            out += "dir:             " + _dirBase        + "\n";
            out += "dir global add:  " + _dirGlobalAdd   + "\n";
            out += "dir default:     " + _dirDefault     + "\n";
            out += "dir default add: " + _dirDefaultAdd  + "\n";
            out += "dir daily:       " + _dirDaily       + "\n";
            out += "dir daily add:   " + _dirDailyAdd    + "\n";
            out += "current File:    " + _currentFile    + "\n";

            out += "weights::  total(" +String(_weightTotal) + ") /daily(" + String(_weightDaily) + ") /dailyAdd(" + String(_weightDailyAdd) +") /globalAdd(" + String(_weightGlobalAdd) +")\n";
            out += "current random number: " + String(_randomValue) +"\n"; 

            out += "File list global add:\n";
            for(int i=0; i < _globalAddFileList.size(); i++){
                out += "     " + _globalAddFileList[i] + "\n";
            }

            out += "File list default:\n";
            for(int i=0; i < _defaultFileList.size(); i++){
                out += "     " + _defaultFileList[i] + "\n";
            }

            out += "File list default add:\n";
            for(int i=0; i < _defaultAddFileList.size(); i++){
                out += "     " + _defaultAddFileList[i] + "\n";
            }

            out += "File list daily:\n";
            for(int i=0; i < _dailyFileList.size(); i++){
                out += "     " + _dailyFileList[i] + "\n";
            }

            out += "File list daily add:\n";
            for(int i=0; i < _dailyAddFileList.size(); i++){
                out += "     " + _dailyAddFileList[i] + "\n";
            }

            return out;
        }


    protected:      
        String                      _dirBase,_dirGlobalAdd,_dirDefault,_dirDefaultAdd;
        String                      _dirDaily,_dirDailyAdd,_currentFile;
        DateTime                    _currentDate;
        DateTime                    _lastDate;
        std::vector<String>         _dailyFileList;
        std::vector<String>         _dailyAddFileList;
        std::vector<String>         _defaultFileList;
        std::vector<String>         _defaultAddFileList;
        std::vector<String>         _globalAddFileList;

        uint32_t                    _weightTotal;
        uint32_t                    _weightDaily;
        uint32_t                    _weightDailyAdd;
        uint32_t                    _weightGlobalAdd;
        uint32_t                    _randomValue;
        

        void _clearAll(){
            _state          = stop;
            _dirBase        = "";
            _dirGlobalAdd   = "";
            _dirDefault     = "";
            _dirDefaultAdd  = "";
            _dirDaily       = "";
            _dirDailyAdd    = "";
            _currentFile    = "";
            _lastDate       = DateTime(2000,1,1,0,0,0);
            _currentDate    = DateTime(2000,1,2,0,0,0);
            _randomValue    = 0;
            _dailyFileList.clear();
            _dailyAddFileList.clear();
            _defaultFileList.clear();
            _defaultAddFileList.clear();
            _globalAddFileList.clear();
        }

        void _selectNextFile(bool forceDirUpdate = false){
            // update dirs if new date or forced (will be checked in function)
            _selectDirectory(forceDirUpdate);

            // Generate a random number between 0 and totalWeight - 1
            _randomValue = random(_weightTotal);

            // Select the appropriate list based on the random value
            if (_randomValue < _weightDaily) {
                // standard daily animation
                int index = random(_dailyFileList.size());
                _currentFile = _dailyFileList[index];
            } else if (_randomValue < (_weightDaily + _weightDailyAdd)) {
                // daily additional animation
                int count = _dailyAddFileList.size();
                if (count > 0){
                    int index = random(count);
                    _currentFile = _dailyAddFileList[index];
                } else {
                    int index = random(_dailyFileList.size());
                    _currentFile = _dailyFileList[index];
                }
            } else {
                // global additional animation
                int count = _globalAddFileList.size();
                if (count > 0){
                    int index = random(count);
                    _currentFile = _globalAddFileList[index];
                } else {
                    int index = random(_dailyFileList.size());
                    _currentFile = _dailyFileList[index];
                }
            }

            String msg = "next file selected :"+_currentFile;
            LOG(msg.c_str());
        }

        bool _testDirectory(String path){
            SDFile entry = globalSDcard0.open(path.c_str());
            if (entry.isDirectory()){
                entry.close();
                return true;
            }
            entry.close();
            return false;
        }

        void _selectDirectory(bool forceNew = false){
            _currentDate = bufferedClock.getLoopDateTime();
            uint8_t day = _currentDate.day();

            if ((day == _lastDate.day()) && (forceNew == false)){
                // no change in day number ==> keep file lists
                return;
            }

            _lastDate = _currentDate;

            if (day < 10){
                _dirDaily = _dirBase + "0" + String(day,DEC) + "/";
            } else {
                _dirDaily = _dirBase + String(day,DEC) + "/";
            }

            _dirDailyAdd= _dirDaily + "ADD/";
            _dailyFileList.clear();
            _dailyAddFileList.clear();
            _addFilesToList(_dirDaily     ,".GIF"  ,_dailyFileList    ,false, 100);
            _addFilesToList(_dirDailyAdd  ,".GIF"  ,_dailyAddFileList ,false, 100);

            if (_dailyFileList.empty()){
                // fallback to default directory
                String msg = "current dir is empty {" + _dirDaily + "} .. fallback to default";
                LOG(msg.c_str());
                _dirDaily         = _dirDefault;
                _dailyFileList    = _defaultFileList; 
                _dirDailyAdd      = _dirDefaultAdd;
                _dailyAddFileList = _defaultAddFileList;
            } 
        }

        void _addFilesToList(const String& path, const String& ext,std::vector<String>& fileList,bool recursive = false,int minSize=0){
            SDFile dir = globalSDcard0.open(path.c_str());
            if (!dir || !dir.isDirectory()) {
                String msg = "Error: Unable to open directory:" + path ;
                LOG(msg.c_str());
                return;
            }

            // dir walk
            while (true) {
                SDFile entry = dir.openNextFile();
                if (!entry) {
                    break; // no more entries
                }

                if (entry.isDirectory()) {
                    // do nothing == no recursive walk !!!
                    if (recursive==true){
                        _addFilesToList(path + "/" + entry.name(), ext, fileList);
                    }
                } else {
                    // check for GIF file and filesize > 0
                    String fileName = entry.name();
                    if ((fileName.endsWith(ext)) && (entry.size() >= minSize)) {
                        // add file name to list 
                        fileList.push_back(path + fileName);
                    }
                }
                entry.close();
            }
            dir.close();
        }

};

