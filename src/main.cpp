#include <Arduino.h>
#include <MainConfig.h>
#include <PinMapping.h>

#include <SPI.h>
#include <Debug.hpp>
#include <helper.hpp>

#include <LedObjects.hpp>
#include <MenuObjects.hpp>

#include <LoopStats.hpp>
#include <Com.hpp>

/*****************************************************************
 * 
 *    ToDo'S / aufräumen
 * 
 ******************************************************************
  + implement startup test mode
  + implement boot screen
  + split bool class in two differnt class
  + add new second class functionaliyt with reset or not
  + add menuEntryInt
  + add menuEntryList
 */




Mutex globalSPI0_mutex;   // ToDo: wo den anlegen ... HelprObjects ... HwObjects ????
Com * pCom;   // ToDo: class auf begin method umbauen und dann nach ComObjects auslagern 


// ==> WITH DISPLAY macht keinen sinn mehr oder muss in COM auch eingebaut werden 
// toDo change this to DisplayObjects (contains TFT )  and TFT_begin   to rotate display etc 
#ifdef WITH_DISPLAY
  #include <SPI.h>
  #include <TFT_eSPI.h> // Hardware-specific library
  TFT_eSPI * pTFT;       // Invoke custom library

  #define TFT_DIM     int(255*1.0)

  //#include <cube.hpp>
  //Cube * pCube;
#endif

#ifdef WITH_SD_CARD
    #include <SPI.h>
    #include <SD.h>
    extern SDClass globalSDcard0;
#endif 

#ifdef WITH_SD_CARD
  SDClass globalSDcard0;
#endif

extern void gifSetup();
extern void gifLoop();

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




void toggleLed(u32_t now){
  static u32_t timerLED=0;
  u32_t diff = now-timerLED;

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
void renderDisplay(u32_t now){
    #ifdef WITH_DISPLAY
    static u32_t lastCycle=0;

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

  LOG(F_CHAR("setup 0:"));
  //analogWriteFreq(3200);
  //analogWriteRange(255);

  LOG(F_CHAR("setup 0: Test functions"));
  TestDebug();

  LOG(F_CHAR("setup 0: COM interface"));
  // ToDo : change this to ARDUINO Style com.begin()
  // is com and arduino name ???  
  pCom = new Com();
  pCom->setup();  // ==> COM.begin !!!

  LOG(F_CHAR("setup 0: startup SPI"));
  SPI.begin();
  globalSPI0_mutex.free();


  #ifdef WITH_SD_CARD
    LOG(F_CHAR("setup 0: test for  SD card..."));
    globalSPI0_mutex.lock();   
      if (!globalSDcard0.begin(PIN_SD_CS)) {
        LOG(F_CHAR("setup 0: SD card initialization failed!"));
      } else {
        LOG(F_CHAR("setup 0: SD card initialization done."));
      }
      globalSDcard0.end();
    globalSPI0_mutex.unlock();  
  #endif

  LOG(F_CHAR("setup 0: LED"));
  setupLed();

  setupStartsecondCore = true;
  while(waitForsecondCore == true){
  }

  LOG(F_CHAR("setup 0: done"));
  digitalWrite(LED_BUILTIN, LOW);
}

void setup1() {
  while(setupStartsecondCore == false){
  }

  LOG(F_CHAR("setup 1:"));


  #ifdef WITH_DISPLAY
    LOG(F_CHAR("setup 1: TFT"));
    // ToDo: aufräumne test auslagern .....  sub Function irgendwo anders (DisplayObjects??)  in einem anderen File das muss kürzer werden
    // toDo change this to DisplayObjects (contains TFT )  and TFT_begin   to rotate,test , logo  display etc 

    globalSPI0_mutex.lock();   
      pTFT = new TFT_eSPI();
      pTFT->init();
      pTFT->setRotation(3);
      pTFT->fillScreen(TFT_BLACK);
      pinMode(PIN_TFT_LED, OUTPUT);
      analogWrite(PIN_TFT_LED,TFT_DIM);
      #ifdef WITH_DSIPLAY_STARTUP_TEST
        setup_t cfg;
        pTFT->getSetup(cfg);
        String out="";
        out+="\nTFT_eSPI version:  "+cfg.version;
        out+="\nTFT_eSPI setup:    "+cfg.setup_info;
        out+="\nTFT_eSPI id:       "+String(cfg.setup_id);
        out+="\nTFT_eSPI pin_mosi: "+String(cfg.pin_tft_mosi);
        out+="\nTFT_eSPI pin_miso: "+String(cfg.pin_tft_miso);
        out+="\nTFT_eSPI pin_clk:  "+String(cfg.pin_tft_clk);
        out+="\nTFT_eSPI pin_cs:   "+String(cfg.pin_tft_cs);
        out+="\nTFT_eSPI pin_dc:   "+String(cfg.pin_tft_dc);
        out+="\nTFT_eSPI spi port: "+String(cfg.port);
        out+="\nTFT_eSPI f spi:    "+String(cfg.tft_spi_freq);
        out+="\nTFT_eSPI f rd:     "+String(cfg.tft_rd_freq);
        out+="\nTFT_eSPI f touch:  "+String(cfg.tch_spi_freq);
        out+="\nTFT_eSPI width:    "+String(cfg.tft_width);
        out+="\nTFT_eSPI height:   "+String(cfg.tft_height);
        LOG(out.c_str());

        pTFT->fillScreen(TFT_RED);
        delay(200);
        pTFT->fillScreen(TFT_GREEN);
        delay(200);
        pTFT->fillScreen(TFT_BLUE);
        delay(200);
        pTFT->fillScreen(TFT_BLACK);
      #endif
    globalSPI0_mutex.free();

    LOG(F_CHAR("setup 1: menu"));
    menuHandler.begin(&menuTestHeader,(MenuEntry **)&menuTest,12,pTFT,&globalSPI0_mutex);
    menuHandler.loop(0);
    //LOG(F_CHAR("setup 1: cube"));
    //pCube = new Cube(pTFT);  // cube includes SPI mutex handling itself
  #endif


  LOG(F_CHAR("setup 1: done"));
  waitForsecondCore = false;
}


/*****************************************************************
 * 
 *    Loops
 * 
 ******************************************************************
 */
void loop() {
  static u8_t prgState=1;
  u32_t now = millis();

  #ifdef PRINT_LOOP_STATS
    static LoopStats stats(10,1);
    stats.measureAndPrint(now,PRINT_LOOP_STATS,F_CONST("loop0 stats:"));
  #endif
  switch(prgState){
      case 1:   pLedCtrl1->loop(now);             break;
      case 2:   pLedCtrl2->loop(now);             break;
      case 3:   pRgbCtrl1->loop(now);             break;
      case 4:   pCom->loop();                     break;
      case 5:   toggleLed(now);                   break;
      default:  prgState = 0;                     break;
  }
  prgState++;
}




void loop1(){
  static u8_t prgState=1;
  u32_t now = millis();

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
        case 5:   menuHandler.loop(now);          break;
      #endif
      default:  prgState = 0;                     break;
  }
  prgState++;
}


