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
  cfg._dimCfg.reg.WR_dim    = 1;
  cfg._dimCfg.reg.setValue  = 0x60;
  obj.config(cfg);
  // value will be taken over at next loop
  TEST_ASSERT_EQUAL_UINT8( 0x0 , obj.getDim());
  obj.loop(1001);  
  TEST_ASSERT_EQUAL_UINT8( 0x60 , obj.getDim());

  cfg._dimCfg.reg.WR_dim    = 1;
  cfg._dimCfg.reg.setValue  = 0x0;
  cfg._dimCfg.reg.incValue  = 0x10;
  obj.config(cfg);
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 0x70 , obj.getDim());
  
  cfg._dimCfg.reg.WR_dim    = 0;
  obj.config(cfg);
  obj.loop(2005);  
  TEST_ASSERT_EQUAL_UINT8( 0x70 , obj.getDim());

  cfg._dimCfg.reg.WR_dim    = 1;
  cfg._dimCfg.reg.incValue  = 0x20;
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
 

  obj.config(AniCfg(ANI_WR_DIM |  51,0,0,0,""));
  // value will be taken over at next loop
  TEST_ASSERT_EQUAL_UINT8( 0x0 , obj.getDim());
  obj.loop(1001);  
  TEST_ASSERT_EQUAL_UINT8( 51 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM |  ((-10)<<8),0,0,0,""));
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 41 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM |  ((-10)<<8),0,0,0,""));
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 31 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM |  11,0,0,0,""));
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 11 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM |  ((-10)<<8),0,0,0,""));
  obj.loop(2000);  
  TEST_ASSERT_EQUAL_UINT8( 1 , obj.getDim());

  obj.config(AniCfg(ANI_WR_DIM |  ((-10)<<8),0,0,0,""));
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
  cfg._dimCfg.reg.WR_color   = 1;
  cfg._colorCfg.reg.t2_ms    = 0xFF; 
  cfg._colorCfg.reg.incValue = 0x01; 
  
  cfg._str = "0,1,2,3,4,5";
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
  TEST_ASSERT_EQUAL_UINT8(  6   , obj.pColorCtrl->getMaxIndex());
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
  TEST_ASSERT_EQUAL_UINT8(  6   , obj.pColorCtrl->getMaxIndex());
  TEST_ASSERT_EQUAL_UINT32( 0, obj.getColor());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  TEST_ASSERT_FALSE(           obj.hasChanged());
  TEST_ASSERT_EQUAL_UINT8(  0, obj.getDim());

  obj.loop(1001);
  TEST_ASSERT_EQUAL_UINT8(  (uint8_t) (COLOR_LIST | LOOP_TRIGGER) , obj.pColorCtrl->getMode());
  TEST_ASSERT_EQUAL_UINT8(  6   , obj.pColorCtrl->getMaxIndex());
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
  cfg._dimCfg.reg.WR_color     = 1;
  cfg._colorCfg.reg.t2_ms      = 0xFF; 
  cfg._colorCfg.reg.incValue   = -1;
  cfg._colorCfg.reg.startIndex = 2; 
  cfg._str = "0,1,2,3";
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
  TEST_ASSERT_EQUAL_UINT8(  4   , obj.pColorCtrl->getMaxIndex());
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

// now we call here all test collections
int runAllCollections(void) {
  UNITY_BEGIN();
  RUN_TEST(test_dimCtrl_init);
  RUN_TEST(test_dimCtrl_set);
  RUN_TEST(test_dimCtrl_dimUp);
  RUN_TEST(test_dimCtrl_dimDown);

  RUN_TEST(test_colorCtrl_init);
  RUN_TEST(test_colorCtrl_triggerList);
  RUN_TEST(test_colorCtrl_triggerListReverse);


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


