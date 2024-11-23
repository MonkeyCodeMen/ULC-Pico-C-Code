
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


#include "Com.hpp"
#include "helper.h"
#include "Debug.hpp"

Com com;


void Com::begin(HardwareSerial * pPort, int baudRate , uint16_t config){
    ASSERT(pPort != NULL, F("invalid serial port"));
    _pPort = pPort;
    _pPort->begin(baudRate,config);
    _pPort->println(F("ULC-Pico V1.0 ready"));
    reset();
}


void Com::loop(){
    switch(_state){
        case WAIT:          doWaiting();        break;
        case START_FRAME:   getStartOfFrame();  break;
        case MODULE:        getModule();        break;
        case COMMAND:       getCommand();       break;
        case PAR1:          getPar1();          break;
        case PAR2:          getPar2();          break;
        case PAR3:          getPar3();          break;
        case PAR4:          getPar4();          break;
        case STR_START:     getStrStart();      break;
        case STR_DATA:      getStrData();       break;
        case STR_END:       getStrEnd();        break;
        case FRAME_DONE:    frameDone();        break;

        default:
            LOG(F("unknown decode state"));           
    }
}

void Com::reset(){
    _endFound = false;
    _field = "";
    _maxFieldLength = COM_FRAME_MAX_COMMAND_LENGTH;
    _dataReceived = 0;
    _frame.reset();
    _state = WAIT;
}

void Com::doWaiting(){
    uint8_t buffer;
    if (getByte(&buffer) == false)  {
        return;             
    }
    
    if (buffer == COM_FRAME_START1) {
        _state=START_FRAME; 
    } 
}

void Com::getStartOfFrame(){
    uint8_t buffer;
    if (getByte(&buffer) == false)  {
        return;            
    } 
    
    if (buffer == COM_FRAME_START2) {
        _endFound = false;
        _state = MODULE;
        return;
    } 
    reset();
}

void Com::getModule(){
    uint8_t buffer[3];
    if (getBytes(3,buffer)== false)   {
        return;
    }
    if (buffer[2] != COM_FRAME_SEP)   {
        reset(); 
        return;   
    }      
    _frame.module = buffer[0];
    _frame.index =  convertDezCharToInt(buffer[1]);
    _state = COMMAND;
    _field = "";
    _maxFieldLength = COM_FRAME_MAX_COMMAND_LENGTH;
}


void Com::getCommand(){
    if (collectField() == false)    {
        return;            
    }
    _frame.command = _field;
    if (_endFound == true){
        _state = FRAME_DONE;
    } else {
        _state = PAR1;
        _frame.withPar=true;
        _field = "";
        _maxFieldLength = COM_FRAME_MAX_PARAMETER_LENGTH;
    }
}


void Com::getPar1(){
    if (collectField() == false)    {
        return;            
    }
    _frame.cfg.dimCfg.uint32 = convertStrToInt(_field);
    if (_endFound == true){
        _state = FRAME_DONE;
    } else {
        _state = PAR2;
        _field = "";
        _maxFieldLength = COM_FRAME_MAX_PARAMETER_LENGTH;
    }
}

void Com::getPar2(){
    if (collectField() == false)    {
        return;            
    }
    _frame.cfg.colorCfg.uint32 = convertStrToInt(_field);
    if (_endFound == true){
        _state = FRAME_DONE;
    } else {
        _state = PAR3;
        _field = "";
        _maxFieldLength = COM_FRAME_MAX_PARAMETER_LENGTH;
    }
}

void Com::getPar3(){
    if (collectField() == false)    {
        return;            
    }
    _frame.cfg.flashCfg.uint32 = convertStrToInt(_field);
    if (_endFound == true){
        _state = FRAME_DONE;
    } else {
        _state = PAR4;
        _field = "";
        _maxFieldLength = COM_FRAME_MAX_PARAMETER_LENGTH;
    }
}

void Com::getPar4(){
    if (collectField() == false)    {
        return;            
    }
    _frame.cfg.breathCfg.uint32 = convertStrToInt(_field);
    _field = "";
    if (_endFound == true){
        _state = FRAME_DONE;
    } else {
        _state = STR_START;
        _field = "";
    }
}

void Com::getStrStart(){
    uint8_t buffer;
    if (getByte(&buffer) == false)  {
        return;            
    } 

    if (buffer == COM_FRAME_TEXT_QUOTES){
        _state = STR_DATA;
        _frame.cfg.str = "";
    } else if (buffer == COM_FRAME_END) {
        _state = FRAME_DONE;
    } else {
        reset();       
    }
}

void Com::getStrData(){
    uint8_t buffer;
    if (getByte(&buffer) == false)  {
        return;            
    } 

    if (buffer == COM_FRAME_TEXT_QUOTES){
        _state = STR_END;
    } else {
        _frame.cfg.str+=(char)buffer;
        if (_frame.cfg.str.length() > COM_FRAME_MAX_STR_LENGTH){
            reset();       
        }
    }
}

void Com::getStrEnd(){
    uint8_t buffer;
    if (getByte(&buffer) == false)  {
        return;            
    } 

    if (buffer == COM_FRAME_END) {
        _state = FRAME_DONE;
    } else {
        reset();       
    }
}


void Com::frameDone(){
    // frame ready for further processing
    bool res = _dispatcher.dispatchFrame(&_frame);
    sendAnswer(res,&_frame);

    // frame processed delete all data now
    reset();  // will free pBuffer if not taken over from application (_frame.pBuffer=NULL)
}

void Com::sendAnswer(bool res,ComFrame * pFrame){
    String out;

    out = COM_FRAME_ANSWER_START;
    out += pFrame->module;
    out += String(pFrame->index);
    out += COM_FRAME_SEP;
    out += pFrame->command;
    if (pFrame->withPar == true){
        out += COM_FRAME_SEP;
        out += String(pFrame->cfg.dimCfg.uint32,HEX);
        out += COM_FRAME_SEP;
        out += String(pFrame->cfg.colorCfg.uint32,HEX);
        out += COM_FRAME_SEP;
        out += String(pFrame->cfg.flashCfg.uint32,HEX);
        out += COM_FRAME_SEP;
        out += String(pFrame->cfg.breathCfg.uint32,HEX);
        out += COM_FRAME_SEP;
        out += COM_FRAME_TEXT_QUOTES;
        out += String(pFrame->cfg.str);
        out += COM_FRAME_TEXT_QUOTES;
    }
    if (res == true){
        out+=COM_FRAME_END;
        out+="OK-";
        out+=pFrame->res;
        out+=COM_FRAME_END;
    } else {
        out+=COM_FRAME_END;
        out+="NOK-";
        out+=pFrame->res;
        out+=COM_FRAME_END;
    }
    _pPort->print(out);
}



bool Com::getByte(uint8_t * pBuffer){
    uint8_t value;
    if (_pPort->available() >= 1) {
        value = _pPort->read();
        *pBuffer= value;
        return true;
    }
    return false;    
}

bool Com::getBytes(uint32_t count,uint8_t * pBuffer){
    uint8_t value;
    if (_pPort->available() >= count) {
        for(int i=0;i < count; i++){
            value = _pPort->read();
            pBuffer[i] = value;
        }
        return true;
    }
    return false;    
}

bool Com::collectField(){
    uint8_t __byte;
    if (getByte(&__byte)== false) { return false;}

    if (__byte == COM_FRAME_END){
        _endFound = true;
        return true;
    }

    if (__byte == COM_FRAME_SEP){
        _endFound=false;
        return true;
    }

    _field += (char)__byte;
    if (_field.length() >= _maxFieldLength) {reset();  return false;}

    return false;
}