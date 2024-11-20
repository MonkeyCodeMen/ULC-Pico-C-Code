#include <Arduino.h>
#include <Debug.hpp>
#include <helper.h>

#include <SPI.h>
#include <BufferedClock.hpp>
#include <Adafruit_NeoMatrix.h>
#include <SDcard.hpp>
#include <TFT_eSPI.h> // Hardware-specific library
#include "Led.hpp"
#include "Ani.hpp"
#include "StringList.hpp"

#include <unity.h>




struct timeListResult_s{
  uint32_t    time;
  bool        triggerColor,triggerFlash;
  uint8_t     dim;
  uint32_t    color;
  bool        hasChanged;
};
typedef struct timeListResult_s timeListResult_t;

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
  TEST_ASSERT_TRUE(obj.hasChanged());
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
  TEST_ASSERT_TRUE(obj.hasChanged());
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
  TEST_ASSERT_TRUE(            obj.hasChanged());
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
  TEST_ASSERT_TRUE(            obj.hasChanged());
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
  TEST_ASSERT_TRUE(            obj.hasChanged());  
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

void test_colorCtrl_wheel(void){
  AniTest obj;
  AniCfg cfg;

  obj.loop(0);
  cfg.dimCfg.reg.WR_color     = 1;
  cfg.colorCfg.reg.t2_ms      = 0xFF; 
  cfg.colorCfg.reg.incValue   = 1; 
  cfg.colorCfg.reg.startIndex = 0;
  cfg.str = "";
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
  TEST_ASSERT_EQUAL_INT(  COLOR_WHEEL | LOOP_TRIGGER , obj.pColorCtrl->getMode());
  TEST_ASSERT_EQUAL_UINT32( 0                   , obj.getColor());
  TEST_ASSERT_TRUE(                               obj.hasChanged());  
  TEST_ASSERT_EQUAL_UINT8(  0                   , obj.getDim());
  TEST_ASSERT_EQUAL_INT( ColorCtrl::waitTrigger , obj.pColorCtrl->getState());
  
  uint8_t index=0;
  uint32_t sollColor;
  uint32_t startTime=100;
  for(int i=0; i < 500;i++ ){
    String msg="loop: "+String(i);
    sollColor = getColorWheel24Bit(index);
    obj.loop(startTime+i);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE( sollColor , obj.getColor(),msg.c_str());
    index++;
    obj.triggerColor();
  }
}

