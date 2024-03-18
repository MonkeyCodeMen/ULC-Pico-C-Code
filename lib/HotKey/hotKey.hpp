#pragma once

#include <Arduino.h>
#include <MainConfig.h>
#include <configScenario.hpp>
#include <ArduinoJson.h>
#include <SDcard.hpp>



/****
 * 
 * JSON config file for hot key
 * 
 * {
 *      "project"     : "ULC pico : universal light controller raspberry prico"
 *      "config_type" : "scenario configuration file"
 *      "date"        : "24.12.2024"
 *      "author"      : "MonkeyCodeMen"
 *      "scenarios":[
 *          { configScenario },
 *          { configScenario },
 *          { configScenario },
 *          { configScenario },
 *          { configScenario },
 *          { configScenario }
 *       ]
 * }
 * 
*/



class HotKey
{
    public:
        HotKey(String configFile) {
            _configFileName = configFile;
            SDFile * pFile = new SDFile();
            *pFile = globalSDcard0.open(_configFileName,FILE_READ);
            

        }
        ~HotKey() = default;

        

    private:
        String  _configFileName;
        String  _project,_date,_author;
        

};
