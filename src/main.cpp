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

/*****************************************************************
 * 
 *    ToDo'S / aufräumen
 * 
 ******************************************************************
  + implement startup test mode
  + implement boot screen
  + add new second class functionality with reset or not
  + add menuEntryList
 */

#ifdef WITH_DISPLAY
  #include <Display.hpp>
#endif

#ifdef WITH_SD_CARD
  #include <SDcard.hpp>
#endif



/*****************************************************************
 * 
 *    VAR's
 * 
 ******************************************************************
 */

volatile bool setupStartsecondCore = false;   // false:  first core0 setup .. then core1 setup      || true: core0 and core1 setup in parallel
volatile bool waitForsecondCore    = true;    // false:  core0 starts with loop directly after setup|| true: core0 waits for core1 to finish setup first, then start loop

uint8_t free_buffer[128];

#define CTRL_OBJECT_COUNT   10
typedef Ctrl* CtrlPtr;
CtrlPtr ctrlObjectList[CTRL_OBJECT_COUNT] = {      pLedCtrl1,pLedCtrl2,pLedCtrl3,pLedCtrl4,
                                                  pRgbCtrl1,pRgbCtrl2,pNeoStripeCtrl1,pNeoStripeCtrl2,
                                                  pNeoMatrixCtrl1,pNeoMatrixCtrl2}; 
// menuCtrlEntries  ist the selection of connected CTRL objects in the menus
typedef MenuEntryList* MenuEntryListPtr;
MenuEntryListPtr menuCtrlEntries[CTRL_OBJECT_COUNT] = {&menuMainSwitch1,&menuMainSwitch2,&menuMainSwitch3,&menuMainSwitch4,
                                                  &menuMainRGB1,&menuMainRGB2,&menuMainNeo1,&menuMainNeo2,
                                                  &menuMainMatrix1,&menuMainMatrix2};

uint8_t free_buffer2[128];


/*****************************************************************
 * 
 *    functions
 * 
 ******************************************************************
 */

void mainMenu_syncToCtrl(){
  int nr;
  for(int i=0;i<CTRL_OBJECT_COUNT;i++){
    nr = menuCtrlEntries[i]->getIndex();
    if (nr > 0){
      ctrlObjectList[i]->setup(nr);
    }
  }
}

void mainMenu_syncFromCtrl(){
  int nr;
  for(int i=0;i<CTRL_OBJECT_COUNT;i++){
    nr = ctrlObjectList[i]->getNr();
    if (nr > 0){
      menuCtrlEntries[i]->setIndex(nr);
    }
  }
}



void mainMenu_begin(){

    String aniList = pLedCtrl1->getNameList();
    menuMainSwitch1.setValueList(aniList);
    menuMainSwitch2.setValueList(aniList);
    menuMainSwitch3.setValueList(aniList);
    menuMainSwitch4.setValueList(aniList);

    aniList = pRgbCtrl1->getNameList();
    menuMainRGB1.setValueList(aniList);
    menuMainRGB2.setValueList(aniList);

    aniList = pNeoStripeCtrl1->getNameList();
    menuMainNeo1.setValueList(aniList);
    menuMainNeo2.setValueList(aniList);

    aniList = pNeoMatrixCtrl1->getNameList();
    menuMainMatrix1.setValueList(aniList);
    menuMainMatrix2.setValueList(aniList);


    ctrlObjectList[0] = pLedCtrl1;
    ctrlObjectList[1] = pLedCtrl2;
    ctrlObjectList[2] = pLedCtrl3;
    ctrlObjectList[3] = pLedCtrl4;
    
    ctrlObjectList[4] = pRgbCtrl1;
    ctrlObjectList[5] = pRgbCtrl2;

    ctrlObjectList[6] = pNeoStripeCtrl1;
    ctrlObjectList[7] = pNeoStripeCtrl2;

    ctrlObjectList[8] = pNeoMatrixCtrl1;
    ctrlObjectList[9] = pNeoMatrixCtrl2;


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

  LOG(F("setup 0:  start SPI & I2C"));
  SPI.begin();
  SPI1.begin();
  Wire.begin();
  Wire.setClock(400*1000);

  LOG(F("setup 0: COM interface"));
  com.begin(&Serial,115200,SERIAL_8N1);

  #ifdef WITH_SD_CARD
    LOG(F("setup 0: SD card on SPI 1..."));
      if (!globalSDcard0.begin(PIN_SD_CS)) {
        LOG(F("setup 0: SD card initialization failed!"));
      } else {
        LOG(F("setup 0: SD card initialization done."));
      }
      //globalSDcard0.end();
  #endif


  //analogWriteFreq(3200);
  //analogWriteRange(255);

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


  #ifdef WITH_DISPLAY
    LOG(F("setup 1: TFT"));
    // ToDo: aufräumne test auslagern .....  sub Function irgendwo anders (DisplayObjects??)  in einem anderen File das muss kürzer werden
    // toDo change this to DisplayObjects (contains TFT )  and TFT_begin   to rotate,test , logo  display etc 
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
  #endif

  LOG(F("setup 1: keyboard"));
  keyboard.begin(&Wire,0,&I2C0_mutex,keyboardStdMapping);


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

  #ifdef PRINT_LOOP_STATS
    static LoopStats stats(10,1);
    stats.measureAndPrint(now,PRINT_LOOP_STATS,F_CONST("loop0 stats:"));
  #endif

  keyboard.loop(now);

  switch(prgState){
      case 1:   pLedCtrl1->loop(now);             break;
      case 2:   pLedCtrl2->loop(now);             break;
      case 3:   pLedCtrl3->loop(now);             break;
      case 4:   pLedCtrl4->loop(now);             break;
      case 5:   pRgbCtrl1->loop(now);             break;
      case 6:   com.loop();                       break;
      case 7:   toggleLed(now);                   break;
      default:  prgState = 0;                     break;
  }
  prgState++;
}




void loop1(){
  static uint8_t prgState=1;
  uint32_t now = millis();
  String time(now/1000);
  Event_Type event;

  #ifdef PRINT_LOOP_STATS
    static LoopStats stats(20,5);
    stats.measureAndPrint(now,PRINT_LOOP_STATS,F_CONST("loop1 stats:"));
  #endif

  switch(prgState){
      case 1:   pNeoMatrixCtrl1->loop(now);       break;
      case 2:   pNeoMatrixCtrl2->loop(now);       break;
      case 3:   pNeoStripeCtrl1->loop(now);       break;
      case 4:   pNeoStripeCtrl1->loop(now);       break;
      #ifdef WITH_DISPLAY
        case 5:
            // update menu entries
            menuTestTime.setNewValueText(time.c_str());
            event = keyboard.getNextEvent();
            if (event != EVENT_NONE){
              while(event != EVENT_NONE){
                menuHandler.onEvent(event);
                event = keyboard.getNextEvent();
              }
              mainMenu_syncToCtrl();
            }
            break;
        case 6:  
            // draw menu            
            menuHandler.loop(now);          
            break;
      #endif
      default:  prgState = 0;                     break;
  }
  prgState++;
}





