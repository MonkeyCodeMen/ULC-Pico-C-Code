#include <Arduino.h>

#include <Debug.hpp>
#include <MainConfig.h>

#include <helper.h>
#include <SPI.h>
#include <Adafruit_NeoMatrix.h>
#include <SDcard.hpp>
#include <TFT_eSPI.h> // Hardware-specific library
#include "Led.hpp"
#include "Ani.hpp"
#include "StringList.hpp"

#include "unity.h"

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

/*
   define a Ani test class with direct public access to dimCtrl, colorCtrl ...
   so I do not need to define the proxy interfaces in the Ani class
   for infos that I only want to access in the test
 
    Note:
       on the other side access internal states violates the class abstraction idea 
       and can bring extra effort in case of refactoring 
       but for start testing complax behavior of connected objects it s worth the effort
       and much better than declare this object public for all in my eyes. 
       so I use this method from time to time, even if i know there are different opinions on this topic
*/
class AniTest:public Ani{
  public:
  	AniTest():Ani("TestObject")  { 
      pDimCtrl    = &_dimCtrl; 
      pColorCtrl  = &_colorCtrl;
      pFlashCtrl  = &_flashCtrl;
      pBreathCtrl = &_breathCtrl;
  	}
		~AniTest() = default;
  
    // make internal ctrl objects public for easy access
    DimCtrl     * pDimCtrl;
    ColorCtrl   * pColorCtrl;
    FlashCtrl   * pFlashCtrl;
    BreathCtrl  * pBreathCtrl;

};


void test_dimCtrl_init(void){
  Ani obj("testObjANi");
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  TEST_ASSERT_FALSE(obj.hasChanged());
  obj.loop(0);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  TEST_ASSERT_FALSE(obj.hasChanged());
  obj.loop(10);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  TEST_ASSERT_FALSE(obj.hasChanged());
  obj.loop(100);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  TEST_ASSERT_FALSE(obj.hasChanged());
  obj.loop(1000);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  TEST_ASSERT_FALSE(obj.hasChanged());
  obj.loop(10*1000);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  TEST_ASSERT_FALSE(obj.hasChanged());
}


void test_dimCtrl_set(void){
  Ani obj("testObjANi");
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  obj.loop(0);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  obj.loop(1000);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM | 0x77,0,0,0,""));
  // value will be taken over at next loop
  TEST_ASSERT_EQUAL_UINT8( 0x0 , obj.getDim());
  TEST_ASSERT_FALSE(obj.hasChanged());
  obj.loop(1001);  
  TEST_ASSERT_TRUE(obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT8( 0x77 , obj.getDim());


  // test config without WR bit
  obj.config(AniCfg(0x66,0,0,0,""));
  // must be still the old value
  TEST_ASSERT_EQUAL_UINT8( 0x77 , obj.getDim());  
  obj.loop(2000);
  TEST_ASSERT_EQUAL_UINT8( 0x77 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM | 0x11,0,0,0,""));
  TEST_ASSERT_EQUAL_UINT8( 0x77 , obj.getDim());
  obj.loop(3000);
  TEST_ASSERT_EQUAL_UINT8( 0x11 , obj.getDim());
}

void test_dimCtrl_dimUp(void){
  Ani obj("testObjANi");
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  obj.loop(0);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
 
  AniCfg cfg;
  cfg.dimCfg.reg.WR_dim    = 1;
  cfg.dimCfg.reg.setValue  = 0x60;
  obj.config(cfg);
  // value will be taken over at next loop
  TEST_ASSERT_EQUAL_UINT8( 0x0 , obj.getDim());
  obj.loop(1001);  
  TEST_ASSERT_EQUAL_UINT8( 0x60 , obj.getDim());

  cfg.dimCfg.reg.WR_dim    = 1;
  cfg.dimCfg.reg.setValue  = 0x0;
  cfg.dimCfg.reg.incValue  = 0x10;
  obj.config(cfg);
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 0x70 , obj.getDim());
  
  cfg.dimCfg.reg.WR_dim    = 0;
  obj.config(cfg);
  obj.loop(2005);  
  TEST_ASSERT_EQUAL_UINT8( 0x70 , obj.getDim());

  cfg.dimCfg.reg.WR_dim    = 1;
  cfg.dimCfg.reg.incValue  = 0x20;
  obj.config(cfg);
  obj.loop(2010);  
  TEST_ASSERT_EQUAL_UINT8( 0x90 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3000);  
  TEST_ASSERT_EQUAL_UINT8( 0xA0 , obj.getDim());
  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3001);  
  TEST_ASSERT_EQUAL_UINT8( 0xB0 , obj.getDim());
  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3002);  
  TEST_ASSERT_EQUAL_UINT8( 0xC0 , obj.getDim());
  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3003);  
  TEST_ASSERT_EQUAL_UINT8( 0xD0 , obj.getDim());
  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3004);  
  TEST_ASSERT_EQUAL_UINT8( 0xE0 , obj.getDim());
  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3005);  
  TEST_ASSERT_EQUAL_UINT8( 0xF0 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3010);  
  TEST_ASSERT_EQUAL_UINT8( 0xFF , obj.getDim());  // limit on max 0xFF

  obj.config(AniCfg(ANI_WR_DIM |  0x1000,0,0,0,""));
  obj.loop(3011);  
  TEST_ASSERT_EQUAL_UINT8( 0xFF , obj.getDim());  // limit on max 0xFF

}

