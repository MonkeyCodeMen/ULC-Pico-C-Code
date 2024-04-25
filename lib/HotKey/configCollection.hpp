#pragma once

#include <Arduino.h>
#include <MainConfig.h>


#include <ArduinoJson.h>
#include <configScenario.hpp>
#include <SDcard.hpp>
#include <SimpleList.hpp>


/****
 * 
 * JSON config file for hot key
 * 
 * {
 *      "project"     : "ULC pico : universal light controller raspberry prico"
 *      "type"        : "scenario configuration file"
 *      "date"        : "24.12.2024"
 *      "author"      : "MonkeyCodeMen"
 *      "version"     : "1.2.3"
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



class configCollection
{
    public:
        configCollection():_configFileName("") {  }
        ~configCollection() = default;

        uint32_t    count()     { return _scenarioList.size();  }
        String      fileName()  { return _configFileName;       }
        String      project()   { return _project;              }
        String      date()      { return _date;                 }
        String      author()    { return _author;               }
        String      type()      { return _type;                 }
        String      version()   { return _version;              }

        configScenario  scenario(EventType event){
            configScenario item;
            if (_scenarioList.size() > 0){
                for (int i = 0 ;i < _scenarioList.size(); i++){
                    item = _scenarioList.get(i);
                    if (item.event() == event)
                        return item;
                }
            }
            return configScenario();
        }

        configScenario  scenario(uint32_t index){
            configScenario item;
            item = _scenarioList.get(index);
            return item;
        }

        bool openFromString(String streamIn){
            JsonDocument config;
            DeserializationError error = deserializeJson(config, streamIn);
            if (error) {
                String msg = "could not decode config file :"+ String(_configFileName);
                debug.log(msg);
                return false;
            }

            _project = String((const char*) config["project"]);
            _date    = String((const char*) config["date"]);
            _author  = String((const char*) config["author"]);
            _type    = String((const char*) config["type"]);
            _version = String((const char*) config["version"]);

            // fix NULL pointer
            if (_project.c_str() == NULL)   _project = "";
            if (_date.c_str() == NULL)      _date    = "";
            if (_author.c_str() == NULL)    _author  = "";
            if (_type.c_str() == NULL)      _type    = "";
            if (_version.c_str() == NULL)   _version = "";
            
            JsonDocument scenarioJson;
            JsonArray arr = config["scenarios"].as<JsonArray>();
            if (arr == nullptr){
                // configItems .. empty config scenario
                String msg = "no scenarios found in file :"+ String(_configFileName);
                debug.log(msg);
                return false;
            } else {
                for (JsonObject item : arr ){
                    configScenario scenario(item);
                    _scenarioList.add(scenario);
                }
            }
            return true;
        }

        bool openFromFile(String fileName) {
            SDFile * pFile = new SDFile();
            *pFile = globalSDcard0.open(fileName,FILE_READ);
            if (pFile == NULL){
                String msg = "could not open config file " + String(fileName);
                debug.log(msg);
                return false;
            }
            String streamIn = pFile->readString();
            pFile->close();
            _configFileName = fileName;

            return openFromString(streamIn);
        }


    private:
        String _configFileName,_project,_date,_author,_type,_version;
        SimpleList<configScenario>  _scenarioList;
        

};
