#pragma once

#include <Arduino.h>
#include <MainConfig.h>
#include <Events.h>

/****
 * 
 * JSON config file for hot key
 * 
 * {
 * 
 *   {
 *     scenario:"name",
 *     hot_key: "EVENT_A1",
 *     {
 *        group : "LED0",
 *        program: "name",
 *        param: ["","","",""],
 *        str:"",
 *        bin:[0,2,255,40,30]
 *     },
 *     {
 *        group : "RGB1",
 *        program: "name",
 *        param: ["","","",""],
 *        str:"",
 *        bin:[0,2,255,40,30]
 *     }
 *  }
 * }
 * 
*/

class configItem{
    public:
        configItem():_ctrlObj(NULL),_cmd(""),_p1(0),_p2(0),_p3(0),_p4(0),_str(""),_pData(NULL),_dataSize(0) {};
        ~configItem() = default;

        CtrlPtr     _ctrlObj;
        String      _cmd;
        uint32_t    _p1,_p2,_p3,_p4;
        String      _str;
        uint8_t     * _pData;
        uint32_t    _dataSize;
};

class configSet{
    public:
        
        
    private:
        String      _name;
        uint8_t     _count;
        configItem  _entry[CTRL_OBJECT_COUNT];
        
};


class MenuHotkey
{
    public:
        MenuHotkey();
        ~MenuHotkey() = default;

        int eventNameToNr(String name);

    private:
        String  _configFileName;

};
