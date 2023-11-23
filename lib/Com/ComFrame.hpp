#pragma once
#include <Arduino.h>

/*

    Start,Module,Index,Command,Par1,Par2,Par3,Par4,length,data#

    simple example
    S:L0;On#   {start:"S:""  Module:"S" Index:"0" ; Command:"on" end:"#""}

    must have: start module command
    other paramtres are optional

    constraints:
    
    Start:      [S:]
                fix sequence of two chars

    Module:     [2 char]  
                LX:  Led Switch
                RX:  RGB Led
                SX:  NeoLed Stripe
                MX:  NeoLed Matrix
                ... to be continued
                X:   0..9 index for module
    
    Command:    [String] max length: COM_FRAME_MAX_COMMAND_LENGTH 
                max length COM_FRAME_MAX_PAR_LENGTH
                COM_FRAME_START2,COM_FRAME_SEP,COM_FRAME_END ist not allowe as content

    Par1,Par2,Par3,Par4:    
                [u32_t as string] max length: COM_FRAME_MAX_PARAMETER_LENGTH
                allowed content only: [ 0123456789ABCDEFabcdefxX-]
    
    str:        [string] 
                text parameter with starting and ending COM_FRAME_TEXT_QUOTES  ==>  COM_FRAME_TEXT_QUOTES are not allowed inside of text

    length,data#  
                length [u32_t as string]
                data   [u8_t binary]   length bytes of data

examples:

-- LED switch ---
S:L0;on#
S:L0;dim;0xFF#
S:L0;dim;10#
S:L0;blink#
S:L0;multi flash;0xFF;0;0x00200060;0x000201F4#

-- RGB LEDS'S ---
S:R0;on;0x00FF00FF#
garbage S:R0;on;0x0000FF00#
S:R0;blink;0;0x000000FF;250;250#
S:R0;breath;0x000000FF;10;10;0x04004000#
S:R0;off#
S:R0;blink#
S:R0;breath#
S:R0;on;0xFFFFFF#
S:R0;rainbow;255;1;50#
S:R0;multi flash#

-- Neo LED stripes
S:S0;Static;0x00FFFFFF;0xFF;1000#
S:S0;Static;0x00FF0000;0xFF;1000#
S:S0;Static;0x0000FF00;0xFF;1000#
S:S0;Static;0x000000FF;0xFF;1000#
S:S0;Static;0x000000FF;0x10;1000#
S:S0;Off#
S:S0;Rainbow Cycle#

"Off";"Static";"Blink";"Breath";"Color Wipe";"Color Wipe Inverse";"Color Wipe Reverse";"Color Wipe Reverse Inverse";"Color Wipe Random";"Random Color";"Single Dynamic";
"Multi Dynamic";"Rainbow";"Rainbow Cycle";"Scan";"Dual Scan";"Fade";"Theater Chase";"Theater Chase Rainbow";"Running Lights";"Twinkle";"Twinkle Random";"Twinkle Fade";
"Twinkle Fade Random";"Sparkle";"Flash Sparkle";"Hyper Sparkle";"Strobe";"Strobe Rainbow";"Multi Strobe";"Blink Rainbow";"Chase White";"Chase Color";"Chase Random";
"Chase Rainbow";"Chase Flash";"Chase Flash Random";"Chase Rainbow White";"Chase Blackout";"Chase Blackout Rainbow";"Color Sweep Random";"Running Color";"Running Red Blue";
"Running Random";"Larson Scanner";"Comet";"Fireworks";"Fireworks Random";"Merry Christmas";"Fire Flicker";"Fire Flicker (soft)";"Fire Flicker (intense)";"Circus Combustus";
"Halloween";"Bicolor Chase";"Tricolor Chase";"TwinkleFOX";"Rain";
"Custom 0";"Custom 1";"Custom 2";"Custom 3";"Custom 4";"Custom 5";"Custom 6";"Custom 7";

- NEO LED Matrix
S:M0;breath#
S:M0;static;0xFF0000FF#
S:M0;rect#
S:M0;circle#
S:M0;gif file#

*/

#define COM_FRAME_MAX_COMMAND_LENGTH    50
#define COM_FRAME_MAX_PARAMETER_LENGTH  30
#define COM_FRAME_MAX_STR_LENGTH        100
#define COM_FRAME_START1            'S'
#define COM_FRAME_START2            ':'
#define COM_FRAME_END               '#'
#define COM_FRAME_SEP               ';'
#define COM_FRAME_PARAMETER_SEP     ','
#define COM_FRAME_TEXT_QUOTES       '"'
#define COM_FRAME_ANSWER_START      "A:"

class ComFrame{
    public:
        ComFrame();
        ~ComFrame();
        void reset();

        char    module;
        u8_t    index;
        String  command;

        // parameter       
        bool    withPar;                
        u32_t   par1,par2,par3,par4;    // u32_t
        String  str;                    // string 
        u32_t   length;                 // binary length
        u8_t *  pData;                  // binary data

        // result
        String res;
};