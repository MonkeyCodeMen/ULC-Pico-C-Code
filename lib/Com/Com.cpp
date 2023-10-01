#include "Com.hpp"
#include "../Debug/Debug.hpp"

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
            LOG("unknown decode state");           
    }
}

void Com::reset(){
    _endFound = false;
    _field = "";
    _frame.reset();
    _state = WAIT;
}

void Com::doWaiting(){
    if (getByte(&_byte) == false)  {return;             } 
    if (_byte == COM_FRAME_START1) {_state=START_FRAME; } 
}

void Com::getStartOfFrame(){
    if (getByte(&_byte) == false)  {return;             } 
    if (_byte == COM_FRAME_START2) {
        _endFound = false;
        _field = "";
        _state = MODULE;
    } else {
        _state = WAIT;
    }
}

void Com::getModule(){
    char __buffer[2];
    if (getBytes(2,(u8_t*)&__buffer)== false)   {return;}
    if (__buffer[1] != COM_FRAME_SEP)           {reset(); return;   }      
    _frame.module = __buffer[0];
    _state = INDEX;
}

void Com::getIndex(){
    char __buffer[2];
    if (getBytes(3,(u8_t*)&__buffer)== false)   {return;}
    if (__buffer[2] != COM_FRAME_SEP)           {reset(); return;   }
    __buffer[2]=0;
    _frame.index = String(__buffer).toInt();
    _state = COMMAND;
}



void Com::getCommand(){
    if (collectField() == false)    {return;            }
    _frame.command = _field;
    if (_endFound == true){
        _state = FRAME_DONE;
    } else {
        _state = PAR1;
    }
}


void Com::getPar1(){
    if (collectField() == false)    {return;            }
    _frame.par1 = _field;
    if (_endFound == true){
        _state = FRAME_DONE;
    } else {
        _state = PAR2;
    }
}

void Com::getPar2(){
    if (collectField() == false)    {return;            }
    _frame.par2 = _field;
    if (_endFound == true){
        _state = FRAME_DONE;
    } else {
        _state = PAR3;
    }
}

void Com::getPar3(){
    if (collectField() == false)    {return;            }
    _frame.par3 = _field;
    if (_endFound == true){
        _state = FRAME_DONE;
    } else {
        _state = PAR4;
    }
}

void Com::getPar4(){
    if (collectField() == false)    {return;            }
    _frame.par4 = _field;
    if (_endFound == true){
        _state = FRAME_DONE;
    } else {
        _state = LENGTH;
    }
}

void Com::getLength(){
    if (collectField() == false)    {return;            }
    _frame.length = _field.toInt();
    if (_endFound == true){
        reset();  // length without data???
    } else {
        _state = DATA;
    }
}

void Com::getData(){
    LOG("getData not implemented until now !!!");
    reset();
}


void Com::getEndOfFrame(){
    LOG("getEndOfFrame not implemented until now !!!");
    reset();
}


void Com::frameDone(){
    // frame ready for further processing
    _dispatcher.dispatchFrame(&_frame);
    // frame processed delete all data now
    reset();
}





bool Com::getByte(u8_t * pBuffer){
    if (_pPort->available() >= 1) {
        *pBuffer= _pPort->read();
        return true;
    }
    return false;    
}

bool Com::getBytes(u32_t count,u8_t * pBuffer){
    if (_pPort->available() >= count) {
        for(int i;i < count; i++){
            *pBuffer= _pPort->read();
            pBuffer++; 
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

    _field += __byte;
    if (_field.length() >= COM_FRAME_MAX_PAR_LENGTH) {reset();  return false;}

    return false;
}