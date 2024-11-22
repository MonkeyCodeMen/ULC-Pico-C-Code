#include "ComDispatch.hpp"
#include "Debug.hpp"
#include "helper.h"

#include <LedObjects.hpp>
#include <BufferedClock.hpp>
#include <Menu.hpp>

#include <SDcard.hpp>
#include <I2C_master.hpp>
#include <I2C_register.h>

extern void mainMenu_syncFromCtrl();

String printDirectory(SDFile dir, String in=String("") , int numTabs=0) {
  String res=in;
  while (true) {
    SDFile entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }

    for (uint8_t i = 0; i < numTabs; i++) {
      res += "  ";
    }

    res += entry.name();
    if (entry.isDirectory()) {
      res += ("/\n");
      res = printDirectory(entry, res , numTabs + 1);
    } else {
      // files have sizes, directories do not
      res+=("    ");
      res+=String(entry.size(), DEC);
      res+="\n";
    }
    entry.close();
  }
  return res;
}


ComDispatch::ComDispatch()
{
}

bool ComDispatch::dispatchFrame(ComFrame * pFrame)
{
    bool res=false;
    int resNr;
    switch(pFrame->module){
        case('C'):  res = _dispatchCommonFrame(pFrame);    break;
        case('L'):
        case('R'):
        case('S'):
        case('M'):  res = _dispatchLedFrame(pFrame);        break;
        default:
            pFrame->res=F("invalid module");
            res = false;
    }

    if (res == true){
        mainMenu_syncFromCtrl();
    }

    return res;
}

bool ComDispatch::_dispatchLedFrame(ComFrame * pFrame){
    Ctrl* pObj = _refToObj(pFrame->module,pFrame->index);
    int resNr=ANI_ERROR_GENERAL;
    bool res=false;
    if (pObj == nullptr){
        pFrame->res = F("invalid index");
        return false;
    }
    if ((pFrame->command == "dump") || (pFrame->command=="Dump") || (pFrame->command=="DUMP")){
        resNr = pObj->dump(pFrame->res);
    }       
    if (pFrame->withPar == true){
        resNr = pObj->select(pFrame->command.c_str());
        if (res == ANI_OK) {
            resNr = pObj->config(pFrame->cfg);
        } 
    } else {
        resNr = pObj->select(pFrame->command.c_str());  // use default parameter for this program change
    }

    if (resNr == ANI_OK) return true;

    pFrame->res = Ani::getErrorText(resNr);     
    return false;       
}


bool ComDispatch::_dispatchCommonFrame(ComFrame * pFrame){
    // ignore index 
    pFrame->command.toUpperCase();

    if (pFrame->command == "UP"){
        return menuHandler.onEvent(EVENT_UP);
    } else if (pFrame->command == "DOWN"){
        return menuHandler.onEvent(EVENT_DOWN);
    } else if (pFrame->command == "LEFT"){
        return menuHandler.onEvent(EVENT_LEFT);
    } else if (pFrame->command == "RIGHT"){
        return menuHandler.onEvent(EVENT_RIGHT);
    } else if (pFrame->command == "ENTER"){
        return menuHandler.onEvent(EVENT_ENTER);
    } else if (pFrame->command == "DIR"){
        pFrame->res = printDirectory(globalSDcard0.open("/"),"directory of SD card:\n "); 
        return true;
    } else if (pFrame->command == "MEM"){
        LOG_MEM(F("mem log requested by COM interface"));
        return true;
    } else if (pFrame->command == "SLAVE_LED"){
        I2C_slave_CtrlReg_struct reg;
        i2c_master.readStatusReg(I2C_ADR_SLAVE,&reg);
        reg.LED_BLINK ^= 1;
        i2c_master.writeCtrlReg(I2C_ADR_SLAVE,&reg); 
        return true;
    } else if (pFrame->command == "CLOCK") {
        pFrame->res = bufferedClock.getLoopDateTime().timestamp();
        return true;
    } else if (pFrame->command == "CLOCKSET") {
        // e.g. "2020-06-25T15:29:37".
        DateTime set=DateTime(pFrame->cfg.str.c_str());
        bufferedClock.udateRTC(set);
        pFrame->res = bufferedClock.getLoopDateTime().timestamp().c_str();
        return true;
    } else if (pFrame->command == "DUMPCALENDAR0") {
        return neoMatrixCtrl1.dump(pFrame->res,"calendar");
    } else if (pFrame->command == "DUMPCALENDAR1") {
        return neoMatrixCtrl2.dump(pFrame->res,"calendar");
    } else if (pFrame->command == "DUMP") {
        return _dump(pFrame);
    }

    pFrame->res = "unknown common comand";
    return false;
}

bool ComDispatch::_dump(ComFrame * pFrame){
    int resNr=ANI_ERROR_GENERAL;

    char module;
    uint8_t index;
    String ref,prg;
    StringList strList(pFrame->cfg.str.c_str(),"#~#");
    ref = strList.getNextListEntry();
    prg = strList.getNextListEntry();
    if (ref.length() != 2) {
        pFrame->res = F("invalid module/index reference");
        return false;
    }
    module = ref[0];
    index = convertDezCharToInt(ref[1]);
    Ctrl* pObj = _refToObj(module,index);
    if (pObj == nullptr ){
        pFrame->res = F("invalid module/index reference");
        return false;
    }

    if (prg == ""){ 
        // use current program
        resNr = pObj->dump(pFrame->res);
    } else {
        // use given program
        resNr = pObj->dump(pFrame->res,prg.c_str());
    }

    if (resNr == ANI_OK) return true;

    pFrame->res = Ani::getErrorText(resNr);     
    return false;  
}


Ctrl * ComDispatch::_refToObj(char module,uint8_t index){
switch(module){
        case('L'):
            if      (index == 0)    { return &ledCtrl1;       }
            else if (index == 1)    { return &ledCtrl2;       }
            else if (index == 2)    { return &ledCtrl3;       }
            else if (index == 3)    { return &ledCtrl4;       }
            else                    { return nullptr;         }
            break;
        case('R'):
            if      (index == 0)    { return &rgbCtrl1;       }
            else if (index == 1)    { return &rgbCtrl2;       }
            else                    { return nullptr;         }
            break;
        case('S'):
            if      (index == 0)    { return &neoStripeCtrl1; }
            else if (index == 1)    { return &neoStripeCtrl2; }
            else                    { return nullptr;         }
            break;
        case('M'):
            if      (index == 0)    { return &neoMatrixCtrl1; }
            else if (index == 1)    { return &neoMatrixCtrl2; }
            else                    { return nullptr;         }
            break;
    }    
    return nullptr;
}