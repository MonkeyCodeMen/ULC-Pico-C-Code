#define JOY_1    (8+8+6+6)
#define JOY_2    (8+8+6+6+1)
#define MOUSE    (8+8+6+6+2)
#define RX       (8+8+6)
#define TX       (8+8+6+1)

#define LED_PROG_TIME           20000
#define AUTOFIRE_BLINK_TIME     200

#define LED_NO_STATE            0
#define LED_MODE_TEST           1
#define LED_MODE_GAME           2
#define LED_MODE_GAME_DARK      3
#define LED_MODE_AUTO_FIRE      5
#define LED_MOUSE_MODE          6

#define LED_MODE_RUN            100
#define LED_MODE_SIDE_BY_SIDE   101
#define LED_MODE_KNIGHT_RIDER   102
#define LED_MODE_WAVE           103

#define LED_MODE_MAX            104


/*
 *   copy from main module 
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

 */


byte _ledOff[LED_MAX] =            { OFF,OFF,OFF,OFF,  OFF,OFF,OFF,OFF,
                                     OFF,OFF,OFF,OFF,  OFF,OFF,OFF,OFF,
                                     OFF,OFF,OFF,OFF,  OFF,OFF,
                                     OFF,OFF,OFF,OFF,  OFF,OFF,
                                     OFF,OFF,OFF };


byte _gameInitLedStatus[LED_MAX] = { ON,ON,ON,ON,      ON,ON,OFF,ON,
                                     OFF,ON,OFF,OFF,   OFF,OFF,ON,ON,
                                     ON,ON,ON,ON,      ON,ON,
                                     ON,ON,ON,ON,      ON,ON,
                                     ON,ON,OFF };

byte _gameInitLedStatusDark[LED_MAX] = { OFF,OFF,OFF,OFF,    OFF,OFF,OFF,ON,
                                     OFF,ON,OFF,OFF,   OFF,OFF,ON,ON,
                                     OFF,OFF,OFF,OFF,  OFF,OFF,
                                     OFF,OFF,OFF,OFF,  OFF,OFF,
                                     ON,ON,OFF };

byte _mouseInitLedStatus[LED_MAX] = {OFF,OFF,OFF,OFF,  OFF,OFF,OFF,ON,
                                     OFF,ON,OFF,OFF,   OFF,OFF,ON,ON,
                                     OFF,OFF,OFF,ON,   ON,ON,
                                     OFF,OFF,OFF,ON,   ON,ON,
                                     OFF,OFF,ON };


/*
 * LED interface
 */


void ledSetup(void){
  if (_debugMode == MODE_TEST)
  {
    // keep serial pin clear in Test mode
    keyLedPin[RX] = 2;
    keyLedPin[TX] = 2;    
  }

  // setup ports for LED's
  for (int i=0;i<LED_MAX;i++){
      pinMode(keyLedPin[i],OUTPUT);
  }

  setAllLeds(_ledOff);
}


void ledLoop(void){
  static int lastState = LED_NO_STATE;
  int state;
  bool init=false;

  state = ledCalcState(lastState);
  if (state != lastState){
    init = true;
    if (_debugMode == MODE_TEST){
      Serial.print(" led module new state:");
      Serial.print(state,DEC);
      Serial.println("");
    }
    lastState = state;
  }
  
  switch(state){
    case LED_MODE_TEST:         ledRun(init);         break;
    case LED_MODE_GAME:         ledGame(init);        break;
    case LED_MODE_GAME_DARK:    ledGameDark(init);    break;
    case LED_MOUSE_MODE:        ledMouseMode(init);   break;
    case LED_MODE_AUTO_FIRE:    ledShowAutoFire(init); break;
    case LED_MODE_SIDE_BY_SIDE: ledSide(init);        break;
    case LED_MODE_KNIGHT_RIDER: ledKnight(init);      break;
    case LED_MODE_WAVE:         ledWave(init);        break;
    case LED_MODE_RUN:          ledRun(init);         break;
  }
}

/*
 *   application logix
 * 
 */


