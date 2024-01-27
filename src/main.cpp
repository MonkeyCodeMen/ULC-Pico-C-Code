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

/*****************************************************************
 * 
 *    functions
 * 
 ******************************************************************
 */




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

  LOG(F("setup 0:"));
  //analogWriteFreq(3200);
  //analogWriteRange(255);

  LOG(F("setup 0: Test functions"));
  TestDebug();

  LOG(F("setup 0: COM interface"));
  com.begin(&Serial,115200,SERIAL_8N1);

    #ifdef WITH_SD_CARD
    LOG(F("setup 0: SD card on SPI 1..."));
    SPI1.begin();
      if (!globalSDcard0.begin(PIN_SD_CS)) {
        LOG(F("setup 0: SD card initialization failed!"));
      } else {
        LOG(F("setup 0: SD card initialization done."));
      }
      //globalSDcard0.end();
  #endif

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

    SPI.begin();
    display.begin();

    LOG(F("setup 1: menu"));
    menuHandler.begin(&menuTestHeader,(MenuEntry **)&menuTest,MENU_TEST_COUNT,&display);
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
      case 3:   pRgbCtrl1->loop(now);             break;
      case 4:   com.loop();                       break;
      case 5:   toggleLed(now);                   break;
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
            do{
              event = keyboard.getNextEvent();
              if (event != EVENT_NONE){
                menuHandler.onEvent(event);
              }
            } while (event != EVENT_NONE );
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


