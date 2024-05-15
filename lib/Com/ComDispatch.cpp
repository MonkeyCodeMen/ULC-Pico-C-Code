#include "ComDispatch.hpp"
#include "Debug.hpp"
#include "helper.h"

#include <LedObjects.hpp>
#include <Menu.hpp>

#include <SDcard.hpp>
#include <I2C_master.hpp>
#include <I2C_register.h>

extern void mainMenu_syncFromCtrl();

void printDirectory(SDFile dir, int numTabs) {
  while (true) {
    SDFile entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }

    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }

    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}


ComDispatch::ComDispatch()
{
}

bool ComDispatch::dispatchFrame(ComFrame * pFrame)
{
    bool res=false;
    switch(pFrame->module){
        case('C'):  res = dispatchCommonFrame(pFrame);    break;
        case('L'):  res = dispatchLedFrame(pFrame);       break;
        case('R'):  res = dispatchRgbLedFrame(pFrame);    break;
        case('S'):  res = dispatchNeoStripeFrame(pFrame); break;
        case('M'):  res = dispatchNeoMatrixFrame(pFrame); break;
        
        default:
            pFrame->res=F("unknown module");
            return false;
    }
    if (res == true){
        mainMenu_syncFromCtrl();
    }
    return res;
}

bool ComDispatch::dispatchCommonFrame(ComFrame * pFrame){
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
        Serial.println("directory of SD card");
        SDFile root = globalSDcard0.open("/");
        printDirectory(root,0);
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

    }
    pFrame->res = "unknown command for menu";
    return false;
}


bool ComDispatch::dispatchLedFrame(ComFrame * pFrame){
    LedCtrl * pLedCtrl;
    int res;
    switch(pFrame->index){
        case 0: pLedCtrl = &ledCtrl1; break;
        case 1: pLedCtrl = &ledCtrl2; break;
        case 2: pLedCtrl = &ledCtrl3; break;
        case 3: pLedCtrl = &ledCtrl4; break;
        default:
            pFrame->res = F("unknown index");
            return false;
    }
    if (pFrame->withPar == true){
        res = pLedCtrl->select(pFrame->command.c_str());
        if (res == ANI_OK) {
            res = pLedCtrl->config(pFrame->cfg);
        }
    } else {
        res = pLedCtrl->select(pFrame->command.c_str());  // use default parameter for 
    }
    pFrame->res = Ani::getErrorText(res);
    return res==ANI_OK?true:false;
}

bool ComDispatch::dispatchRgbLedFrame(ComFrame * pFrame){
    RgbLedCtrl * pRgbCtrl;
    switch(pFrame->index){
        case 0: pRgbCtrl = &rgbCtrl1; break;
        case 1: pRgbCtrl = &rgbCtrl2; break;
        default:
            pFrame->res=F("unknown index");
            return false;
    }

    int res;
    if (pFrame->withPar == true){
        res = pRgbCtrl->select(pFrame->command.c_str());
        if (res == ANI_OK){
            res = pRgbCtrl->config(pFrame->cfg);
        }
    } else {
        res = pRgbCtrl->select(pFrame->command.c_str());  // use default parameter for 
    }
    pFrame->res = Ani::getErrorText(res);
    return res==ANI_OK?true:false;
}

bool ComDispatch::dispatchNeoStripeFrame(ComFrame * pFrame){
    NeoStripeCtrl * pStripeCtrl;
    switch(pFrame->index){
        case 0: pStripeCtrl = &neoStripeCtrl1; break;
        case 1: pStripeCtrl = &neoStripeCtrl2; break;
        default:
            pFrame->res=F("unknown index");
            return false;
    }
    int res;
    if (pFrame->withPar == true){
        res = pStripeCtrl->select(pFrame->command.c_str());
        if (res == ANI_OK) {
            res = pStripeCtrl->config(pFrame->cfg);
        }
    } else {
        res = pStripeCtrl->select(pFrame->command.c_str());  // use default parameter for 
    }
    pFrame->res = Ani::getErrorText(res);
    return res==ANI_OK?true:false;
}

bool ComDispatch::dispatchNeoMatrixFrame(ComFrame * pFrame){
    NeoMatrixCtrl * pMatrixCtrl;
    switch(pFrame->index){
        case 0: pMatrixCtrl = &neoMatrixCtrl1; break;
        case 1: pMatrixCtrl = &neoMatrixCtrl2; break;
        default:
            pFrame->res=F("unknown index");
            return false;
    }
    int res;
    if (pFrame->withPar == true){
        res = pMatrixCtrl->select(pFrame->command.c_str());
        if (res == ANI_OK){
            res = pMatrixCtrl->config(pFrame->cfg);
        }
    } else {
        res = pMatrixCtrl->select(pFrame->command.c_str());  // use default parameter for 
    }
    pFrame->res = Ani::getErrorText(res);
    return res==ANI_OK?true:false;
}
