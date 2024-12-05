
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
    // pFrame->command.toUpperCase();

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
    } else if (pFrame->command == "CLOCK set") {
        // e.g. "2020-06-25T15:29:37".
        DateTime set=DateTime(pFrame->cfg.str.c_str());
        if (bufferedClock.udateRTC(set,true) == false){
            pFrame->res = "update failed current time : " + bufferedClock.getLoopDateTime().timestamp();    
            return false;
        } else {
            pFrame->res = bufferedClock.getLoopDateTime().timestamp();
            return true;
        }
    } else if (pFrame->command == "DUMP CALENDAR0") {
        return neoMatrixCtrl1.dump(pFrame->res,"calendar");
    } else if (pFrame->command == "DUMP CALENDAR1") {
        return neoMatrixCtrl2.dump(pFrame->res,"calendar");
    } else if (pFrame->command == "DUMP") {
        return _dump(pFrame);
    }  else if (pFrame->command == "FILE read") {
        return _readFile(pFrame);
    } else if (pFrame->command == "FILE write") {
        return _writeFile(pFrame);
    } else if (pFrame->command == "FILE list"){
        pFrame->res = printDirectory(globalSDcard0.open("/"),"directory of SD card:\n ",0,"/"); 
        return true;
    } else if (pFrame->command == "FILE delete"){
        return _deleteFile(pFrame);
    } else if (pFrame->command == "FILE mkdir"){
        return _createDirectory(pFrame);
    } else if (pFrame->command == "FILE rmdir"){
        return _deleteDirectory(pFrame);
    }

    pFrame->res = "Error: unknown common comand.";
    return false;
}

bool ComDispatch::_dump(ComFrame * pFrame){
    int resNr=ANI_ERROR_GENERAL;

    char module;
    uint8_t index;
    String ref,prg;
    Split strList(pFrame->cfg.str,(char*)"~&~");
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
    uint32_t sequenz = pFrame->cfg.COM_FILE_P1.uint32;

    if (sequenz == COM_FILE_INIT) {
        _fileTransferState.reset();

        _fileTransferState.filename = pFrame->cfg.str;
        SDFile file = globalSDcard0.open(_fileTransferState.filename.c_str(), FILE_READ);
        if (!file) {
            pFrame->res = "Error: File not found: " + _fileTransferState.filename;
            pFrame->cfg.COM_FILE_P1.uint32 = 0;  // P1
            pFrame->cfg.COM_FILE_P2.uint32 = 0;  // P2
            pFrame->cfg.COM_FILE_P3.uint32 = 0;  // P3
            pFrame->cfg.COM_FILE_P4.uint32 = 0;  // P4
            pFrame->cfg.str                = _fileTransferState.filename;

            return false;
        }

        _fileTransferState.fileSize = file.size();
        _fileTransferState.totalChunks = (_fileTransferState.fileSize + MAX_FILE_CHUNK_SIZE - 1) / MAX_FILE_CHUNK_SIZE;
        _fileTransferState.currentChunk = 1;  // expect to start with [chunk 1]  run to  [total chunk]
        _fileTransferState.isActive = true;

        pFrame->cfg.COM_FILE_P1.uint32  = COM_FILE_INIT;  // P1 INIT
        pFrame->cfg.COM_FILE_P2.uint32  = 0;  // P2 current chunk of init = 0
        pFrame->cfg.COM_FILE_P3.uint32  = _fileTransferState.totalChunks;   // P3
        pFrame->cfg.COM_FILE_P4.uint32  = _fileTransferState.fileSize;  // P4
        pFrame->cfg.str                 = _fileTransferState.filename;

        pFrame->res = "";
        file.close();
        return true;
    } else if (sequenz == COM_FILE_DATA){
        // check state
        if (!_fileTransferState.isActive) {
            pFrame->res = "Error: No active file read sequence.";
            return false;
        }

        // check chunk number
        uint32_t chunk = pFrame->cfg.colorCfg.uint32;
        if ((chunk != _fileTransferState.currentChunk) || (chunk < 1)) {
            pFrame->res = "Error: Invalid chunk order.";
            return false;
        }

        // read data
        SDFile file = globalSDcard0.open(_fileTransferState.filename.c_str(), FILE_READ);
        if (!file) {
            pFrame->res = "Error: File not found: " + _fileTransferState.filename;
            return false;
        }
        uint32_t offset = (_fileTransferState.currentChunk-1) * MAX_FILE_CHUNK_SIZE;
        file.seek(offset);
        uint8_t buffer[MAX_FILE_CHUNK_SIZE];
        uint32_t bytesToRead = min((uint32_t)MAX_FILE_CHUNK_SIZE, _fileTransferState.fileSize - offset);
        file.read(buffer, bytesToRead);
        file.close();

        // fill answer frame
        char bufferBase64[COM_FRAME_MAX_STR_LENGTH];
        encode_base64(buffer, bytesToRead,(uint8_t*) bufferBase64);
        pFrame->cfg.COM_FILE_P1.uint32  = COM_FILE_DATA;    // sequenz
        pFrame->cfg.COM_FILE_P2.uint32  = _fileTransferState.currentChunk;  // P2 current chunk 
        pFrame->cfg.COM_FILE_P3.uint32  = _fileTransferState.totalChunks;   // P3 total chunk
        pFrame->cfg.COM_FILE_P4.uint32  = _fileTransferState.fileSize;      // P4 filesize
        pFrame->cfg.str                 =  String(bufferBase64);
        pFrame->res = "";

        // count 
        _fileTransferState.currentChunk++;
        if (_fileTransferState.currentChunk > _fileTransferState.totalChunks) {
            // this was the last chunk
            _fileTransferState.reset();
        }

        return true;
    }

    pFrame->res = "Error: Invalid sequenze id P1:" + String(sequenz,HEX) ;
    _fileTransferState.reset();
    return false;

}

