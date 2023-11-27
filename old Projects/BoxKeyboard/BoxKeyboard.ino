/*
 * Main sketch for Ardunio Due  used as "keyboard" for my arcade machine
 * consist of two joysticks with 6 fire buttons each , 
 * several addtional keys, Trackball as mouse and
 * LEDS for all push buttons / keys / joystick and trackball
 * 
 * main modes: 
 *   + keyboard mode (with autofire functionality 
 *   + autofire configuration 
 *   + mouse mode
 * several light game modes 
 * 
 * 4 module structure:
 * + main (this)
 * + key      handle button debouncing / evalutaion and autofire config 
 *            att key modules althoug hanldes mouse buttons !!!
 * + led      light shows 
 * + mouse    convert trackball logic to mouse signals
 *            only movement, buttons handled  by key module
 * 
 * 
 */

#define CYCLE_TIME  5

#define MODE_PIN   70
#define MODE_RUN   0
#define MODE_TEST  1
#include <Keyboard.h>
#include <Mouse.h>
#include <FastLED.h>  // for slicer only



// define buttons 
#define KEY_PLAYER_1    0
#define KEY_PLAYER_2    1
#define KEY_SAVE        2
#define KEY_LOAD        3
#define KEY_SELECT_1    4
#define KEY_SELECT_2    5
#define KEY_SELECT_3    6
#define KEY_KILL        7
#define KEY_SELECT_4    8
#define KEY_MODE        9
#define KEY_VOLUME_UP   14
#define KEY_VOLUME_DOWN 15
#define P1_BUT1         16
#define P1_BUT2         17
#define P1_BUT3         18
#define P1_BUT4         19
#define P1_BUT5         20
#define P1_BUT6         21
#define P2_BUT1         22
#define P2_BUT2         23
#define P2_BUT3         24
#define P2_BUT4         25
#define P2_BUT5         26
#define P2_BUT6         27
#define J1_UP           28
#define J1_DOWN         29
#define J1_RIGHT        30
#define J1_LEFT         31
#define J2_UP           32
#define J2_DOWN         33
#define J2_RIGHT        34
#define J2_LEFT         35

// key pin mapping
#define KEY_MAX         (8+8+6+6+8)
int  keyPin[KEY_MAX] =  {  54, 55, 56, 57,  58, 59, 60, 61,                                                                         // function keys A
                           62, 63, 64, 65,  66, 67, 68, 69,                                                                         // function keys B
                           22, 24, 26, 28,  30, 32,                                                                                 // fire button joystick 1
                           23, 25, 27, 29,  31, 33,                                                                                 // fire button joystick 2
                           14, 15, 16, 17,  18, 19, 20, 21                                                                          // direction joystick 1 and joystick 2
};

// key code table (arcade modes)
int  keyCode[KEY_MAX] = { '1','2',KEY_F5,KEY_F7,               '3','4','5',KEY_KILL,                                                // function keys A 
                          '6',KEY_MODE,'7','8',                '9','0',KEY_VOLUME_UP,KEY_VOLUME_DOWN,                               // function keys B
                          'y','x','q','b',                     'a','w',                                                             // fire button joystick 1
                          'z','u','i','h',                     'j','k',                                                             // fire button joystick 2
                          KEY_UP_ARROW,KEY_DOWN_ARROW,KEY_RIGHT_ARROW,KEY_LEFT_ARROW,                                               // direction joystick 1
                          'r','f','g','d'                                                                                           // direction joystick 2
};
// key debug texts
String keyCodeText[KEY_MAX] = { "one Player 1","two Player 2","save F5","Load F7",    "sel1","sel2","sel3","kill(ALT+F4)",          // function keys A
                                "sel4","Mode","sel5","sel6",                          "sel7","sel8","Volume up","Volume Down",      // function keys B
                                "J1B1","J1B2","J1B3","J1B4",                          "J1B5","J1B6",                                // fire button joystick 1
                                "J2B1","J2B2","J2B3","J2B4",                          "J2B5","J2B6",                                // fire button joystick 2
                                "Up","Down","Right","Left",                           "Pad-Up","Pad-Down","Pad-Right","Pad-Left"    // direction joystick 1 and joystick 2
};

// LED assignment (same order like keys !!)
#define LED_MAX  (8+8+6+6+3)
int keyLedPin[LED_MAX]   ={  52, 50, 48, 46,  44, 42, 40, 38,                                                                       // function keys A
                             53, 51, 49, 47,  45, 43, 41, 39,                                                                       // function keys B
                              8,  9, 10, 11,  12, 13,                                                                               // fire button joystick 1
                              0,  1,  2,  3,   4,  5,                                                                               // fire button joystick 2   // save rs232 for Debugging
                             36, 37, 35                                                                                             // Joystick 1, joystick 2 , Mouse/Trackball
                              };          



// defines for app mode
#define MODE_INIT           0
#define MODE_ARCADE         1
#define MODE_MOUSE          2
#define MODE_FIRE_CONFIG    3
#define MODE_ARCADE_DARK    4
#define MODE_MAX            5

// negative Logic for LEDs
#define ON  LOW
#define OFF HIGH



// status Flags
volatile int  _debugMode;
volatile int  _appMode;
volatile int  _ledMode;
//volatile bool _mouseMode;              // global flag for general Status Mouse Mode
//volatile bool _autoFireConfigMode;     // global flag for autofire config mode
volatile bool _autoFireBut[KEY_MAX];   // global array for buttons for autofire or not  
volatile bool _noGame;
volatile byte _onePlayer;
volatile byte _twoPlayer;

// prototypes (interfaces to other modules beside the global variables)
void ledInit(void);
void ledToggle(void);
void mouseInit(void);
void mouseScan(void);
void keyInit(void);
void keyScan(void);



void setup(){
    int i;
    Serial.begin(115200);
    Serial.println("Init begin");
    _appMode = MODE_INIT;

    Serial.println("init pins");
    pinMode(MODE_PIN,INPUT_PULLUP);
    _debugMode = digitalRead(MODE_PIN);
    //_debugMode = HIGH;


    // clear all autofire selections
    for(i=0;i<KEY_MAX;i++){
      _autoFireBut[i]=false;
    }

    Serial.println("init keys LED");
    ledSetup();

    Serial.println("init keyboard");
    keySetup();
  
    Serial.println("init mouse");
    mouseSetup();
  
    Serial.println("init done");
    Serial.flush();

    // set standard mode
    _noGame = true;
    _onePlayer = ON;
    _twoPlayer = ON;
    _appMode = MODE_ARCADE;

}

/*
 * get a reading from the mouse and report it back to the
 * host via the serial line.
 */
void loop(){
  EVERY_N_MILLISECONDS(CYCLE_TIME)
  {
    ledLoop();
    keyLoop();
    mouseLoop();  
  }
}
 
