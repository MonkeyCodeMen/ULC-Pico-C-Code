#include <Arduino.h>
#include <MainConfig.h>
#include <PinMapping.h>

#include <SPI.h>
#include <Debug.hpp>
#include <helper.h>

#include <LedObjects.hpp>
#include <Menu.hpp>

#include <LoopStats.hpp>
#include <Com.hpp>
#include <keyboard.hpp>
#include <Resources.hpp>

#include <I2C_master.hpp>
#include <I2C_register.h>

#include <Display.hpp>
#include <SDcard.hpp>

#include <configCollection.hpp>

/*****************************************************************
 * 
 *    ToDo'S / aufräumen
 * 
 ******************************************************************
  + make bakup (and read back) of last setup data .. how to deal with binary data ?
  + implement startup test mode
  + implement boot screen
  + add new second class functionality with reset or not  ????
 */




/*****************************************************************
 * 
 *    VAR's
 * 
 ******************************************************************
 */

volatile bool setupStartsecondCore = false;   // false:  first core0 setup .. then core1 setup      || true: core0 and core1 setup in parallel
volatile bool waitForsecondCore    = true;    // false:  core0 starts with loop directly after setup|| true: core0 waits for core1 to finish setup first, then start loop


// menuCtrlEntries  ist the selection of connected CTRL objects in the menus
typedef MenuEntryList* MenuEntryListPtr;
MenuEntryListPtr menuCtrlEntries[CTRL_OBJECT_COUNT] = {&menuMainSwitch1,&menuMainSwitch2,&menuMainSwitch3,&menuMainSwitch4,
                                                      &menuMainRGB1,&menuMainRGB2,&menuMainNeo1,&menuMainNeo2,
                                                      &menuMainMatrix1,&menuMainMatrix2};


I2C_SlaveReceiveReg_Struct  I2C_slaveSoll;
I2C_SlaveTransmitReg_Struct I2C_slaveIst;


/*****************************************************************
 * 
 *    functions
 * 
 ******************************************************************
 */

void mainMenu_syncToCtrl(){
  int nrMenu,nrCtrl;
  for(int i=0;i<CTRL_OBJECT_COUNT;i++){
    nrMenu = menuCtrlEntries[i]->getIndex();
    nrCtrl = ctrlObjectList[i]->getNr();
    if ((nrMenu >= 0) && (nrCtrl != nrMenu)){
      ctrlObjectList[i]->setup(nrMenu);
    }
  }
}

void mainMenu_syncFromCtrl(){
  int nr;
  for(int i=0;i<CTRL_OBJECT_COUNT;i++){
    nr = ctrlObjectList[i]->getNr();
    if (nr >= 0){
      menuCtrlEntries[i]->setIndex(nr);
    }
  }
}



void mainMenu_begin(){
    String aniList = ledCtrl1.getNameList();
    menuMainSwitch1.setValueList(aniList);
    menuMainSwitch2.setValueList(aniList);
    menuMainSwitch3.setValueList(aniList);
    menuMainSwitch4.setValueList(aniList);

    aniList = rgbCtrl1.getNameList();
    menuMainRGB1.setValueList(aniList);
    menuMainRGB2.setValueList(aniList);

    aniList = neoStripeCtrl1.getNameList();
    menuMainNeo1.setValueList(aniList);
    menuMainNeo2.setValueList(aniList);

    aniList = neoMatrixCtrl1.getNameList();
    menuMainMatrix1.setValueList(aniList);
    menuMainMatrix2.setValueList(aniList);
}


void toggleLed(uint32_t now){
  static uint32_t timerLED=0;
  uint32_t diff = now-timerLED;

  if (diff < 250){
    digitalWrite(LED_BUILTIN,HIGH);
  } else if (diff < 500){
    digitalWrite(LED_BUILTIN,LOW);
  } else if (diff < 1000){
    digitalWrite(LED_BUILTIN,HIGH);
  } else if (diff < 1500){
    digitalWrite(LED_BUILTIN,LOW);
  } else if (diff < 1750){
    digitalWrite(LED_BUILTIN,HIGH);
  } else if (diff < 2000){
    digitalWrite(LED_BUILTIN,LOW);
  } else {
    timerLED = now;
  }
}

/* 
ToDo: no longer used, but where should we place this
void renderDisplay(uint32_t now){
    #ifdef WITH_DISPLAY
    static uint32_t lastCycle=0;

    if (now-lastCycle > 25){
        pCube->Render();
        pCube->moveView();
      lastCycle = now;
    }
    #endif
}
*/

void TestDebug(){
  // place code to debug here 
  // will be called early in setup of core 0
}