void test_dimCtrl_dimDown(void){
  Ani obj("testObjANi");
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
  obj.loop(0);
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());
 
  AniCfg cfg;
  cfg.dimCfg.byte.HHH = 0x80;
  cfg.dimCfg.byte.L   = 51;
  obj.config(cfg);
  // value will be taken over at next loop
  TEST_ASSERT_EQUAL_UINT8( 0x0 , obj.getDim());
  obj.loop(1001);  
  TEST_ASSERT_EQUAL_UINT8( 51 , obj.getDim());

  cfg.dimCfg.byte.H = -10;   
  cfg.dimCfg.byte.L = 0;   
  obj.config(cfg);
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 41 , obj.getDim());

  cfg.dimCfg.byte.H = -10;   
  cfg.dimCfg.byte.L = 0xFF;   
  obj.config(cfg);
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 31 , obj.getDim());

  cfg.dimCfg.reg.setValue = 11;
  cfg.dimCfg.reg.incValue = 0;
  obj.config(cfg);
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 11 , obj.getDim());

  cfg.dimCfg.reg.setValue = 0;
  cfg.dimCfg.reg.incValue = -10;
  obj.config(cfg);
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 1 , obj.getDim());

  obj.config(cfg);
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 0 , obj.getDim());

}

void test_colorCtrl_init(void){
  AniTest obj;
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) (COLOR_LIST | LOOP_STOP) , obj.pColorCtrl->getMode());
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  obj.loop(0);
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  obj.loop(10);
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  obj.loop(100);
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  obj.loop(1000);
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  obj.loop(10*1000);
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) (COLOR_LIST | LOOP_STOP) , obj.pColorCtrl->getMode());
}


