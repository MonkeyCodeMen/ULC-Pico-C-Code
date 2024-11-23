
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
            if (globalSDcard0.exists(fileName) == true){
                SDFile file;
                file = globalSDcard0.open(fileName,FILE_READ);
                String streamIn = file.readString();
                file.close();
                _configFileName = fileName;
                return openFromString(streamIn);
            }
            String msg = "file not found: " + String(fileName);
            debug.log(msg);
            return false;
        }

        bool onEvent(EventType event){
            configScenario item;
            if (_scenarioList.size() > 0){
                for (int i = 0 ;i < _scenarioList.size(); i++){
                    item = _scenarioList.get(i);
                    if (item.event() == event)
                        return item.run();
                }
            }
            return false;
        }


    private:
        String _configFileName,_project,_date,_author,_type,_version;
        SimpleList<configScenario>  _scenarioList;
        

};

extern configCollection globalHotkey;
