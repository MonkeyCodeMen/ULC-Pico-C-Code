
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


#pragma once
#include <Arduino.h>
#include <Ani.hpp>

/*
    for details see: README

    Start,ModuleIndex,Command,Par1,Par2,Par3,Par4,str,length,data#

    simple example
    S:L0,On#   {start:"S:""  Module:"S" Index:"0" ; Command:"on" end:"#""}

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
                [uint32_t as string] max length: COM_FRAME_MAX_PARAMETER_LENGTH
                allowed content only: [ 0123456789ABCDEFabcdefxX-]
    
    str:        [string] 
                text parameter with starting and ending COM_FRAME_TEXT_QUOTES  ==>  COM_FRAME_TEXT_QUOTES are not allowed inside of text

    #           end of frame

*/

#define COM_FRAME_MAX_COMMAND_LENGTH    50
#define COM_FRAME_MAX_PARAMETER_LENGTH  30
#define COM_FRAME_MAX_STR_LENGTH        250
#define COM_FRAME_START1            'S'
#define COM_FRAME_START2            ':'
#define COM_FRAME_END               '#'
#define COM_FRAME_SEP               ','
#define COM_FRAME_PARAMETER_SEP     ';'
#define COM_FRAME_TEXT_QUOTES       '"'
#define COM_FRAME_ANSWER_START      "A:"

class ComFrame{
    public:
        ComFrame(): module(0),index(0),command(""),cfg(0,0,0,0,""),res(""),withPar(false)  {}
        ~ComFrame() {cfg.str = "";   }
        void reset(){
            module = ' ';
            index  = 0;
            command == "";
            cfg = AniCfg(0,0,0,0,"");
            withPar = false;
            res ="";
        }



        char    module;
        uint8_t index;
        String  command;

        // parameter       
        bool        withPar;                
        AniCfg      cfg;

        // result
        String res;
};