void test_colorCtrl_copyWheel(void){
  AniTest obj;
  AniCfg cfg;

  obj.loop(0);
  cfg.dimCfg.reg.WR_color     = 1;
  cfg.colorCfg.reg.t2_ms      = 0xFF; 
  cfg.colorCfg.reg.incValue   = 1; 
  cfg.colorCfg.reg.startIndex = 0;
  cfg.str = "";

  ColorCtrl colorCopy = *obj.pColorCtrl;
  
  uint8_t index=0;
  uint32_t startTime=100;
  for(int i=0; i < 500;i++ ){
    String msg="loop: "+String(i);
    colorCopy.loop(startTime+i);
    obj.pColorCtrl->loop(startTime+i);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE( obj.pColorCtrl->getColor() , colorCopy.getColor(),msg.c_str());
    index++;
    colorCopy.trigger();
    obj.pColorCtrl->trigger();
  }
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
  TEST_ASSERT_TRUE(            obj.hasChanged());
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
  TEST_ASSERT_TRUE(            obj.hasChanged());
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
  TEST_ASSERT_TRUE(            obj.hasChanged());
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
  TEST_ASSERT_TRUE(                     obj.hasChanged());
  // cfg does not change anything until first loop !!


  timeListResult_t res[] = {
    {100  , false, false, 0, 0x000000FF  , true },
    {101  , false, false, 0, 0x000000FF  , false },
    {102  , false, false, 0, 0x000000FF  , false },
    {103  , false, false, 0, 0x000000FF  , false },
    {104  , false, false, 0, 0x000000FF  , false },
    {105  , false, false, 0, 0x000000FF  , false },
    {106  , false, false, 0, 0x000000FF  , false },
    {107  , false, false, 0, 0x000000FF  , false },
    {108  , false, false, 0, 0x000000FF  , false },
    {109  , false, false, 0, 0x000000FF  , false },
    {110  , false, false, 0, 0x0000FF00  , true },
    {111  , false, false, 0, 0x0000FF00  , false },
    {112  , false, false, 0, 0x0000FF00  , false },
    {113  , false, false, 0, 0x0000FF00  , false },
    {114  , false, false, 0, 0x0000FF00  , false },
    {115  , false, false, 0, 0x0000FF00  , false },
    {116  , false, false, 0, 0x0000FF00  , false },
    {117  , false, false, 0, 0x0000FF00  , false },
    {118  , false, false, 0, 0x0000FF00  , false },
    {119  , false, false, 0, 0x0000FF00  , false },
    {120  , false, false, 0, 0x00FF0000  , true },
    {121  , false, false, 0, 0x00FF0000  , false },
    {122  , false, false, 0, 0x00FF0000  , false },
    {123  , false, false, 0, 0x00FF0000  , false },
    {124  , false, false, 0, 0x00FF0000  , false },
    {125  , false, false, 0, 0x00FF0000  , false },
    {126  , false, false, 0, 0x00FF0000  , false },
    {127  , false, false, 0, 0x00FF0000  , false },
    {128  , false, false, 0, 0x00FF0000  , false },
    {129  , false, false, 0, 0x00FF0000  , false },
    {130  , false, false, 0, 0x000000FF  , true },
    {131  , false, false, 0, 0x000000FF  , false },
    {132  , false, false, 0, 0x000000FF  , false },
    {133  , false, false, 0, 0x000000FF  , false },
    {134  , false, false, 0, 0x000000FF  , false },
    {135  , false, false, 0, 0x000000FF  , false },
    {136  , false, false, 0, 0x000000FF  , false },
    {137  , false, false, 0, 0x000000FF  , false },
    {138  , false, false, 0, 0x000000FF  , false },
    {139  , false, false, 0, 0x000000FF  , false },
    {140  , false, false, 0, 0x0000FF00  , true }
  };

  int count = sizeof(res) / sizeof(timeListResult_t);
  uint32_t time,startTime;

  startTime = 0;
  for(int i=0;i< count;i++){
    time = startTime + res[i].time;
    obj.loop(time);
    TEST_ASSERT_EQUAL_UINT8( (uint8_t) ColorCtrl::waitTime  , obj.pColorCtrl->getState()  );
    TEST_ASSERT_EQUAL_INT(    res[i].hasChanged             , obj.hasChanged()            );
    TEST_ASSERT_EQUAL_UINT32( res[i].color                  , obj.getColor()              );
    TEST_ASSERT_EQUAL_UINT8(  res[i].dim                    , obj.getDim()                );

  }

  TEST_ASSERT_EQUAL_INT( ANI_OK       , obj.config(cfg));
  startTime = 127;
  for(int i=0;i< count;i++){
    time = startTime + res[i].time;
    obj.loop(time);
    TEST_ASSERT_EQUAL_UINT8( (uint8_t) ColorCtrl::waitTime  , obj.pColorCtrl->getState());
    TEST_ASSERT_EQUAL_INT(    res[i].hasChanged             , obj.hasChanged()            );
    TEST_ASSERT_EQUAL_UINT32( res[i].color                  , obj.getColor()              );
    TEST_ASSERT_EQUAL_UINT8(  res[i].dim                    , obj.getDim()                );
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
  TEST_ASSERT_TRUE_MESSAGE(  obj.hasChanged(),"init reverse");   // change has been checked / cleard in loop above

  startTime = 374;
  for(int i=0;i< count;i++){
    time = startTime + res[i].time;
    String msg="loop: "+String(i)+"   time: " +String(time) + "ms    nextLoopTime: "+String(obj.pColorCtrl->getNextLoopTime())+"ms    call now loop!";
    obj.loop(time);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE( ColorCtrl::waitTime  , obj.pColorCtrl->getState()  ,msg.c_str());
    TEST_ASSERT_EQUAL_INT(    res[i].hasChanged             , obj.hasChanged()            );
    TEST_ASSERT_EQUAL_UINT32( res[i].color                  , obj.getColor()              );
    TEST_ASSERT_EQUAL_UINT8(  res[i].dim                    , obj.getDim()                );
  }

}

void test_flashCtrl_init(void){
  AniTest obj;
  TEST_ASSERT_EQUAL_INT( FlashCtrl::stop , obj.pFlashCtrl->getState());

  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_TRUE(            obj.hasChanged());
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

  TEST_ASSERT_EQUAL_INT( FlashCtrl::stop , obj.pFlashCtrl->getState());
}


void test_flashCtrl_trigger(void){
  AniTest obj;
  AniCfg cfg;
  cfg.dimCfg.reg.WR_dim   = 1;
  cfg.dimCfg.reg.WR_color = 1;
  cfg.dimCfg.reg.WR_flash = 1;
  
  cfg.dimCfg.reg.setValue = 77;
  cfg.colorCfg.reg.eventCounter = 0;
  cfg.colorCfg.reg.incValue     = 1;
  cfg.colorCfg.reg.startIndex   = 0;
  cfg.colorCfg.reg.t2_ms        = 0xFF;   // change color on trigger  (provided by flash ctrl)
  cfg.str = "0,80,1,81,2,82";

  cfg.flashCfg.reg.flashPerGroup = 3;
  cfg.flashCfg.reg.t1_10ms       = 1;     // flash 10 ms
  cfg.flashCfg.reg.t2_10ms       = 10;    // time between flashes 100ms
  cfg.flashCfg.reg.t3_100ms      = 0xFF;  // wait for trigger to start flash group 

  TEST_ASSERT_EQUAL_INT( FlashCtrl::stop , obj.pFlashCtrl->getState());
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_TRUE(            obj.hasChanged());
  obj.loop(0);
  TEST_ASSERT_EQUAL_INT( FlashCtrl::stop , obj.pFlashCtrl->getState());
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());  

  obj.config(cfg);
  TEST_ASSERT_EQUAL_INT( FlashCtrl::init , obj.pFlashCtrl->getState());  // init state selected but not processed until now
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_TRUE(            obj.hasChanged());  
  obj.loop(1);      // init state processed
  TEST_ASSERT_EQUAL_INT( FlashCtrl::waitTrigger , obj.pFlashCtrl->getState());  // init state selected but not processed until now
  TEST_ASSERT_EQUAL_UINT8( 77, obj.getDim());
  TEST_ASSERT_EQUAL_UINT32(0 , obj.getColor());
  TEST_ASSERT_TRUE(            obj.hasChanged());  // dim has changed !! (from black 0 % to black 77/255 %)

  timeListResult_t res[] = {
    { 100  , false, false, 77, 0,  false },
    { 101  , false, false, 77, 0,  false },
    { 109  , false, false, 77, 0,  false },
    { 110  , false, true,  77, 80, true  },
    { 111  , false, false, 77, 80, false },
    { 112  , false, false, 77, 80, false },
    { 118  , false, false, 77, 80, false },
    { 119  , false, false, 77, 80, false },
    { 120  , false, false, 77, 1,  true  },
    { 121  , false, false, 77, 1,  false },
    { 122  , false, false, 77, 1,  false },
    { 219  , false, false, 77, 1,  false },
    { 220  , false, false, 77, 81, true  },
    { 221  , false, false, 77, 81, false },
    { 228  , false, false, 77, 81, false },
    { 229  , false, false, 77, 81, false },
    { 230  , false, false, 77, 2,  true  },
    { 231  , false, false, 77, 2,  false },
    { 329  , false, false, 77, 2,  false },
    { 330  , false, false, 77, 82, true  },
    { 331  , false, false, 77, 82, false },
    { 339  , false, false, 77, 82, false },
    { 340  , false, false, 77, 0,  true  },   // wait for new trigger
    { 341  , false, false, 77, 0,  false },
    { 499  , false, false, 77, 0,  false },  
    { 500  , false, true,  77, 80, true  },
    { 509  , false, false, 77, 80, false },
    { 510  , false, false, 77, 1,  true  }
 
  };  

  int count = sizeof(res) / sizeof(timeListResult_t);
  uint32_t time,startTime;

  startTime = 0;
  for(int i=0;i< count;i++){
    time = startTime + res[i].time;
    if ( res[i].triggerColor == true ) { obj.triggerColor(); }
    if ( res[i].triggerFlash == true ) { obj.triggerFlash(); }
    obj.loop(time);
    String msg="loop: "+String(i)+"   time: " +String(time) + "ms ";
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(    res[i].hasChanged       , obj.hasChanged()   , msg.c_str()  );
    TEST_ASSERT_EQUAL_UINT32_MESSAGE( res[i].color            , obj.getColor()     , msg.c_str()  );
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(  res[i].dim              , obj.getDim()       , msg.c_str()  );
  }  
}