int ledCalcState(int lastState){
  // calc status for led module
  // priorities are defined by order of if switches
  
  static int counter=0;
  int newState = LED_NO_STATE;

  switch(_appMode){
    case MODE_ARCADE_DARK:  newState = LED_MODE_GAME_DARK;  break;
    case MODE_ARCADE:       newState = LED_MODE_GAME;       break;
    case MODE_MOUSE:        newState = LED_MOUSE_MODE;      break;
    case MODE_FIRE_CONFIG:  newState = LED_MODE_AUTO_FIRE;  break;
    default:                newState = LED_NO_STATE;        break;
  }
  if ((newState == LED_NO_STATE) && (_debugMode == MODE_TEST)){
    Serial.println(" ERROR in calculating new state");
  }


  if ((newState == MODE_ARCADE) && (_noGame==true)){
    if (lastState < LED_MODE_RUN){
      newState = LED_MODE_RUN;
      counter = 0;
    } else{
      if (counter * CYCLE_TIME > LED_PROG_TIME){
        newState = lastState+1;
        if (newState >= LED_MODE_MAX){
          newState = LED_MODE_RUN; 
        }
        counter = 0;
      } else {
        newState = lastState;
        counter++;
      }
    }
  }
  return newState;
}


void setAllLeds(byte * pData){
  for (int i=0;i<LED_MAX;i++){
    digitalWrite(keyLedPin[i],pData[i]);
  }
}

/*
 * 
 * LED programs
 * 
 */



void ledGame(bool init){
  static int counter=0;
  if (init == true){
    setAllLeds(_gameInitLedStatus);
    counter=0;
  }

  digitalWrite(keyLedPin[KEY_PLAYER_1],_onePlayer);
  digitalWrite(keyLedPin[KEY_PLAYER_2],_twoPlayer);

  for(int i=P1_BUT1;i <= P2_BUT6;i++)  // only for autofire button to avoid overwritting static leds with on
  {
    if (_autoFireBut[i] == false){
      // autofire off  ==> Led on
      digitalWrite(keyLedPin[i],ON);
    }else{
      //autofire on  ==> LED blink
      if (counter * CYCLE_TIME < AUTOFIRE_BLINK_TIME){
        digitalWrite(keyLedPin[i],ON);  
      } else if (counter * CYCLE_TIME <2* AUTOFIRE_BLINK_TIME){
        digitalWrite(keyLedPin[i],OFF);  
      } else {
        counter = 0;
      }
    }
  }
  counter++;
}


void ledGameDark(bool init){
  static int counter=0;
  if (init == true){
    setAllLeds(_gameInitLedStatusDark);
    counter=0;
  }

  digitalWrite(keyLedPin[KEY_PLAYER_1],_onePlayer);
  digitalWrite(keyLedPin[KEY_PLAYER_2],_twoPlayer);

  for(int i=P1_BUT1;i <= P2_BUT6;i++)  // only for autofire button to avoid overwritting static leds with on
  {
    if (_autoFireBut[i] == false){
      // autofire off  ==> Led on
      digitalWrite(keyLedPin[i],ON);
    }else{
      //autofire on  ==> LED blink
      if (counter * CYCLE_TIME < AUTOFIRE_BLINK_TIME){
        digitalWrite(keyLedPin[i],ON);  
      } else if (counter * CYCLE_TIME <2* AUTOFIRE_BLINK_TIME){
        digitalWrite(keyLedPin[i],OFF);  
      } else {
        counter = 0;
      }
    }
  }
  counter++;
}


void ledMouseMode(bool init){
  if (init == true){
    // switch all leds for keys on that are active in mouse mode
    setAllLeds(_mouseInitLedStatus);
  }

  // static picture  ... no LED action
}


void ledShowAutoFire(bool init){
  static int counter=0;
  int i,ii;
  if (init == true){
    // no init needed
    setAllLeds(_ledOff);
    counter=0;
    digitalWrite(keyLedPin[JOY_1],ON);
    digitalWrite(keyLedPin[JOY_2],ON);
    digitalWrite(keyLedPin[KEY_MODE],ON);
  }
  for(ii=0;ii < AUTOFIRE_SCAN_MAX-1;ii++)
  {
    i = autoFireScanOrder[ii];
    if (_autoFireBut[i] == true){
      //autofire on  ==> blink
      if (counter * CYCLE_TIME < AUTOFIRE_BLINK_TIME){
        digitalWrite(keyLedPin[i],ON);  
      } else if (counter * CYCLE_TIME <2* AUTOFIRE_BLINK_TIME){
        digitalWrite(keyLedPin[i],OFF);  
      } else {
        counter = 0;
      }
    } else {
      // autofire not on ==> LED stays on
      digitalWrite(keyLedPin[i],ON); 
    } 
  }
  counter++;
}



/*
 * 
 * LED animations
 * 
 */


void ledRun(bool init){
  static int count;
  static int led;

  if (init == true){
    count = 0;
    led = 0;
    setAllLeds(_ledOff);
    digitalWrite(keyLedPin[JOY_1],LOW);
    digitalWrite(keyLedPin[JOY_2],LOW);
  }

  count++;
  if (count > 10)
  {
    count = 0;
    digitalWrite(keyLedPin[led],HIGH);
    led++;
    led %= LED_MAX-3; // do not touch JOY1,JOY2 and mouse
    digitalWrite(keyLedPin[led],LOW);
  }
}


