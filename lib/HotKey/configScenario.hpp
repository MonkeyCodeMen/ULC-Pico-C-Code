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
            } else {
                int count = arr.size();
                int count=0;
                for (JsonObject config : arr ){
                    DeserializationError error = deserializeJson(configJson, config);
                    if (error) {
                        String msg = "could not decode configItem Nr." + String(count) + " of scenario: " + _name;
                        debug.log(msg);
                    } else {
                        configItem configEntry(configJson);
                        _configList.add(configEntry);
                    }
                }
            }
        }
       
        String                 _name;
        EventType              _event;
        SimpleList<configItem> _configList;
};