void test_flashCtrl_time(void){
  AniTest obj;
  AniCfg cfg;
  cfg.dimCfg.reg.WR_dim   = 1;
  cfg.dimCfg.reg.WR_color = 1;
  cfg.dimCfg.reg.WR_flash = 1;
  
  cfg.dimCfg.reg.setValue = 77;
  cfg.colorCfg.reg.eventCounter = 0;
  cfg.colorCfg.reg.incValue     = 1;
  cfg.colorCfg.reg.startIndex   = 0;
  cfg.colorCfg.reg.t2_ms        = 0xFF;   // change color on trigger  (provided by flash ctrl)
  cfg.str = "0,80,1,81,2,82";

  cfg.flashCfg.reg.flashPerGroup = 3;
  cfg.flashCfg.reg.t1_10ms       = 1;     // flash 10 ms
  cfg.flashCfg.reg.t2_10ms       = 10;    // time between flashes 100ms
  cfg.flashCfg.reg.t3_100ms      = 10;    // 1Sec time between flahs groups

  obj.loop(0);
  obj.config(cfg);
  TEST_ASSERT_EQUAL_INT( FlashCtrl::init , obj.pFlashCtrl->getState());  // init state selected but not processed until now
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_TRUE(            obj.hasChanged());  
  obj.loop(1000);      // init state processed
  TEST_ASSERT_EQUAL_INT( FlashCtrl::pause , obj.pFlashCtrl->getState());  // init state selected but not processed until now
  TEST_ASSERT_EQUAL_UINT8( 77, obj.getDim());
  TEST_ASSERT_EQUAL_UINT32(0 , obj.getColor());
  TEST_ASSERT_TRUE(            obj.hasChanged());  // dim has changed !! (from black 0 % to black 77/255 %)

  timeListResult_t res[] = {
    { 1001  , false, true,  77, 0,  false },
    { 1499  , false, true,  77, 0,  false },
    { 1500  , false, false, 77, 0,  false }, // trigger has no influence in time mode
    { 1999  , false, true,  77, 0,  false },
    { 2000  , false, false, 77, 80, true  },
    { 2009  , false, false, 77, 80, false },
    { 2010  , false, false, 77, 1,  true  },
    { 2109  , false, false, 77, 1,  false },
    { 2110  , false, false, 77, 81, true  },
    { 2119  , false, false, 77, 81, false },
    { 2120  , false, false, 77, 2,  true  },
    { 2219  , false, false, 77, 2,  false },
    { 2220  , false, false, 77, 82, true  },
    { 2229  , false, false, 77, 82, false },
    { 2230  , false, false, 77, 0,  true  },
    { 3000  , false, false, 77, 0,  false },
    { 3229  , false, false, 77, 0,  false },
    { 3230  , false, false, 77, 80, true  },
    { 3239  , false, false, 77, 80, false },
    { 3240  , false, false, 77, 1,  true  },
    { 3339  , false, false, 77, 1,  false },
    { 3340  , false, false, 77, 81, true  }
  };  

  int count = sizeof(res) / sizeof(timeListResult_t);
  uint32_t time,startTime;

  startTime = 0;
  for(int i=0;i< count;i++){
    time = startTime + res[i].time;
    if ( res[i].triggerColor == true ) { obj.triggerColor(); }
    if ( res[i].triggerFlash == true ) { obj.triggerFlash(); }
    obj.loop(time);
    String msg="loop: "+String(i)+"   time: " +String(time) + "ms ";
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(    res[i].hasChanged       , obj.hasChanged()   , msg.c_str()  );
    TEST_ASSERT_EQUAL_UINT32_MESSAGE( res[i].color            , obj.getColor()     , msg.c_str()  );
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(  res[i].dim              , obj.getDim()       , msg.c_str()  );
  }  
}



// now we call here all test collections
int runAllTests(void) {
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
  RUN_TEST(test_colorCtrl_wheel);

  // color copy
  RUN_TEST(test_colorCtrl_copyWheel);

  // flash ctrl
  RUN_TEST(test_flashCtrl_init);
  RUN_TEST(test_flashCtrl_trigger);
  RUN_TEST(test_flashCtrl_time);

  /*
    ToDo
    + color ctrl obj copy test
    + breath test init
    + breath test time

  */



  return UNITY_END();
}



/**
  * For Arduino framework
  */
#include <MainConfig.h>
#include <Blink.hpp>
BlinkingLED  blink = BlinkingLED(LED_BUILTIN);
std::vector<uint32_t> testBlinkSeq = BLINK_SEQ_TEST;

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}


void setup() {
  blink.on();

  // Wait ~2 seconds before the Unity test runner
  // establishes connection with a board Serial interface
  delay(WAIT_FOR_UINTY_FRAMEWORK);

  runAllTests();

  blink.setup(testBlinkSeq);
}


void loop() {
  uint32_t now = millis();
  blink.loop(now);  
}


