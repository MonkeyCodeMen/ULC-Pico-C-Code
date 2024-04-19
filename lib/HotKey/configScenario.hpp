#pragma once

#include <Arduino.h>
#include <SimpleList.hpp>
#include <Events.hpp>
#include <configItem.hpp>
#include <ArduinoJson.h>
#include <Debug.hpp>



/****
 * 
 * JSON config file for hot key
 * 
 *          {
 *              "name"   : "OFF",
 *              "key"    : "A1"
 *              "configs":[
 *                  { configItem },
 *                  { configItem },
 *                  { configItem } 
 *              ]
 *          }
 * 
*/

class configScenario{
    public:
        configScenario():_name(""),_event(EVENT_NONE)    {                           }
        ~configScenario()                                {   _configList.clear();    }
        configScenario(JsonDocument scenario){
            _name = String((const char*) scenario["name"]);
            _event = eventNameToNr((const char*) scenario["key"]);
            
            JsonDocument configJson;
            JsonArray arr = scenario["configs"].as<JsonArray>();
            if (arr == nullptr){
                // configItems .. empty config scenario
                String msg = "no configs found in scenario :"+ String(_name);
                debug.log(msg);
                return;
            } else {
                int count=0;
                for (JsonObject config : arr ){
                    DeserializationError error = deserializeJson(configJson, config);
                    if (error) {
                        String msg = "could not decode configItem Nr." + String(count) + " of scenario: " + _name;
                        debug.log(msg);
                    } else {
                        configItem configEntry(configJson);
                        _configList.add(configEntry);
                        count++;
                    }
                }
            }
        }


        String      name()      { return _name;                 }
        EventType   event()     { return _event;                }
        uint32_t    count()     { return _configList.size();    }
        configItem config(uint32_t i){
            if (i < _configList.size()){
                return _configList.get(i);
            }
            return configItem();  // return empty item
        }

    protected:   
        String                 _name;
        EventType              _event;
        SimpleList<configItem> _configList;
};

