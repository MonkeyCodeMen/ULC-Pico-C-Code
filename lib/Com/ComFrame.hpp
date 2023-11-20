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

-- LED switch ---
S:L;00;on#
S:L;00;dim;0xFF#
S:L;00;dim;10#
S:L;00;blink#
S:L;00;multi flash;0xFF;0;0x00200060;0x000201F4#

-- RGB LEDS'S ---
S:R;00;on;0x00FF00FF#
garbage S:R;00;on;0x0000FF00#
S:R;00;blink;0;0x000000FF;250;250#
S:R;00;breath;0x000000FF;10;10;0x04004000#
S:R;00;off#
S:R;00;blink#
S:R;00;breath#
S:R;00;on;0xFFFFFF#
S:R;00;rainbow;255;1;50#
S:R;00;multi flash#

-- Neo LED stripes
S:S;00;Static;0x00FFFFFF;0xFF;1000#
S:S;00;Static;0x00FF0000;0xFF;1000#
S:S;00;Static;0x0000FF00;0xFF;1000#
S:S;00;Static;0x000000FF;0xFF;1000#
S:S;00;Static;0x000000FF;0x10;1000#
S:S;00;Off#
S:S;00;Rainbow Cycle#

"Off";"Static";"Blink";"Breath";"Color Wipe";"Color Wipe Inverse";"Color Wipe Reverse";"Color Wipe Reverse Inverse";"Color Wipe Random";"Random Color";"Single Dynamic";
"Multi Dynamic";"Rainbow";"Rainbow Cycle";"Scan";"Dual Scan";"Fade";"Theater Chase";"Theater Chase Rainbow";"Running Lights";"Twinkle";"Twinkle Random";"Twinkle Fade";
"Twinkle Fade Random";"Sparkle";"Flash Sparkle";"Hyper Sparkle";"Strobe";"Strobe Rainbow";"Multi Strobe";"Blink Rainbow";"Chase White";"Chase Color";"Chase Random";
"Chase Rainbow";"Chase Flash";"Chase Flash Random";"Chase Rainbow White";"Chase Blackout";"Chase Blackout Rainbow";"Color Sweep Random";"Running Color";"Running Red Blue";
"Running Random";"Larson Scanner";"Comet";"Fireworks";"Fireworks Random";"Merry Christmas";"Fire Flicker";"Fire Flicker (soft)";"Fire Flicker (intense)";"Circus Combustus";
"Halloween";"Bicolor Chase";"Tricolor Chase";"TwinkleFOX";"Rain";
"Custom 0";"Custom 1";"Custom 2";"Custom 3";"Custom 4";"Custom 5";"Custom 6";"Custom 7";

- NEO LED Matrix
S:M;00;breath#
S:M;00;static;0xFF0000FF#
S:M;00;running rect#
S:M;00;gif file#



*/

#define COM_FRAME_MAX_COMMAND_LENGTH    50
#define COM_FRAME_MAX_PARAMETER_LENGTH  30
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

