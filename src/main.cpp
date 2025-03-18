
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


#include <Arduino.h>
#include <MainConfig.h>
#include <PinMapping.h>
#include <Blink.hpp>

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
#include <BufferedClock.hpp>

#include <configCollection.hpp>

/*****************************************************************
 * 
 *    ToDo'S / aufräumen
 * 
 ******************************************************************
  + use AniCfg in hotkey / scenario 
  + make bakup (and read back) of last setup data .. how to deal with binary data ?
  + implement startup test mode
  + implement boot screen
  + add new second class functionality with reset or not  ????
  + exchange JSON config by YAML
 */




/*****************************************************************
 * 
 *    VAR's
 * 
 ******************************************************************
 */

volatile bool setupStartsecondCore = false;   // false:  first core0 setup .. then core1 setup      || true: core0 and core1 setup in parallel
volatile bool waitForsecondCore    = true;    // false:  core0 starts with loop directly after setup|| true: core0 waits for core1 to finish setup first, then start loop

BlinkingLED blink(LED_BUILTIN);

// menuCtrlEntries  ist the selection of connected CTRL objects in the menus
typedef MenuEntryList* MenuEntryListPtr;
MenuEntryListPtr menuCtrlEntries[CTRL_OBJECT_COUNT] = {&menuMainSwitch1,&menuMainSwitch2,&menuMainSwitch3,&menuMainSwitch4,
                                                      &menuMainRGB1,&menuMainRGB2,&menuMainNeo1,&menuMainNeo2,
                                                      &menuMainMatrix1,&menuMainMatrix2};



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
      ctrlObjectList[i]->select(nrMenu);
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
#include <configItem.hpp>

void TestDebug(){
  String configIn;
  JsonDocument config;

  configIn ="";
  configIn += " {                                                    \r\n";
  configIn += "     \"obj\"  : \"LED0\",                             \r\n";
  configIn += "     \"cmd\"  :    \"off\",                           \r\n";
  configIn += "     \"param\": [\"0x00FF00\",\"  2 \",\" 0 \",\"\"], \r\n";
  configIn += "     \"str\"  :  \"abc -# \",                         \r\n";
  configIn += " }                                         ";

  DeserializationError error = deserializeJson(config, configIn);
  if (error) {
    String msg = "deserializeJson() failed: ";
    msg += error.c_str(); 
    LOG(msg.c_str());
    String msg2 = msg;
  }
}


/*****************************************************************
 * 
 *    Setups
 * 
 ******************************************************************
 */

void setup() {
  blink.on();
  #ifdef WAIT_FOR_TERMINAL
  while (millis()  < WAIT_FOR_TERMINAL) {  }; 
  #endif

  Serial1.begin(115200);
  Serial1.println(" start DEBUG module ");
  debug.begin(&Serial1);

  LOG(F("setup 0:  start random"));
  randomSeed(analogRead(PIN_MIC_ANALOG));

  LOG(F("setup 0:  start Co Prozessor Arduino Nano - PWM"));
  pinMode(PIN_RESET_NANO,OUTPUT);
  digitalWrite(PIN_RESET_NANO,LOW);
  delay(5);
  digitalWrite(PIN_RESET_NANO,HIGH);

  LOG(F("setup 0:  start SPI & I2C"));
  SPI.begin();
  SPI1.begin();
  Wire.begin();
  Wire.setClock(I2C_DEFAULT_SPEED);

  LOG(F("setup 0: RTC"));
  bufferedClock.begin(&Wire,&I2C0_mutex);
  LOG(bufferedClock.getLoopDateTime().timestamp().c_str());

  
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

  blink.off();
  setupStartsecondCore = true;
  while(waitForsecondCore == true){
  }

  blink.setup(BLINK_SEQ_MAIN);
  LOG(F("setup 0: done"));
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
  static uint8_t loop1State=1;
  uint32_t now = millis();
  static uint32_t lastUpdate;

  #ifdef PRINT_LOOP_STATS
    static LoopStats stats(10,1);
    stats.measureAndPrint(now,PRINT_LOOP_STATS,F_CONST("loop0 stats:"));
  #endif

  blink.loop(now);
  bufferedClock.loop(now);

  switch(loop1State){
      case 1:   ledCtrl1.loop(now);               break;
      case 2:   ledCtrl2.loop(now);               break;
      case 3:   ledCtrl3.loop(now);               break;
      case 4:   ledCtrl4.loop(now);               break;
      case 5:   rgbCtrl1.loop(now);               break;
      case 6:   rgbCtrl2.loop(now);               break;
      case 7:   com.loop();                       break;
      default:  loop1State = 0;                     break;
  }
  loop1State++;
  if (now-lastUpdate >= 20){
    keyboard.loop(now);
    lastUpdate = now;
    i2c_master.write_regSet(I2C_ADR_SLAVE,&I2C_slaveSoll,sizeof(I2C_slaveSoll),false);
  }
}




void loop1(){
  static uint8_t loop2State=1;
  uint32_t now = millis();
  String time(now/1000);
  EventType event;

  #ifdef PRINT_LOOP_STATS
    static LoopStats stats(20,5);
    stats.measureAndPrint(now,PRINT_LOOP_STATS,F_CONST("loop1 stats:"));
  #endif

  switch(loop2State){
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
              if (event >= EVENT_A1){
                globalHotkey.onEvent(event);
                mainMenu_syncFromCtrl();
              } else {
                menuHandler.onEvent(event);
                mainMenu_syncToCtrl();
              }
              event = keyboard.getNextEvent();
            }
          }
          break;
      case 6:  
          // draw menu            
          menuHandler.loop(now);          
          break;
      default:  loop2State = 0;                     break;
  }
  loop2State++;
}





