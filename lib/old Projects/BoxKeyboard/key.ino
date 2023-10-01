/*
 * press an release will be evaluated individaully and send to host
 * a debounce functionality can be added if needed by uncomment teh defien DEBOUCNE
 * 
 * there are two fields to control the scan process
 * scanOrder:   number and order of keys for scanning
 * scanReport:  a boolean field to control if result of scann process is been sent to host as key-code 
 * 
 */

// For direct use of USB HID (yes, 3 is left out)
const u8_t kHID_ReportID_Mouse           =  1;
const u8_t kHID_ReportID_Keyboard        =  2;
const u8_t kHID_ReportID_ConsumerControl =  4;
const u8_t kHID_ReportID_SystemControl   =  5;
const u8_t kHID_ReportID_Gamepad         =  6;
const u8_t kHID_ReportID_MouseAbsolute   =  7;
const u8_t kHID_ReportID_NKRO_keyboard   =  8;
const u8_t kHID_ReportID_TeensyKeyboard  =  9;
const u8_t kHID_ReportID_SurfaceDial     = 10;

// For use in "Consumer Control". They are bit coded.
const u8_t kVolumeUp   = 1 << 0; // Bit 0
const u8_t kVolumeDown = 1 << 1; // Bit 1
const u8_t kVolumeMute = 1 << 2; // Bit 2


static const u8_t HID_ReportDescriptor_ConsumerControl[] PROGMEM =
{
    0x05, 0x0c,                           // Usage Page ("Consumer Devices")
    0x09, 0x01,                           // Usage (Consumer Control)
    0xa1, 0x01,                           // Collection (Application)
    0x85, kHID_ReportID_ConsumerControl,  // "Report ID"
    0x15, 0x00,                           // Logical Minimum (0)
    0x25, 0x01,                           // Logical Maximum (1)
    0x09, 0xe9,                           // Usage (Volume Up)
    0x09, 0xea,                           // Usage (Volume Down)
    0x75, 0x01,                           // Report Size (1)
    0x95, 0x02,                           // Report Count (2)
    0x81, 0x06,                           // Input (Data, Variable, Relative)

    0x09, 0xe2,                           // Usage (Mute)
    0x95, 0x01,                           // Report Count (1)
    0x81, 0x06,                           // Input (Data, Variable, Relative)

    0x09, 0xb0,                           // Usage (Play)
    0x95, 0x01,                           // Report Count (1)
    0x81, 0x06,                           // Input (Data, Variable, Relative)

    0x09, 0xb1,                           // Usage (Pause)
    0x95, 0x01,                           // Report Count (1)
    0x81, 0x06,                           // Input (Data, Variable, Relative)

    0x09, 0xb7,                           // Usage (Stop)
    0x95, 0x01,                           // Report Count (1)
    0x81, 0x06,                           // Input (Data, Variable, Relative)

    0x09, 0xb5,                           // Usage (Next)
    0x95, 0x01,                           // Report Count (1)
    0x81, 0x06,                           // Input (Data, Variable, Relative)

    0x09, 0xb6,                           // Usage (Previous)
    0x95, 0x01,                           // Report Count (1)
    0x81, 0x06,                           // Input (Data, Variable, Relative)

    0x09, 0xb3,                           // Usage (Fast Forward)
    0x95, 0x01,                           // Report Count (1)
    0x81, 0x06,                           // Input (Data, Variable, Relative)

    0x09, 0xb4,                           // Usage (Rewind)
    0x95, 0x01,                           // Report Count (1)
    0x81, 0x06,                           // Input (Data, Variable, Relative)

    0x95, 0x06,                           // Report Count (6) Number of bits remaining in byte
    0x81, 0x07,                           // Input (Constant, Variable, Relative)
    0xc0                                  // End Collection
}; // Table for USB HID "Consumer Control" (for audio volume control, etc.)

static HIDSubDescriptor node(HID_ReportDescriptor_ConsumerControl,
                               sizeof(HID_ReportDescriptor_ConsumerControl));



//#define DEBOUNCE         1
#define KEY_TRESHOLD_ON  2
#define KEY_TRESHOLD_OFF 3

bool keyStatus[KEY_MAX];
int  keyCounter[KEY_MAX];
int  gameTimeoutCounter;
int  autoFireCounter=0;

#define GAME_TIMEOUT          ((1000*60*15)/CYCLE_TIME)  // [cycle_time]
#define AUTO_FIRE_CYCLE_TIME  (15/CYCLE_TIME)            // real CYCLE is time[ms] * 2 (low time+high time) [cycle_time]