bool ComDispatch::_writeFile(ComFrame *pFrame) {
    uint32_t sequenz = pFrame->cfg.COM_FILE_P1.uint32;

    if (sequenz == COM_FILE_INIT) {
        _fileTransferState.reset();
        _fileTransferState.filename     = pFrame->cfg.str;
        _fileTransferState.currentChunk = pFrame->cfg.COM_FILE_P2.uint32;
        _fileTransferState.totalChunks  = pFrame->cfg.COM_FILE_P3.uint32;
        _fileTransferState.fileSize     = pFrame->cfg.COM_FILE_P4.uint32;
        _fileTransferState.isActive     = true;
        pFrame->res = "";

        if (SD.exists(_fileTransferState.filename.c_str())) {
            // delete file if already exist
            if (!SD.remove(_fileTransferState.filename.c_str())) {
                pFrame->res = "Error: Failed to delte existing file with same filename.";
                return false;
            } 
            pFrame->res = "Existing file deleted.";
        }

        SDFile file = globalSDcard0.open(_fileTransferState.filename.c_str(), FILE_WRITE);
        if (!file) {
            pFrame->res += "Error: Failed to create new file: " + _fileTransferState.filename;
            _fileTransferState.reset();
            return false;
        }

        file.close();
        pFrame->cfg.COM_FILE_P1.uint32  = COM_FILE_INIT;  // P1 INIT
        pFrame->cfg.COM_FILE_P2.uint32  = 0;  // P2 current chunk of init = 0
        pFrame->cfg.COM_FILE_P3.uint32  = _fileTransferState.totalChunks;   // P3
        pFrame->cfg.COM_FILE_P4.uint32  = _fileTransferState.fileSize;  // P4
        pFrame->cfg.str                 = _fileTransferState.filename;
        
        _fileTransferState.currentChunk = 1;    // expect transfer starts with first chunk

        return true;
    } else if (sequenz == COM_FILE_DATA){
        // check state
        if (!_fileTransferState.isActive) {
            pFrame->res = "Error: No active file write sequence.";
            return false;
        }
        // check order
        uint32_t chunk = pFrame->cfg.COM_FILE_P2.uint32;
        if (chunk != _fileTransferState.currentChunk) {
            pFrame->res = "Error: Invalid chunk order.";
            return false;
        }

        // decode data
        String base64Data = pFrame->cfg.str;
        uint8_t buffer[MAX_FILE_CHUNK_SIZE];
        size_t decodedLength = decode_base64((const uint8_t *)base64Data.c_str(), base64Data.length(), buffer);
        if (decodedLength == 0) {
            pFrame->res = "Error: Base64 decode failed.";
            return false;
        }

        // write data
        SDFile file = globalSDcard0.open(_fileTransferState.filename.c_str(), O_APPEND | O_WRITE);
        if (!file) {
            pFrame->res = "Error: Failed to write to file: " + _fileTransferState.filename;
            _fileTransferState.reset();
            return false;
        }
        file.write(buffer, decodedLength);
        file.close();

        if (_fileTransferState.currentChunk == _fileTransferState.totalChunks) {
            // this was the last chunk
            _fileTransferState.reset();
            pFrame->res = "File write completed.";
        } else {
            _fileTransferState.currentChunk++;
        }
        return true;
    }

    pFrame->res = "Error: Invalid sequenze id P1:" + String(sequenz,HEX) ;
    _fileTransferState.reset();
    return false;
}


