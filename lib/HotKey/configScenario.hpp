#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SimpleList.hpp>
#include <Events.hpp>
#include <configItem.hpp>
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
        configScenario():_name(""),_event(EVENT_NONE)   {                           }
        configScenario(configScenario&  src)            { *this = src;              }            
        ~configScenario()                               { _configList.clear();      }

        configScenario(JsonDocument scenario){
            _event = eventNameToNr((const char*) scenario["key"]);
            _name = (const char *) scenario["name"];
            // fix NULL pointer return
            if (_name.c_str() == NULL) _name = "";
            
            JsonDocument configJson;
            JsonArray arr = scenario["configs"].as<JsonArray>();
            if (arr == nullptr){
                // configItems .. empty config scenario
                String msg = "no configs found in scenario :"+ String(_name);
                debug.log(msg);
                return;
            } else {
                for (JsonObject item : arr ){
                    configItem configEntry(item);
                    _configList.add(configEntry);
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

        void operator=(configScenario const& src){
            _configList.clear();
            _name  = src._name;
            _event = src._event;

            // before calling of member function we must get rid of const qualifier
            configScenario& ref = const_cast <configScenario&>(src);
            for(uint32_t i=0;i < ref.count() ;i++){
                _configList.add(ref.config(i)); 
            }  
        }


    protected:   
        String                 _name;
        EventType              _event;
        SimpleList<configItem> _configList;
};