// definitions for mouse mode
#define MOUSE_SCAN_MAX (6+6)
byte mouseScanOrder[MOUSE_SCAN_MAX] = {         P1_BUT4,P1_BUT5,P1_BUT6, P2_BUT4,P2_BUT5,P2_BUT6,
                                                KEY_SAVE,KEY_LOAD,
                                                KEY_KILL,KEY_VOLUME_UP,KEY_VOLUME_DOWN,KEY_MODE};
bool mouseScanReport[MOUSE_SCAN_MAX] = {        false,false,false, false,false,false,
                                                true,true,
                                                false,false,false,false};

// defintions for autofire configuration mode
#define AUTOFIRE_SCAN_MAX (6+6+1)
byte autoFireScanOrder[AUTOFIRE_SCAN_MAX] = {   P1_BUT1,P1_BUT2,P1_BUT3, P1_BUT4,P1_BUT5,P1_BUT6,
                                                P2_BUT1,P2_BUT2,P2_BUT3, P2_BUT4,P2_BUT5,P2_BUT6, 
                                                KEY_MODE};
bool autoFireScanReport[AUTOFIRE_SCAN_MAX] = {  false,false,false, false,false,false,
                                                false,false,false, false,false,false, 
                                                false};

#define ARCADE_SCAN_MAX (8+4+6+6+8)    // scan all keys 
byte arcadeScanOrder[ARCADE_SCAN_MAX] =  {      KEY_PLAYER_1,KEY_PLAYER_2,KEY_SAVE,KEY_LOAD,          KEY_SELECT_1,KEY_SELECT_2,KEY_SELECT_3,KEY_KILL,
                                                KEY_SELECT_4,KEY_MODE,KEY_VOLUME_UP,KEY_VOLUME_DOWN,
                                                P1_BUT1,P1_BUT2,P1_BUT3,P1_BUT4,                      P1_BUT5,P1_BUT6,
                                                P2_BUT1,P2_BUT2,P2_BUT3,P2_BUT4,                      P2_BUT5,P2_BUT6,
                                                J1_UP,J1_DOWN,J1_RIGHT,J1_LEFT,                       J2_UP,J2_DOWN,J2_RIGHT,J2_LEFT};
bool arcadeScanReport[ARCADE_SCAN_MAX] = {      true,true,true,true,                                  true,true,true,false,
                                                true,false,false,false,
                                                true,true,true,true,                                  true,true,
                                                true,true,true,true,                                  true,true,
                                                true,true,true,true,                                  true,true,true,true};



/*
 * 
 * key interface
 * 
 */
                                              
void keySetup(void){
  Serial.println("Keyboard pin init");
  for(int i=0;i<KEY_MAX;i++){
      pinMode(keyPin[i],INPUT_PULLUP);
  }
  resetKeyStates();

  Serial.println("USB HID Keyboard driver init");
  Keyboard.begin();

  
  Serial.println("USB HID consumer driver init");
  HID().AppendDescriptor(&node);
}


void keyLoop(void){
  // scan all active keys (depends on mode) although in mouse because buttons are reused
  byte * pOrder;
  bool * pReport;
  int maxKey;
  byte currentKey;
  bool currentReport;
  int  currentState;


  // select scan and report line for current mode
  switch(_appMode){
    case MODE_ARCADE_DARK:
    case MODE_ARCADE:       
      pOrder   = arcadeScanOrder;
      pReport  = arcadeScanReport;
      maxKey   = ARCADE_SCAN_MAX;
      break;
        
    case MODE_MOUSE:        
      pOrder   = mouseScanOrder;
      pReport  = mouseScanReport;
      maxKey   = MOUSE_SCAN_MAX;
      break;

    case MODE_FIRE_CONFIG:  
      pOrder   = autoFireScanOrder;
      pReport  = autoFireScanReport;
      maxKey   = ARCADE_SCAN_MAX;
      break;
    
    default: 
      Serial.println("Keyboard: invalid application mode");
      _appMode = MODE_ARCADE;
      pOrder   = arcadeScanOrder;
      pReport  = arcadeScanReport;
      maxKey   = ARCADE_SCAN_MAX;
      break;
  }

  // scan keys
  for(int i=0;i < maxKey;i++) {
      currentKey    = pOrder[i];
      currentReport = pReport[i];
      currentState  = digitalRead(keyPin[currentKey]);

      // check for standard and addtional sction
      if (currentState == LOW){
        if (keyCheckFallingEdge(currentKey,currentState,currentReport) == true){
          checkForAdditionalPressAction(currentKey);  // like press event
        } else {
          checkForAdditionalHoldAction(currentKey);   // like autofire
        }
      } else {
        if (keyCheckRisingEdge(currentKey,currentState,currentReport) == true) {
          checkForAdditionalReleaseAction(currentKey);
          gameTimeoutCounter = 0;
        }
      }
  }
  checkForGameTimeout();
  updateAutoFireCounter();
}