void test_colorCtrl_triggerList(void){
  AniTest obj;
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) (COLOR_LIST | LOOP_STOP) , obj.pColorCtrl->getMode());
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  obj.loop(0);
  obj.loop(10);
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  
  AniCfg cfg;
  cfg.dimCfg.reg.WR_color   = 1;
  cfg.colorCfg.reg.t2_ms    = 0xFF; 
  cfg.colorCfg.reg.incValue = 0x01; 
  
  cfg.str = "0,1,2,3,4,5";
  /*
  	p2 : color Index:   xx xx xx xx   :
						|| || || ++---: start index of color list   (0..255)
						|| || ||      :     color list is provided in cfg.str
						|| || ||      :     if cfg.str is empty color are taken from color wheel (0..255)
						|| || ++------: == 0   : no inc / dec  = static
						|| ||         : <  128 : inc step
						|| ||         : >= 128 : dec step  (-1 = 255; -2 = 254)
						|| ++---------: time t2 in ms: time between two color steps 
						||            : 0xxxFF xxxx wait for trigger 
						++------------: event divider 0..255 = 1..256 (2 ==> 3 trigger or 3 time event until color change)			
  */
  TEST_ASSERT_EQUAL_INT( ANI_OK , obj.config(cfg));
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) (COLOR_LIST | LOOP_TRIGGER) , obj.pColorCtrl->getMode());
  TEST_ASSERT_EQUAL_UINT8(  5   , obj.pColorCtrl->getMaxIndex());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());  // frist value of new color cfg has no change to init value .. no update necessary
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());
  obj.loop(100);
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  obj.loop(1000);
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());

  obj.triggerColor();     // no change until next loop !!
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) (COLOR_LIST | LOOP_TRIGGER) , obj.pColorCtrl->getMode());
  TEST_ASSERT_EQUAL_UINT8(  5   , obj.pColorCtrl->getMaxIndex());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());

  obj.loop(1001);
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) (COLOR_LIST | LOOP_TRIGGER) , obj.pColorCtrl->getMode());
  TEST_ASSERT_EQUAL_UINT8(  5   , obj.pColorCtrl->getMaxIndex());
  TEST_ASSERT_TRUE(               obj.hasChanged());
  TEST_ASSERT_FALSE(              obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT32( 1   , obj.getColor());
  TEST_ASSERT_EQUAL_UINT8(  0   , obj.getDim());

  obj.loop(1002);
  TEST_ASSERT_FALSE(              obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT32( 1   , obj.getColor());
  TEST_ASSERT_EQUAL_UINT8(  0   , obj.getDim());

  obj.triggerColor();
  obj.loop(2000);
  TEST_ASSERT_TRUE(               obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT32( 2   , obj.getColor());
  TEST_ASSERT_EQUAL_UINT8(  0   , obj.getDim());

  obj.triggerColor();
  obj.loop(2001);
  TEST_ASSERT_TRUE(               obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT32( 3   , obj.getColor());
  TEST_ASSERT_EQUAL_UINT8(  0   , obj.getDim());

  obj.triggerColor();
  obj.loop(2002);
  TEST_ASSERT_TRUE(               obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT32( 4   , obj.getColor());
  TEST_ASSERT_EQUAL_UINT8(  0   , obj.getDim());


  obj.triggerColor();
  obj.loop(2003);
  TEST_ASSERT_TRUE(               obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT32( 5   , obj.getColor());
  TEST_ASSERT_EQUAL_UINT8(  0   , obj.getDim());


  obj.triggerColor();
  obj.loop(2004);
  TEST_ASSERT_TRUE(               obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT32( 0   , obj.getColor());
  TEST_ASSERT_EQUAL_UINT8(  0   , obj.getDim());


}


void test_colorCtrl_triggerListReverse(void){
  AniTest obj;
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) (COLOR_LIST | LOOP_STOP) , obj.pColorCtrl->getMode());
  
  AniCfg cfg;
  cfg.dimCfg.reg.WR_color     = 1;
  cfg.colorCfg.reg.t2_ms      = 0xFF; 
  cfg.colorCfg.reg.incValue   = -1;
  cfg.colorCfg.reg.startIndex = 2; 
  cfg.str = "0,1,2,3";
  /*
  	p2 : color Index:   xx xx xx xx   :
						|| || || ++---: start index of color list   (0..255)
						|| || ||      :     color list is provided in cfg.str
						|| || ||      :     if cfg.str is empty color are taken from color wheel (0..255)
						|| || ++------: == 0   : no inc / dec  = static
						|| ||         : <  128 : inc step
						|| ||         : >= 128 : dec step  (-1 = 255; -2 = 254)
						|| ++---------: time t2 in ms: time between two color steps 
						||            : 0xxxFF xxxx wait for trigger 
						++------------: event divider 0..255 = 1..256 (2 ==> 3 trigger or 3 time event until color change)			
  */
  TEST_ASSERT_EQUAL_INT( ANI_OK , obj.config(cfg));
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) (COLOR_LIST | LOOP_TRIGGER) , obj.pColorCtrl->getMode());
  TEST_ASSERT_EQUAL_UINT8(  3   , obj.pColorCtrl->getMaxIndex());
  // reset Values
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());

  // cfg values
  obj.loop(100);
  TEST_ASSERT_EQUAL_UINT32( 2, obj.getColor());
  TEST_ASSERT_TRUE(            obj.hasChanged());  // ==> cfg change 
  
  obj.triggerColor();     // no change until next loop !!
  obj.loop(101);
  TEST_ASSERT_TRUE(               obj.hasChanged());
  TEST_ASSERT_FALSE(              obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT32( 1   , obj.getColor());

  obj.triggerColor();     // no change until next loop !!
  obj.loop(102);
  TEST_ASSERT_EQUAL_UINT32( 0   , obj.getColor());

  obj.triggerColor();     // no change until next loop !!
  obj.loop(103);
  TEST_ASSERT_EQUAL_UINT32( 3   , obj.getColor());

  obj.triggerColor();     // no change until next loop !!
  obj.loop(104);
  TEST_ASSERT_EQUAL_UINT32( 2   , obj.getColor());

}

void test_colorCtrl_const(void){
  AniTest obj;
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) (COLOR_LIST | LOOP_STOP) , obj.pColorCtrl->getMode());
  
  AniCfg cfg;
  cfg.dimCfg.reg.WR_color     = 1;  
  cfg.colorCfg.uint32         = 0;   // const color 
  cfg.str = "0x0000 00FF";           // const blue
  TEST_ASSERT_EQUAL_INT( ANI_OK , obj.config(cfg));
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) (COLOR_LIST | LOOP_STOP) , obj.pColorCtrl->getMode());
  TEST_ASSERT_EQUAL_UINT8(  0   , obj.pColorCtrl->getMaxIndex());
  
  // reset Values
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());

  // cfg values
  obj.loop(100);
  TEST_ASSERT_EQUAL_UINT32( 0xFF, obj.getColor());
  TEST_ASSERT_TRUE(               obj.hasChanged());  // ==> cfg change 
  
  obj.loop(101);
  TEST_ASSERT_FALSE(              obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT32( 0xFF, obj.getColor());

  for(int i=0; i < 1000;i++){
    obj.loop(200+i*7);
    obj.triggerColor();     
    TEST_ASSERT_FALSE(              obj.hasChanged());
    TEST_ASSERT_EQUAL_UINT32( 0xFF, obj.getColor());
  }
  // const is const
}

