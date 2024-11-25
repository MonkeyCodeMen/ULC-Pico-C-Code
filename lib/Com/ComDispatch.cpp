
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


#include "ComDispatch.hpp"
#include "Debug.hpp"
#include "helper.h"

#include <LedObjects.hpp>
#include <BufferedClock.hpp>
#include <Menu.hpp>

#include <SDcard.hpp>
#include <I2C_master.hpp>
#include <I2C_register.h>

#include <Base64.hpp> 

extern void mainMenu_syncFromCtrl();

String printDirectory(SDFile dir, String in=String("") , int numTabs=0,String baseDir=String("~")) {
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

    String name(entry.name());
    
    if (entry.isDirectory()) {
        if (baseDir == "~"){
            res += name+"/\n";
            res = printDirectory(entry, res,numTabs+1 );
        } else {
            res += baseDir+name+"/\n";
            res = printDirectory(entry, res,0,baseDir+name+"/" );    
        }
    
    } else {
        // files have sizes, directories do not
        if (baseDir == "~"){
            res+=String("    ")+name+"*"+String(entry.size(), DEC)+"\n";
        } else {
            res+=baseDir+name+"*"+String(entry.size(), DEC)+"\n";
        }
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
    }  else if (pFrame->command == "FILEREAD") {
        return _readFile(pFrame);
    } else if (pFrame->command == "FILEWRITE") {
        return _writeFile(pFrame);
    } else if (pFrame->command == "FILELIST"){
        pFrame->res = printDirectory(globalSDcard0.open("/"),"directory of SD card:\n ",0,"/"); 
        return true;
    }

    pFrame->res = "unknown common comand";
    return false;
}

bool ComDispatch::_dump(ComFrame * pFrame){
    int resNr=ANI_ERROR_GENERAL;

    char module;
    uint8_t index;
    String ref,prg;
    StringList strList(pFrame->cfg.str.c_str(),"~&~");
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





bool ComDispatch::_readFile(ComFrame *pFrame) {
    StringList strList(pFrame->cfg.str.c_str(), "~&~");
    String cmdType = strList.getNextListEntry();

    if (cmdType == "INIT") {
        _fileTransferState.reset();

        _fileTransferState.filename = strList.getNextListEntry();
        SDFile file = globalSDcard0.open(_fileTransferState.filename.c_str(), FILE_READ);
        if (!file) {
            pFrame->res = "File not found: " + _fileTransferState.filename;
            return false;
        }

        _fileTransferState.fileSize = file.size();
        _fileTransferState.totalChunks = (_fileTransferState.fileSize + MAX_FILE_CHUNK_SIZE - 1) / MAX_FILE_CHUNK_SIZE;
        _fileTransferState.currentChunk = 0;
        _fileTransferState.isActive = true;

        pFrame->res = "INIT~&~" + _fileTransferState.filename + "~&~" + String(_fileTransferState.fileSize) + "~&~" +
                      String(_fileTransferState.totalChunks);
        file.close();
        return true;
    }

    if (!_fileTransferState.isActive) {
        pFrame->res = "No active file read sequence.";
        return false;
    }

    uint32_t chunk = strList.getNextListEntry().toInt();
    if (chunk != _fileTransferState.currentChunk) {
        pFrame->res = "Invalid chunk order.";
        return false;
    }

    SDFile file = globalSDcard0.open(_fileTransferState.filename.c_str(), FILE_READ);
    if (!file) {
        pFrame->res = "File not found: " + _fileTransferState.filename;
        return false;
    }

    uint32_t offset = _fileTransferState.currentChunk * MAX_FILE_CHUNK_SIZE;
    file.seek(offset);
    uint8_t buffer[MAX_FILE_CHUNK_SIZE];
    uint32_t bytesToRead = min((uint32_t)MAX_FILE_CHUNK_SIZE, _fileTransferState.fileSize - offset);
    file.read(buffer, bytesToRead);
    file.close();

    char bufferBase64[COM_FRAME_MAX_STR_LENGTH];
    encode_base64(buffer, bytesToRead,(uint8_t*) bufferBase64);
    pFrame->res = "DATA~&~" + _fileTransferState.filename + "~&~" + String(_fileTransferState.currentChunk) + "~&~" + String(bufferBase64);

    _fileTransferState.currentChunk++;
    if (_fileTransferState.currentChunk >= _fileTransferState.totalChunks) {
        _fileTransferState.reset();
    }

    return true;
}

bool ComDispatch::_writeFile(ComFrame *pFrame) {
    StringList strList(pFrame->cfg.str.c_str(), "~&~");
    String cmdType = strList.getNextListEntry();

    if (cmdType == "INIT") {
        _fileTransferState.reset();

        _fileTransferState.filename = strList.getNextListEntry();
        _fileTransferState.fileSize = strList.getNextListEntry().toInt();
        _fileTransferState.totalChunks = strList.getNextListEntry().toInt();
        _fileTransferState.currentChunk = 0;
        _fileTransferState.isActive = true;

        SDFile file = globalSDcard0.open(_fileTransferState.filename.c_str(), FILE_WRITE);
        if (!file) {
            pFrame->res = "Failed to create file: " + _fileTransferState.filename;
            _fileTransferState.reset();
            return false;
        }

        file.close();
        pFrame->res = "INIT~&~" + _fileTransferState.filename + "~&~" + String(_fileTransferState.fileSize) + "~&~" +
                      String(_fileTransferState.totalChunks);
        return true;
    }

    if (!_fileTransferState.isActive) {
        pFrame->res = "No active file write sequence.";
        return false;
    }

    uint32_t chunk = strList.getNextListEntry().toInt();
    if (chunk != _fileTransferState.currentChunk) {
        pFrame->res = "Invalid chunk order.";
        return false;
    }

    String base64Data = strList.getNextListEntry();
    uint8_t buffer[MAX_FILE_CHUNK_SIZE];
    size_t decodedLength = decode_base64((const uint8_t *)base64Data.c_str(), base64Data.length(), buffer);


    if (decodedLength == 0) {
        pFrame->res = "Base64 decode failed.";
        return false;
    }

    SDFile file = globalSDcard0.open(_fileTransferState.filename.c_str(), O_APPEND);
    if (!file) {
        pFrame->res = "Failed to write to file: " + _fileTransferState.filename;
        _fileTransferState.reset();
        return false;
    }

    file.write(buffer, decodedLength);
    file.close();

    _fileTransferState.currentChunk++;
    if (_fileTransferState.currentChunk >= _fileTransferState.totalChunks) {
        _fileTransferState.reset();
        pFrame->res = "File write completed.";
    } else {
        pFrame->res = "DATA~&~" + _fileTransferState.filename + "~&~" + String(_fileTransferState.currentChunk);
    }

    return true;
}
