#include <Arduino.h>
#include <MainConfig.h>
#include <PinMapping.h>
#include <SPI.h>

#include <Debug.hpp>
#include <helper.hpp>

#include <globalObjects.hpp>
#include <LoopStats.hpp>

void TestDebug();



volatile bool setupStartsecondCore = false;
volatile bool waitForsecondCore    = true;





/***********************************************************************************************************************************/
void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  LOG(F_CONST("setup 0:"));
  //analogWriteFreq(3200);
  //analogWriteRange(255);


  LOG(F_CONST("setup 0: Test functions"));
  TestDebug();

  LOG(F_CONST("setup 0: LED switch"));
  pLedCtrl1 = new LedCtrl(&ledStripe1);
  pLedCtrl2 = new LedCtrl(&ledStripe2);
  pLedCtrl1->setup(F_CONST("blink"));
  pLedCtrl1->setup(0xFF,250,250,0,0,NULL);
  pLedCtrl2->setup(F_CONST("blink"));
  pLedCtrl2->setup(0xFF,250,250,0,0,NULL);

  LOG(F_CONST("setup 0: RGB LED"));
  pRgbCtrl1 = new RgbLedCtrl(&rgbLedStrip1);
  pRgbCtrl1->setup(F_CONST("multi flash"));  

  LOG(F_CONST("setup 0: Neo stripe"));
  pNeoStripeCtrl1 = new NeoStripeCtrl(&ws2812strip1);
  pNeoStripeCtrl2 = new NeoStripeCtrl(&ws2812strip2);
  pNeoStripeCtrl1->setup(13);  
  pNeoStripeCtrl2->setup(13);  

  LOG(F_CONST("setup 0: Neo matrix"));
  pNeoMatrixCtrl1 = new NeoMatrixCtrl(&neoMatrix1);
  pNeoMatrixCtrl2 = new NeoMatrixCtrl(&neoMatrix2);
  pNeoMatrixCtrl1->setup("breath");
  pNeoMatrixCtrl2->setup("breath");

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

  LOG(F_CONST("setup 1: COM interface"));
  pCom = new Com();
  pCom->setup();

  #ifdef WITH_DISPLAY
    LOG(F_CONST("setup 1: TFT"));
    pTFT = new TFT_eSPI();
    pTFT->init();
    pTFT->setRotation(1);
    pTFT->fillScreen(TFT_BLACK);
    pinMode(PIN_TFT_LED, OUTPUT);
    analogWrite(PIN_TFT_LED,TFT_DIM);

    LOG(F_CONST("setup 1: cube"));
    pCube = new Cube(pTFT);
  #endif

  #ifdef WITH_SD_CARD
    LOG(F_CONST("setup 1: Initializing SD card..."));
    if (!SD.begin(PIN_SPI0_CS_SD)) {
      LOG(F_CONST("setup 1: SD card initialization failed!"));
    }
    LOG(F_CONST("setup 1: SD card initialization done."));
    root = SD.open("/");
    String dir = printDirectory(root, 0);
    dir = "setup 1: directory of SD card:\n\n" + dir +"\n\n";
    LOG(dir.c_str());
  #endif
 
  LOG(F_CONST("setup 1: done"));
  waitForsecondCore = false;
}


/***********************************************************************************************************************************/

void loop() {
  static u8_t prgState=1;
  static u8_t ledState=0;
  static u32_t lastSwitch=0;
  static u32_t lastStatReport = 0;
  static LoopStats stats(20,10);
  
  
  u32_t now = millis();
  stats.measure(now);

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

  #ifdef PRINT_LOOP_STATS
    if (now - lastStatReport > 5000){
      String out = stats.print();
      LOG(out.c_str());
      lastStatReport = now;
    }
  #endif

  switch(prgState){

    case 1:   pLedCtrl1->loop(now);            break;
    case 2:   pLedCtrl2->loop(now);            break;
    case 3:   pRgbCtrl1->loop(now);            break;
    case 4:   pLedCtrl1->loop(now);
              pNeoStripeCtrl1->loop(now);      break;
    
    case 5:   pLedCtrl1->loop(now);
              pNeoStripeCtrl2->loop(now);      break;
    
    case 6:   pRgbCtrl1->loop(now);
              pNeoMatrixCtrl1->loop(now);      break;
    
    case 7:   pLedCtrl2->loop(now);
              pNeoMatrixCtrl2->loop(now);      break;
    
    default:  prgState = 0;                    break;
  }
  prgState++;
}

void loop1(){
  static u32_t lastCycle=0;
  u32_t now = millis();

  if (now-lastCycle > 25){
    #ifdef WITH_DISPLAY
      pCube->Render();
      pCube->moveView();
    #endif
    lastCycle = now;
  }
  pCom->loop();
}

void TestDebug(){
  // place code to debug here (single core before startup)
}


 
#ifdef WITH_SD_CARD
  String printDirectory(SDFile dir, int numTabs) {
    String out="";
    while (true) {

      SDFile entry =  dir.openNextFile();
      if (! entry) {
        // no more files
        break;
      }
      for (uint8_t i = 0; i < numTabs; i++) {
        out+=('\t');
      }
      out+=entry.name();
      if (entry.isDirectory()) {
        out+="/\n";
        out += printDirectory(entry, numTabs + 1);
      } else {
        // files have sizes, directories do not
        out+="\t\t";
        out+=String(entry.size(),DEC);
        out+="\n";
      }
      entry.close();
    }
    return out;
  }
#endif