void resetKeyStates(){
  for(int i=0;i<KEY_MAX;i++){
    keyStatus[i] = false;
    keyCounter[i] = 0;
  }

  gameTimeoutCounter = 0;
  autoFireCounter    = 0;
}



/*
 * 
 * additional Action (not only Key Send)
 * 
 */


void checkForAdditionalReleaseAction(int key){
  switch(_appMode){
    case MODE_ARCADE_DARK:  checkForAdditionalReleaseActionArcade(key);     break;
    case MODE_ARCADE:       checkForAdditionalReleaseActionArcade(key);     break;
    case MODE_MOUSE:        checkForAdditionalReleaseActionMouse(key);      break;
    case MODE_FIRE_CONFIG:  checkForAdditionalReleaseActionConfig(key);     break;
  }
}

void checkForAdditionalPressAction(int key){
  switch(_appMode){
    case MODE_ARCADE_DARK:  checkForAdditionalPressActionArcade(key);     break;
    case MODE_ARCADE:       checkForAdditionalPressActionArcade(key);     break;
    case MODE_MOUSE:        checkForAdditionalPressActionMouse(key);      break;
    case MODE_FIRE_CONFIG:  checkForAdditionalPressActionConfig(key);     break;
  }
}

void checkForAdditionalHoldAction(int key){
  switch(_appMode){
    case MODE_ARCADE_DARK:  checkForAdditionalHoldActionArcade(key);     break;
    case MODE_ARCADE:       checkForAdditionalHoldActionArcade(key);     break;
    case MODE_MOUSE:        checkForAdditionalHoldActionMouse(key);      break;
    case MODE_FIRE_CONFIG:  checkForAdditionalHoldActionConfig(key);     break;
  }
}


/*
 * 
 *  additional action arcade
 * 
 */

void checkForAdditionalPressActionArcade(int key){
}

void checkForAdditionalHoldActionArcade(int key){
  // in arcade mode check on hold button for autofire
  if ((keyStatus[key] == true) && (_autoFireBut[key] == true))
  { // key active (LOW) and autofire for this Button selected
    if (autoFireCounter == 0){
      if (_debugMode == MODE_RUN){
          Keyboard.press(keyCode[key]);    
      } else {
          Serial.println("key Autofire press :" + keyCodeText[key]);
      }
    } else if (autoFireCounter * CYCLE_TIME == AUTO_FIRE_CYCLE_TIME) {
      if (_debugMode == MODE_RUN){
          Keyboard.release(keyCode[key]);    
      } else {
          Serial.println("key Autofire release :" + keyCodeText[key]);
      }
    }
  }
}

void checkForAdditionalReleaseActionArcade(int key){
  switch(key){
    case KEY_MODE:        switchMode();      break;
    case KEY_KILL:        kill();            break;
    case KEY_VOLUME_UP:   volumeUp();        break;
    case KEY_VOLUME_DOWN: volumeDown();      break;
    case KEY_PLAYER_1:    _onePlayer = ON;  _twoPlayer = OFF;     break;
    case KEY_PLAYER_2:    _twoPlayer = ON;  _onePlayer = OFF;     break;
  }
}

/*
 * 
 *  additional action mouse
 * 
 */
void checkForAdditionalPressActionMouse(int key){
  switch(key){
    case P1_BUT4:         Mouse.press(MOUSE_LEFT);    break;
    case P1_BUT5:         Mouse.press(MOUSE_MIDDLE);  break;
    case P1_BUT6:         Mouse.press(MOUSE_RIGHT);   break;
    case P2_BUT4:         Mouse.press(MOUSE_LEFT);    break;
    case P2_BUT5:         Mouse.press(MOUSE_MIDDLE);  break;
    case P2_BUT6:         Mouse.press(MOUSE_RIGHT);   break;
  }
}

void checkForAdditionalHoldActionMouse(int key){
}

void checkForAdditionalReleaseActionMouse(int key){
  switch(key){
    case KEY_MODE:        switchMode();                 break;
    case KEY_KILL:        kill();                       break;
    case KEY_VOLUME_UP:   volumeUp();                   break;
    case KEY_VOLUME_DOWN: volumeDown();                 break;
    case P1_BUT4:         Mouse.release(MOUSE_LEFT);    break;
    case P1_BUT5:         Mouse.release(MOUSE_MIDDLE);  break;
    case P1_BUT6:         Mouse.release(MOUSE_RIGHT);   break;
    case P2_BUT4:         Mouse.release(MOUSE_LEFT);    break;
    case P2_BUT5:         Mouse.release(MOUSE_MIDDLE);  break;
    case P2_BUT6:         Mouse.release(MOUSE_RIGHT);   break;
  }
}


/*
 * 
 *  additional action config
 * 
 */