/*****************************************************************
 * 
 *    Setups
 * 
 ******************************************************************
 */

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  #ifdef WAIT_FOR_TERMINAL
  while (millis()  < WAIT_FOR_TERMINAL) {  }; 
  #endif

  Serial1.begin(115200);
  Serial1.println(" start DEBUG module ");
  debug.begin(&Serial1);

  LOG(F("setup 0:  start Co Prozessor Arduino Nano - PWM"));
  pinMode(PIN_RESET_NANO,OUTPUT);
  digitalWrite(PIN_RESET_NANO,LOW);
  delay(5);
  digitalWrite(PIN_RESET_NANO,HIGH);

  LOG(F("setup 0:  start SPI & I2C"));
  SPI.begin();
  SPI1.begin();
  Wire.begin();
  Wire.setClock(400*1000);
  
  LOG(F("setup 0: PWM expander"));
  i2c_master.begin(&Wire,&I2C0_mutex);

  LOG(F("setup 0: keyboard"));
  keyboard.begin(&Wire,0,&I2C0_mutex,keyboardStdMapping);

  LOG(F("setup 0: COM interface"));
  com.begin(&Serial,115200,SERIAL_8N1);

  LOG(F("setup 0: SD card on SPI 1..."));
    if (!globalSDcard0.begin(PIN_SD_CS)) {
      LOG(F("setup 0: SD card initialization failed!"));
    } else {
      LOG(F("setup 0: SD card initialization done."));
      LOG(F("setup 0: load config File"));
      globalHotkey.openFromFile("hotkey.cfg");
    }

  LOG(F("setup 0: Test functions"));
  TestDebug();

  LOG(F("setup 0: LED"));
  setupLed();


  setupStartsecondCore = true;
  while(waitForsecondCore == true){
  }

  LOG(F("setup 0: done"));
  digitalWrite(LED_BUILTIN, LOW);
}

void setup1() {
  while(setupStartsecondCore == false){
  }

  LOG(F("setup 1:"));


  LOG(F("setup 1: TFT"));
  display.begin();

  LOG(F("setup 1: menu"));
  mainMenu_begin();
  menuHandler.begin(&menuMainHeader,(MenuEntry **)&menuMain,MENU_MAIN_COUNT,&display);

  LOG(F("setup 1: sync menu entries & ctrl objects"));
  mainMenu_syncFromCtrl();

  LOG(F("setup 1: draw menue"));
  menuHandler.loop(0);
  //LOG(F("setup 1: cube"));
  //pCube = new Cube(pTFT);  // cube includes SPI mutex handling itself

  LOG(F("setup 1: done"));
  waitForsecondCore = false;
}


/*****************************************************************
 * 
 *    Loops
 * 
 ******************************************************************
 */
void loop() {
  static uint8_t prgState=1;
  uint32_t now = millis();
  static uint32_t lastUpdate;

  #ifdef PRINT_LOOP_STATS
    static LoopStats stats(10,1);
    stats.measureAndPrint(now,PRINT_LOOP_STATS,F_CONST("loop0 stats:"));
  #endif


  switch(prgState){
      case 1:   ledCtrl1.loop(now);               break;
      case 2:   ledCtrl2.loop(now);               break;
      case 3:   ledCtrl3.loop(now);               break;
      case 4:   ledCtrl4.loop(now);               break;
      case 5:   rgbCtrl1.loop(now);               break;
      case 6:   rgbCtrl2.loop(now);               break;
      case 7:   com.loop();                       break;
      case 8:   toggleLed(now);                   break;
      default:  prgState = 0;                     break;
  }
  prgState++;
  if (now-lastUpdate >= 20){
    keyboard.loop(now);
    lastUpdate = now;
    i2c_master.write_regSet(I2C_ADR_SLAVE,&I2C_slaveSoll,sizeof(I2C_slaveSoll),false);
  }
}




void loop1(){
  static uint8_t prgState=1;
  uint32_t now = millis();
  String time(now/1000);
  EventType event;

  #ifdef PRINT_LOOP_STATS
    static LoopStats stats(20,5);
    stats.measureAndPrint(now,PRINT_LOOP_STATS,F_CONST("loop1 stats:"));
  #endif

  switch(prgState){
      case 1:   neoMatrixCtrl1.loop(now);       break;
      case 2:   neoMatrixCtrl2.loop(now);       break;
      case 3:   neoStripeCtrl1.loop(now);       break;
      case 4:   neoStripeCtrl2.loop(now);       break;
      case 5:
          // update menu entries
          menuTestTime.setNewValueText(time.c_str());
          event = keyboard.getNextEvent();
          if (event != EVENT_NONE){
            while(event != EVENT_NONE){
              menuHandler.onEvent(event);
              globalHotkey.onEvent(event);
              event = keyboard.getNextEvent();
            }
            mainMenu_syncToCtrl();
          }
          break;
      case 6:  
          // draw menu            
          menuHandler.loop(now);          
          break;
      default:  prgState = 0;                     break;
  }
  prgState++;
}