/**
 * @brief Deletes a directory and its contents from the SD card.
 * 
 * @param path Absolute path to the directory (e.g., "/myDir").
 * @return "" if the directory and its contents were successfully deleted, otherwise "Error msg"
 */
String ComDispatch::_deleteDirectory(const String& path) {
    String res="";
    // Check if the path exists
    if (!SD.exists(path.c_str())) {
        res = "Error: Path does not exist: " + path;
        return res;
    }

    // Open the directory
    SDFile dir = SD.open(path.c_str());
    if (!dir || !dir.isDirectory()) {
        res = "Error: Path is not a directory: " + path;
        return res;
    }

    // Recursively delete files and subdirectories
    while (true) {
        SDFile entry = dir.openNextFile();
        if (!entry) {
            // No more entries
            break;
        }

        if (entry.isDirectory()) {
            // Recursively delete subdirectory
            String subDir = path + "/" + entry.name();
            String subRes = _deleteDirectory(subDir);
            if ( subRes != "") {
                entry.close();
                dir.close();
                return subRes;
            }
        } else {
            // Delete file
            String filePath = path + "/" + entry.name();
            if (!SD.remove(filePath.c_str())) {
                res = "Error: Failed to delete file: " + filePath;
                entry.close();
                dir.close();
                return res;
            }
            /*
            String msg = "Deleted file: " + filePath;
            LOG(msg.c_str());
            */
        }
        entry.close();
    }
    dir.close();

    // Remove the directory itself
    if (!SD.rmdir(path.c_str())) {
        res = "Error: Failed to remove directory: " + path;
        return res;
    }
    return res; // success Res should be stil ""
}


/**
 * @brief Deletes a directory and its contents from the SD card.
 * 
 * @param pFrame->cfg.str Absolute path to the directory (e.g., "/myDir").
 * @return true if the directory and its contents were successfully deleted, otherwise false and error message stored in pFrame->res
 */
bool ComDispatch::_deleteDirectory(ComFrame * pFrame) {
    String res = _deleteDirectory(pFrame->cfg.str);
    if (res != "") return false;
    return true;
}

/**
 * @brief Creates a subdirectory on the SD card.
 * 
 * @param pFrame-cfg.str Absolute path to the directory (e.g., "/myDir").
 * @return true if the directory was successfully created, false otherwise and error message stored in pFrame->res
 */
bool ComDispatch::_createDirectory(ComFrame * pFrame) {
    String path = pFrame->cfg.str;
    // Check if the path already exists
    if (SD.exists(path.c_str())) {
        pFrame->res="Error: Path already exists.";
        return false;
    }

    // Attempt to create the directory
    if (SD.mkdir(path.c_str())) {
        return true;
    } else {
        pFrame->res = "Error: Failed to create directory.";
        return false;
    }
}

/**
 * @brief Deletes a file from the SD card.
 * 
 * @param pFrame->cfg.str Absolute path to the file (e.g., "/example.txt").
 * @return true if the file was successfully deleted, false otherwise and error message stored in pFrame->res
 */
bool ComDispatch::_deleteFile(ComFrame * pFrame) {
    String filePath=pFrame->cfg.str;
    // Check if the file exists
    if (!SD.exists(filePath.c_str())) {
        pFrame->res = "Error: File does not exist.";
        return false;
    }

    // Attempt to delete the file
    if (SD.remove(filePath.c_str())) {
        return true;
    } 

    pFrame->res = "Error: Failed to delete file.";
    return false;
}