#include <Arduino.h>
#include <MainConfig.h>
#include <PinMapping.h>
#include <SPI.h>

#include <Debug.hpp>
#include <helper.hpp>

#include <globalObjects.hpp>
#include <LoopStats.hpp>
#include <Com.hpp>


#ifdef WITH_DISPLAY
  #include <SPI.h>
  #include <TFT_eSPI.h> // Hardware-specific library
  TFT_eSPI * pTFT;       // Invoke custom library
  #define TFT_DIM     int(255*1.0)

  #include <cube.hpp>
  Cube * pCube;
#endif

volatile bool setupStartsecondCore = false;
volatile bool waitForsecondCore    = true;

Com * pCom;



/*****************************************************************
 * 
 *    functions
 * 
 ******************************************************************
 */

extern void gifSetup();
extern void gifLoop();


void toggleLed(u32_t now){
  static u8_t ledState=0;
  static u32_t lastSwitch=0;

  switch(ledState){
    case 0:   if (now-lastSwitch >= 250){
                  digitalWrite(LED_BUILTIN, HIGH);
                  lastSwitch = now;
                  ledState = 1;
                  }
              break;
    case 1:   if (now-lastSwitch >= 250){
                  digitalWrite(LED_BUILTIN, LOW);
                  lastSwitch = now;
                  ledState = 0;
                  }
              break;
    default:  LOG(F_CONST("loop 0 unknown ledState"));
              digitalWrite(LED_BUILTIN, LOW);
              lastSwitch = now;
              ledState = 0;
              break;
  }

}

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


void TestDebug(){
  // place code to debug here (single core before startup)
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

  LOG(F_CONST("setup 0:"));
  //analogWriteFreq(3200);
  //analogWriteRange(255);

  LOG(F_CONST("setup 0: Test functions"));
  TestDebug();

  LOG(F_CONST("setup 0: COM interface"));
  pCom = new Com();
  pCom->setup();

  LOG(F_CONST("setup 0: startup SPI"));
  SPI.begin();
  globalSPI0_mutex.unlock();


  #ifdef WITH_SD_CARD
    LOG(F_CONST("setup 0: test for  SD card..."));
    globalSPI0_mutex.lock();   
      if (!globalSDcard0.begin(PIN_SPI0_CS_SD)) {
        LOG(F_CONST("setup 0: SD card initialization failed!"));
      } else {
        LOG(F_CONST("setup 0: SD card initialization done."));
      }
      globalSDcard0.end();
    globalSPI0_mutex.unlock();  
  #endif



  LOG(F_CONST("setup 0: LED switch"));
  pLedCtrl1 = new LedCtrl(&ledStripe1);
  pLedCtrl2 = new LedCtrl(&ledStripe2);
  pLedCtrl1->setup(F_CONST("breath"));
  pLedCtrl2->setup(F_CONST("breath"));

  LOG(F_CONST("setup 0: RGB LED"));
  pRgbCtrl1 = new RgbLedCtrl(&rgbLedStrip1);
  pRgbCtrl1->setup(F_CONST("rainbow"));  

  LOG(F_CONST("setup 0: Neo stripe"));
  pNeoStripeCtrl1 = new NeoStripeCtrl(&ws2812strip1);
  pNeoStripeCtrl2 = new NeoStripeCtrl(&ws2812strip2);
  pNeoStripeCtrl1->setup(13);  
  pNeoStripeCtrl2->setup(13);  

  LOG(F_CONST("setup 0: Neo matrix"));
  pNeoMatrixCtrl1 = new NeoMatrixCtrl(&neoMatrix1);
  pNeoMatrixCtrl2 = new NeoMatrixCtrl(&neoMatrix2);
  pNeoMatrixCtrl1->setup("gif");
  pNeoMatrixCtrl2->setup("off");

  setupStartsecondCore = true;
  while(waitForsecondCore == true){
  }

  LOG(F_CONST("setup 0: done"));
  digitalWrite(LED_BUILTIN, LOW);
}

void setup1() {
  while(setupStartsecondCore == false){
  }

  LOG(F_CONST("setup 1:"));


  #ifdef WITH_DISPLAY
    LOG(F_CONST("setup 1: TFT"));
    globalSPI0_mutex.lock();   
      pTFT = new TFT_eSPI();
      pTFT->init();
      pTFT->setRotation(1);
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
    globalSPI0_mutex.unlock();

    LOG(F_CONST("setup 1: cube"));
    pCube = new Cube(pTFT);  // cube includes SPI mutex handling itself
  #endif


  LOG(F_CONST("setup 1: done"));
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
        case 5:   renderDisplay(now);               break;
      #endif
      default:  prgState = 0;                     break;
  }
  prgState++;
}