void test_colorCtrl_timeList(void){
  AniTest obj;
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) (COLOR_LIST | LOOP_STOP) , obj.pColorCtrl->getMode());
  // reset values
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());


  AniCfg cfg;
  cfg.dimCfg.reg.WR_color     = 1;    // change color register content
  cfg.colorCfg.reg.incValue   = 1;    // on event inc/dec color index
  cfg.colorCfg.reg.startIndex = 0;    // start with color index 0
  cfg.colorCfg.reg.eventCounter = 0;  // inc/dec on each event
  cfg.colorCfg.reg.t2_ms      = 10;   // event: time event each 10ms
  cfg.str = "0x0000 00FF,0x0000 FF00,0x00FF 0000";       // color list with 3 entries   
  TEST_ASSERT_EQUAL_INT( ANI_OK       , obj.config(cfg));
  TEST_ASSERT_EQUAL_UINT8(  2         , obj.pColorCtrl->getMaxIndex());
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) (COLOR_LIST | LOOP_TIME) , obj.pColorCtrl->getMode());
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) ColorCtrl::initTime      , obj.pColorCtrl->getState());
  TEST_ASSERT_EQUAL_UINT32( 0x00000000, obj.getColor());
  TEST_ASSERT_FALSE(                    obj.hasChanged());
  // cfg does not change anything until first loop !!

  struct timeListResult_s{
    uint32_t    time;
    uint32_t    color;
    bool        hasChanged;
  };
  typedef struct timeListResult_s timeListResult_t;

  timeListResult_t res[] = {
    {100  , 0x000000FF  , true },
    {101  , 0x000000FF  , false },
    {102  , 0x000000FF  , false },
    {103  , 0x000000FF  , false },
    {104  , 0x000000FF  , false },
    {105  , 0x000000FF  , false },
    {106  , 0x000000FF  , false },
    {107  , 0x000000FF  , false },
    {108  , 0x000000FF  , false },
    {109  , 0x000000FF  , false },
    {110  , 0x0000FF00  , true },
    {111  , 0x0000FF00  , false },
    {112  , 0x0000FF00  , false },
    {113  , 0x0000FF00  , false },
    {114  , 0x0000FF00  , false },
    {115  , 0x0000FF00  , false },
    {116  , 0x0000FF00  , false },
    {117  , 0x0000FF00  , false },
    {118  , 0x0000FF00  , false },
    {119  , 0x0000FF00  , false },
    {120  , 0x00FF0000  , true },
    {121  , 0x00FF0000  , false },
    {122  , 0x00FF0000  , false },
    {123  , 0x00FF0000  , false },
    {124  , 0x00FF0000  , false },
    {125  , 0x00FF0000  , false },
    {126  , 0x00FF0000  , false },
    {127  , 0x00FF0000  , false },
    {128  , 0x00FF0000  , false },
    {129  , 0x00FF0000  , false },
    {130  , 0x000000FF  , true },
    {131  , 0x000000FF  , false },
    {132  , 0x000000FF  , false },
    {133  , 0x000000FF  , false },
    {134  , 0x000000FF  , false },
    {135  , 0x000000FF  , false },
    {136  , 0x000000FF  , false },
    {137  , 0x000000FF  , false },
    {138  , 0x000000FF  , false },
    {139  , 0x000000FF  , false },
    {140  , 0x0000FF00  , true },
  };

  int count = sizeof(res) / sizeof(timeListResult_t);
  uint32_t time,startTime;

  startTime = 0;
  for(int i=0;i< count;i++){
    time = startTime + res[i].time;
    obj.loop(time);
    TEST_ASSERT_EQUAL_UINT8( (uint8_t) ColorCtrl::waitTime  , obj.pColorCtrl->getState());
    TEST_ASSERT_EQUAL_INT( (int) res[i].hasChanged          , (int) obj.hasChanged());
    TEST_ASSERT_EQUAL_UINT32( res[i].color                  , obj.getColor());
  }

  TEST_ASSERT_EQUAL_INT( ANI_OK       , obj.config(cfg));
  startTime = 127;
  for(int i=0;i< count;i++){
    time = startTime + res[i].time;
    obj.loop(time);
    TEST_ASSERT_EQUAL_UINT8( (uint8_t) ColorCtrl::waitTime  , obj.pColorCtrl->getState());
    TEST_ASSERT_EQUAL_INT( (int) res[i].hasChanged          , (int) obj.hasChanged());
    TEST_ASSERT_EQUAL_UINT32( res[i].color                  , obj.getColor());
  }


  // same reverse
  cfg.dimCfg.reg.WR_color     = 1;  
  cfg.colorCfg.reg.incValue   = -1;  
  cfg.colorCfg.reg.startIndex = 0;
  cfg.colorCfg.reg.t2_ms      = 10;
  cfg.str = "0x0000 00FF,0x00FF 0000,0x0000 FF00";          
  TEST_ASSERT_EQUAL_INT( ANI_OK       , obj.config(cfg));
  TEST_ASSERT_EQUAL_UINT8(  2         , obj.pColorCtrl->getMaxIndex());
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) (COLOR_LIST | LOOP_TIME) , obj.pColorCtrl->getMode());
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) ColorCtrl::initTime      , obj.pColorCtrl->getState());
  TEST_ASSERT_EQUAL_UINT32( 0x0000FF00, obj.getColor());     // last selected color 
  TEST_ASSERT_FALSE_MESSAGE(  obj.hasChanged(),"init reverse");   // change has been checked / cleard in loop above

  startTime = 374;
  for(int i=0;i< count;i++){
    time = startTime + res[i].time;
    String msg="loop: "+String(i)+"   time: " +String(time) + "ms    nextLoopTime: "+String(obj.pColorCtrl->getNextLoopTime())+"ms    call now loop!";
    obj.loop(time);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE( ColorCtrl::waitTime  , obj.pColorCtrl->getState()  ,msg.c_str());
    TEST_ASSERT_EQUAL_INT_MESSAGE( res[i].hasChanged      , (int) obj.hasChanged()      ,msg.c_str());
    TEST_ASSERT_EQUAL_UINT32_MESSAGE( res[i].color        , obj.getColor()              ,msg.c_str());
  }

}

// now we call here all test collections
int runAllCollections(void) {
  UNITY_BEGIN();

  // dim ctrl
  RUN_TEST(test_dimCtrl_init);
  RUN_TEST(test_dimCtrl_set);
  RUN_TEST(test_dimCtrl_dimUp);
  RUN_TEST(test_dimCtrl_dimDown);

  // color ctrl
  RUN_TEST(test_colorCtrl_init);
  RUN_TEST(test_colorCtrl_triggerList);
  RUN_TEST(test_colorCtrl_triggerListReverse);
  RUN_TEST(test_colorCtrl_const);
  RUN_TEST(test_colorCtrl_timeList);



  return UNITY_END();
}



/**
  * For Arduino framework
  */
void setup() {
  // Wait ~2 seconds before the Unity test runner
  // establishes connection with a board Serial interface
  delay(6000);
  
  runAllCollections();

  pinMode(LED_BUILTIN,OUTPUT);
}


void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);

}