void checkForAdditionalPressActionConfig(int key){
}

void checkForAdditionalHoldActionConfig(int key){
}

void checkForAdditionalReleaseActionConfig(int key){
  switch(key){
    case KEY_MODE:        switchMode();                 break;
    default:
        // change autofire status of current button
        // ~ does not work on bool ==> use if then else
        if (_autoFireBut[key] == false){
          _autoFireBut[key]= true;
          if (_debugMode == MODE_TEST){
            Serial.println("autofire set for :"+keyCodeText[key]);
          }
        } else { 
          _autoFireBut[key]= false;            
          if (_debugMode == MODE_TEST){
            Serial.println("autofire cleared for :"+keyCodeText[key]);
          }
        }
        break;
  }
}


/*
 * 
 * common action functions
 * 
 */

void volumeUp(){
  u8_t data[2];
  data[0] = kVolumeUp;
  data[1] = 0;
  HID().SendReport(kHID_ReportID_ConsumerControl, data, 2);
}

void volumeDown(){
  u8_t data[2];
  data[0] = kVolumeDown;
  data[1] = 0;
  HID().SendReport(kHID_ReportID_ConsumerControl, data, 2);
}

void kill(){
  // time slicing hurt, but does not matter
  Keyboard.releaseAll();
  delay(100);
  Keyboard.press(KEY_LEFT_ALT);  
  Keyboard.press(KEY_F4);
  delay(100);
  Keyboard.releaseAll();
  resetKeyStates();
}

void switchMode(){
  _appMode++;
  if(_appMode == MODE_MAX){
    _appMode = MODE_ARCADE; 
  }
  resetKeyStates();
}

/*
 *  
 *   key low level function to check pin state with or without debouncing
 * 
 */


bool keyCheckFallingEdge(int i,int state,bool report){
#ifndef DEBOUNCE
  if ((state == LOW) && (keyStatus[i] == false))
  {  // new pressed  falling edge 
      if (_debugMode == MODE_RUN){
        if (report == true){
          Keyboard.press(keyCode[i]);             
        }
      } else {
          Serial.println("key pressed:" + keyCodeText[i]);
      }
      keyStatus[i] = true;
      return true;
  }
  return false;
#else
  if (keyStatus[i] == false)
  { // key status released
      if (state == LOW)
      {
          keyCounter[i]++;
          if (keyCounter[i] > KEY_TRESHOLD_ON)
          {//switch on
              if (_debugMode == MODE_RUN){
                if (report == true){
                  Keyboard.press(keyCode[i]);             
                }
              } else {
                  Serial.println("key pressed:" + keyCodeText[i]);
              }
              keyStatus[i] = true;
              keyCounter[i] =0;
              return true;
          }
      }
      else
      {   //restart debouncing 
          keyCounter[i] = 0;
      }
      return false;
  }

#endif
}


bool keyCheckRisingEdge(int i,int state,bool report){
#ifndef DEBOUNCE
  if ((state == HIGH) && (keyStatus[i] == true))
  {   // new released  rising edge
      if (_debugMode == MODE_RUN){
        if (report == true){
          Keyboard.release(keyCode[i]);             
        }
      } else {
          Serial.println("key released:"+ keyCodeText[i]);
      }
      keyStatus[i] = false;
      return true;
  } 
  return false;
#else
  if (state == HIGH)
  {
      keyCounter[i]++;
      if (keyCounter[i] > KEY_TRESHOLD_OFF)
      {//switch off 
          if (_debugMode == MODE_RUN){
            if (report == true){
              Keyboard.release(keyCode[i]);             
            }
          } else {
              Serial.println("key released:" + keyCodeText[i]);
          }
          keyStatus[i] = false;
          keyCounter[i] = 0;
          return true;
      }
  }
  else
  {   //restart debouncing 
      keyCounter[i] = 0;
  }
  return false;


#endif
}

void checkForGameTimeout(void){
    if (gameTimeoutCounter == 0){
      _noGame = false;
      gameTimeoutCounter++;
    } else if (gameTimeoutCounter*CYCLE_TIME < GAME_TIMEOUT){
      gameTimeoutCounter++;
    } else if (gameTimeoutCounter*CYCLE_TIME == GAME_TIMEOUT){
      _noGame=true;
      gameTimeoutCounter++;
    } else if (gameTimeoutCounter*CYCLE_TIME > GAME_TIMEOUT){
      // do nothing wait for next key
    } else {
      if (_debugMode == MODE_TEST){
        Serial.println("key Scan State error");
      }    
    }
}



void updateAutoFireCounter(){
  autoFireCounter++;
  if (autoFireCounter * CYCLE_TIME >  AUTO_FIRE_CYCLE_TIME *2){
     autoFireCounter = 0;
  }
}
