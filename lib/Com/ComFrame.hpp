#pragma once
#include <Arduino.h>

/*

    Start,Module,Index,Command,Par1,Par2,Par3,Par4,length,data#

    simple example
    S:L,00,On#   {start:S:,Module:switch,Index:0,Command:on,Ende:#}

    must have: start module command
    other paramtres are optional

    constraints:
    
    Start:      [S:]
                fix sequence of two chars

    Module:     [1 char]  
                L:  Led Switch
                R:  RGB Led
                S:  NeoLed Stripe
                M:  NeoLed Matrix
                ... to be continued

    Index:      [0-99 as ASCII leading 0]  
                = two char
    
    Command:    [String] max length: COM_FRAME_MAX_COMMAND_LENGTH 
                max length COM_FRAME_MAX_PAR_LENGTH
                COM_FRAME_START2,COM_FRAME_SEP,COM_FRAME_END ist not allowe as content

    Par1,Par2,Par3,Par4:    [u32_t] max length: COM_FRAME_MAX_PARAMETER_LENGTH
                            allowed content only: [ 0123456789ABCDEFabcdefxX-]


    


examples:
    S:R;00;on;0x00FF00FF#
    garbage S:R;00;on;0x0000FF00#
    S:R;00;blink;0;0x000000FF;250;250#
    S:R;00;breath;0x000000FF;10;10;0x04004000#
    S:R;00;off#
    S:R;00;blink#
    S:R;00;breath#
*/

#define COM_FRAME_MAX_COMMAND_LENGTH    20
#define COM_FRAME_MAX_PARAMETER_LENGTH  15
#define COM_FRAME_START1            'S'
#define COM_FRAME_START2            ':'
#define COM_FRAME_END               '#'
#define COM_FRAME_SEP               ';'
#define COM_FRAME_PARAMETER_SEP     ','

class ComFrame{
    public:
        ComFrame();
        ~ComFrame();
        void reset();

        char    module;
        u8_t    index;
        String  command;
        
        bool    withPar;
        u32_t   par1,par2,par3,par4;
        
        /*
            if length == 0 && pData != NULL ==> pData --> const char * Object (null terminated) 
            if length != 0 && pData != NULL ==> pData --> binary Data    of len = length
        */
        u32_t   length;         
        u8_t *  pData;
        String  str;
};