void setPart(int nr,int value){
  int i;
  switch(nr){
    case 0:
      for(i=0; i < 14;i++)
        digitalWrite(keyLedPin[i],value);
      break;
    
    case 1:
      for(i=14; i < 16;i++)
        digitalWrite(keyLedPin[i],value);
      break;
  
    case 2:
      for(i=16; i < 22;i++)
        digitalWrite(keyLedPin[i],value);
      break;

    case 3:
      for(i=22; i < 28;i++)
        digitalWrite(keyLedPin[i],value);
      break;
        
    default:
      if (_debugMode == MODE_TEST){
        Serial.println(" ERROR in setPart : invalid part segment given");
      }    
  }
}

void ledSide(bool init){
  static int counter;
  static int part;
  #define LED_SIDE_SWITCH_TIME  2000
  if (init == true){
    counter = 0;
    part = 0;
    setAllLeds(_ledOff);
    setPart(0,LOW); // switch first segment on
    digitalWrite(keyLedPin[JOY_1],LOW);
    digitalWrite(keyLedPin[JOY_2],LOW);
  }
  if (counter * CYCLE_TIME > LED_SIDE_SWITCH_TIME)  {
    setPart(part,HIGH); // clear led old part
    part++;
    part %= 4;
    setPart(part,LOW); // switch on new part
    counter = 0;
  } else {
    counter++;
  }
}

void setKnightCluster(int nr,int value){
  switch(nr){
    case 0: 
      digitalWrite(keyLedPin[0],value);
      break;
    case 1: 
      digitalWrite(keyLedPin[2],value);
      digitalWrite(keyLedPin[P1_BUT1],value);
      digitalWrite(keyLedPin[P1_BUT4],value);
      break;
    case 2: 
      digitalWrite(keyLedPin[3],value);
      digitalWrite(keyLedPin[P1_BUT2],value);
      digitalWrite(keyLedPin[P1_BUT5],value);
      break;
    case 3: 
      digitalWrite(keyLedPin[4],value);
      digitalWrite(keyLedPin[P1_BUT3],value);
      digitalWrite(keyLedPin[P1_BUT6],value);
      break;
    case 4: 
      digitalWrite(keyLedPin[5],value);
      digitalWrite(keyLedPin[KEY_VOLUME_DOWN],value);
      break;
    case 5: 
      digitalWrite(keyLedPin[6],value);
      digitalWrite(keyLedPin[KEY_VOLUME_UP],value);
      break;
    case 6: 
      digitalWrite(keyLedPin[7],value);
      break;
    case 7: 
      digitalWrite(keyLedPin[8],value);
      digitalWrite(keyLedPin[P2_BUT1],value);
      digitalWrite(keyLedPin[P2_BUT4],value);
      break;
    case 8: 
      digitalWrite(keyLedPin[9],value);
      digitalWrite(keyLedPin[P2_BUT2],value);
      digitalWrite(keyLedPin[P2_BUT5],value);
      break;
    case 9: 
      digitalWrite(keyLedPin[10],value);
      digitalWrite(keyLedPin[P2_BUT3],value);
      digitalWrite(keyLedPin[P2_BUT6],value);
      break;
    default:
      if (_debugMode == MODE_TEST){
          Serial.println(" ERROR in setKnightCluster : invalid part segment given");
      }    
  }
}

void ledKnight(bool init){
  static int counter;
  static int pos;
  #define LED_KNIGHT_SWITCH_TIME  500
  if (init == true){
    counter = 0;
    pos = 0;
    setAllLeds(_ledOff);
    setKnightCluster(0,LOW); // switch on first pos
    digitalWrite(keyLedPin[JOY_1],LOW);
    digitalWrite(keyLedPin[JOY_2],LOW);
  }
  if (counter * CYCLE_TIME > LED_KNIGHT_SWITCH_TIME)  {
    setKnightCluster(pos,HIGH); // clear led old pos
    pos++;
    pos %= 10;
    setKnightCluster(pos,LOW); // switch on new pos
    counter = 0;
  } else {
    counter++;
  }
}

void ledWave(bool init){
  #define LED_KNIGHT_SWITCH_TIME  500
  if (init == true){
    setAllLeds(_ledOff);
    digitalWrite(keyLedPin[JOY_1],LOW);
    digitalWrite(keyLedPin[JOY_2],LOW);
  }
}
