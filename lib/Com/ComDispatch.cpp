#include "ComDispatch.hpp"
#include "Debug.hpp"
#include "helper.h"

#include <LedObjects.hpp>
#include <Menu.hpp>

#include <SDcard.hpp>

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
    bool res;
    switch(pFrame->module){
        case('C'):  res = dispatchCommonFrame(pFrame);    break;
        case('L'):  res = dispatchLedFrame(pFrame);       break;
        case('R'):  res = dispatchRgbLedFrame(pFrame);    break;
        case('S'):  res = dispatchNeoStripeFrame(pFrame); break;
        case('M'):  res = dispatchNeoMatrixFrame(pFrame); break;
        case('E'):  res = dispatchMenuFrame(pFrame);      break;        // remote control of menu
        
        default:
            pFrame->res=F("unknown module");
            return false;
    }
    return res;
}

bool ComDispatch::dispatchCommonFrame(ComFrame * pFrame){
    return false;
}


bool ComDispatch::dispatchLedFrame(ComFrame * pFrame){
    LedCtrl * pLedCtrl;
    int res;
    switch(pFrame->index){
        case 0: pLedCtrl = pLedCtrl1; break;
        case 1: pLedCtrl = pLedCtrl2; break;
        case 2: pLedCtrl = pLedCtrl3; break;
        case 3: pLedCtrl = pLedCtrl4; break;
        default:
            pFrame->res = F("unknown index");
            return false;
    }
    if (pFrame->withPar == true){
        res = pLedCtrl->setup(pFrame->command.c_str());
        if (res == ANI_OK) {
            res = pLedCtrl->setup(pFrame->par1,pFrame->par2,pFrame->par3,pFrame->par4,pFrame->str,pFrame->length,&(pFrame->pData));
        }
    } else {
        res = pLedCtrl->setup(pFrame->command.c_str());  // use default parameter for 
    }
    pFrame->res = Ani::getErrorText(res);
    return res==ANI_OK?true:false;
}

bool ComDispatch::dispatchRgbLedFrame(ComFrame * pFrame){
    RgbLedCtrl * pRgbCtrl;
    switch(pFrame->index){
        case 0: pRgbCtrl = pRgbCtrl1; break;
        case 1: pRgbCtrl = pRgbCtrl2; break;
        default:
            pFrame->res=F("unknown index");
            return false;
    }

    int res;
    if (pFrame->withPar == true){
        res = pRgbCtrl->setup(pFrame->command.c_str());
        if (res == ANI_OK){
            res = pRgbCtrl->setup(pFrame->par1,pFrame->par2,pFrame->par3,pFrame->par4,pFrame->str,pFrame->length,&(pFrame->pData));
        }
    } else {
        res = pRgbCtrl->setup(pFrame->command.c_str());  // use default parameter for 
    }
    pFrame->res = Ani::getErrorText(res);
    return res==ANI_OK?true:false;
}

bool ComDispatch::dispatchNeoStripeFrame(ComFrame * pFrame){
    NeoStripeCtrl * pStripeCtrl;
    switch(pFrame->index){
        case 0: pStripeCtrl = pNeoStripeCtrl1; break;
        case 1: pStripeCtrl = pNeoStripeCtrl2; break;
        default:
            pFrame->res=F("unknown index");
            return false;
    }
    int res;
    if (pFrame->withPar == true){
        res = pStripeCtrl->setup(pFrame->command.c_str());
        if (res == ANI_OK) {
            res = pStripeCtrl->setup(pFrame->par1,pFrame->par2,pFrame->par3,pFrame->par4,pFrame->str,pFrame->length,&(pFrame->pData));
        }
    } else {
        res = pStripeCtrl->setup(pFrame->command.c_str());  // use default parameter for 
    }
    pFrame->res = Ani::getErrorText(res);
    return res==ANI_OK?true:false;
}

bool ComDispatch::dispatchNeoMatrixFrame(ComFrame * pFrame){
    NeoMatrixCtrl * pMatrixCtrl;
    switch(pFrame->index){
        case 0: pMatrixCtrl = pNeoMatrixCtrl1; break;
        case 1: pMatrixCtrl = pNeoMatrixCtrl2; break;
        default:
            pFrame->res=F("unknown index");
            return false;
    }
    int res;
    if (pFrame->withPar == true){
        res = pMatrixCtrl->setup(pFrame->command.c_str());
        if (res == ANI_OK){
            res = pMatrixCtrl->setup(pFrame->par1,pFrame->par2,pFrame->par3,pFrame->par4,pFrame->str,pFrame->length,&(pFrame->pData));
        }
    } else {
        res = pMatrixCtrl->setup(pFrame->command.c_str());  // use default parameter for 
    }
    pFrame->res = Ani::getErrorText(res);
    return res==ANI_OK?true:false;
}

bool ComDispatch::dispatchMenuFrame(ComFrame * pFrame){
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
    }
    pFrame->res = "unknown command for menu";
    return false;
}
