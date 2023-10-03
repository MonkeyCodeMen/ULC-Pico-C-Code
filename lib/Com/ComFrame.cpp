#include <Arduino.h>
#include "ComFrame.hpp"

ComFrame::ComFrame():
    module(0),index(0),command(""),par1(""),par2(""),par3(""),par4(""),length(0),pData(NULL)
{
    _rec = "";
}

ComFrame::~ComFrame()
{
    if (pData != NULL)  delete pData;
}

void ComFrame::reset()
{
    module = 0;
    index  = 0;
    command == "";
    par1="";
    par2="";
    par3="";
    par4="";
    length=0;
    if (pData != NULL)  delete pData;
    pData = NULL;
    _rec="";
}

void ComFrame::addRec(String add){
    _rec += add;
}

char* ComFrame::print(){
    return (char*)(_rec.c_str());
}

