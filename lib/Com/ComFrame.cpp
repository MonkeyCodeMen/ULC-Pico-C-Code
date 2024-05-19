#include <Arduino.h>
#include "ComFrame.hpp"

ComFrame::ComFrame():
    module(0),index(0),command(""),cfg(0,0,0,0,""),res(""),withPar(false)
{}


ComFrame::~ComFrame()
{
   cfg.str = "";   
}

void ComFrame::reset()
{
    module = ' ';
    index  = 0;
    command == "";
    cfg = AniCfg(0,0,0,0,"");
    withPar = false;
    res ="";
}



