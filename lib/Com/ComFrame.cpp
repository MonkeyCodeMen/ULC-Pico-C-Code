#include <Arduino.h>
#include "ComFrame.hpp"

ComFrame::ComFrame():
    module(0),index(0),command(""),par1(0),par2(0),par3(0),par4(0),str(""),length(0),pData(NULL),res("")
{}

ComFrame::~ComFrame()
{
    if (pData != NULL) {
        delete pData;
        pData = NULL;
    }
    str = "";   
}

void ComFrame::reset()
{
    module = 0;
    index  = 0;
    command == "";
    par1=0;
    par2=0;
    par3=0;
    par4=0;
    if (pData != NULL) {
        delete pData;
        pData = NULL;
    }   
    length = 0;
    str="";
    withPar = false;
    res ="";
}



