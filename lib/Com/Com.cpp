#include "Com.hpp"
#include "helper.hpp"
#include "Debug.hpp"

Com::Com()
{
    _pPort = &Serial1;
}

void Com::setup(){
    _pPort->begin(115200);
    _state = WAIT;
}


void Com::loop(){
    switch(_state){
        case WAIT:          doWaiting();        break;
        case START_FRAME:   getStartOfFrame();  break;
        case MODULE:        getModule();        break;
        case INDEX:         getIndex();         break;
        case COMMAND:       getCommand();       break;
        case PAR1:          getPar1();          break;
        case PAR2:          getPar2();          break;
        case PAR3:          getPar3();          break;
        case PAR4:          getPar4();          break;
        case LENGTH:        getLength();        break;
        case DATA:          getData();          break;
        case END_FRAME:     getEndOfFrame();    break;
        case FRAME_DONE:    frameDone();        break;

        default:
            LOG(F("unknown decode state"));           
    }
}

void Com::reset(){
    LOG(F("COM:Reset "));
    _endFound = false;
    _field = "";
    _maxFieldLength = COM_FRAME_MAX_COMMAND_LENGTH;
    _frame.reset();
    _state = WAIT;
}

void Com::doWaiting(){
    u8_t buffer;
    if (getByte(&buffer) == false)  {
        return;             
    }
    
    if (buffer == COM_FRAME_START1) {
        _state=START_FRAME; 
    } 
}

void Com::getStartOfFrame(){
    u8_t buffer;
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
    u8_t buffer[2];
    if (getBytes(2,buffer)== false)   {
        return;
    }
    if (buffer[1] != COM_FRAME_SEP)   {
        reset(); 
        return;   
    }      
    _frame.module = buffer[0];
    _state = INDEX;
}

void Com::getIndex(){
    u8_t buffer[3];
    if (getBytes(3,buffer)== false)   {
        return;
    }
    if (buffer[2] != COM_FRAME_SEP)   {
        reset(); 
        return;   
    }

    buffer[2]=0;
    _frame.index = String((char*)buffer).toInt();
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
    _frame.par1 = convertStrToInt(_field);
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
    _frame.par2 = convertStrToInt(_field);
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
    _frame.par3 = convertStrToInt(_field);
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
    _frame.par4 = convertStrToInt(_field);
    _field = "";
    if (_endFound == true){
        _state = FRAME_DONE;
    } else {
        _state = DATA;
        _frame.length=0;
        _field = "";
        _frame.pData = (u8_t *) new String();
    }
}

void Com::getLength(){
    if (collectField() == false)    {
        return;            
    }
    _frame.length = convertStrToInt(_field);
    if (_endFound == true){
        reset();  // length without data???
    } else {
        _state = DATA;
    }
}

void Com::getData(){
    u8_t buffer;
    if (getByte(&buffer) == false)  {
        return;            
    }

    if (buffer == 0){
        // end of pData (ASCII mode string) reached
        _frame.pData = (u8_t *)_frame.str.c_str();
        _state = END_FRAME;
        return;
    }

    // end not reached at char to pData (String)
    _frame.str.concat((char)buffer);
}


void Com::getEndOfFrame(){
    u8_t buffer;
    if (getByte(&buffer) == false)  {
        return;            
    }

    if (buffer != COM_FRAME_END){
        LOG(F("getEndOfFrame wrong end frame signature   .. frame skipped"));
        reset();
        return;
    }

    _state = FRAME_DONE;
    _endFound = true;
    _frame.length = 0;  // it is a string
}


void Com::frameDone(){
    LOG(F("COM:Dispatch Frame:"));
    // frame ready for further processing
    _dispatcher.dispatchFrame(&_frame);
    // frame processed delete all data now
    reset();
}





bool Com::getByte(u8_t * pBuffer){
    u8_t value;
    if (_pPort->available() >= 1) {
        value = _pPort->read();
        *pBuffer= value;
        return true;
    }
    return false;    
}

bool Com::getBytes(u32_t count,u8_t * pBuffer){
    u8_t value;
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
    u8_t __byte;
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