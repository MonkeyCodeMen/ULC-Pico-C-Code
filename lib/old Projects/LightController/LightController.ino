/*
   universal light controller based on node CPU with small oled display
   rotary with push button, state machine, sound input 

   and last but not least ws2812FX lib ... a beatifull piece of code

   OLED display two lines:
   top line = headline (topic) 
   top right side Flag indication
   bot line value (change)

   one push with rotary   ==> next topic / menue entry
   double push ==> switch flag (in config write start config to EEPROM)
   long push ==> switch off

   Menue:

   mode => brightness => color => speed => config => mode

   
  
*/

/*
 * include libs
 */
#include <Arduino.h>
#include <U8g2lib.h>
#include <WS2812FX.h>
#include <OneButton.h>
#include <StateMachine.h>
//#define ENCODER_DO_NOT_USE_INTERRUPTS   // interrupt modus does not work on ESP8266 polling mode ist ok for user hand inputs
//#include <Encoder.h>  
#include <RotaryEncoder.h>  
#include <EEPROM.h>


#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

/*
 *    define IO PINS
 */
#define PIN_LEDS_DO       2
#define PIN_DISPLAY_CLK   5
#define PIN_DISPLAY_DATA  4
#define PIN_DISPLAY_RESET 16
#define PIN_BUTTON        0
#define PIN_ROTARY_CLK    12
#define PIN_ROTARY_DT     13



///////////////////////////////////
// EEPROM functions
///////////////////////////////////


/*
 * EEPROM Layout:
 * 
 * Adr    | len | data
 * 0x0000   4     magic number 0xdeadbeef
 * 0x0004   4     version  0.0.0.2
 * 0x0008   1     length strip 0
 * 0x0009   1     length strip 1
 * 0x000A   1     length strip 2
 * 0x000B   1     length strip 3
 * 0x000C   1     init mode                
 * 0x000D   1     init color
 * 0x000E   1     init bright
 * 0x000F   1     init speed
 * 
 */

#define EE_MAGIC_NUMBER   0xDEADBEEF
#define EE_LAYOUT_VERSION 0x00000200

#define EE_ADR_MAGIC      0x0000
#define EE_ADR_VERSION    0x0004
#define EE_ADR_LENGTH_0   0x0008
#define EE_ADR_LENGTH_1   0x0009
#define EE_ADR_LENGTH_2   0x000A
#define EE_ADR_LENGTH_3   0x000B
#define EE_ADR_MODE       0x000C
#define EE_ADR_COLOR      0x000D
#define EE_ADR_BRIGHT     0x000E
#define EE_ADR_SPEED      0x000F


u32_t EE_magicNumber;
u32_t EE_version;
u8_t EE_length0;
u8_t EE_length1;
u8_t EE_length2;
u8_t EE_length3;
u8_t EE_mode;
u8_t EE_color;
u8_t EE_bright;
u8_t EE_speed;


u32_t EE_readu32(u32_t adr){
  Serial.println("EE_readu32");
  u32_t val = 0;
  val |= u32_t(EEPROM.read(adr+0) & 0xFF) << 24;
  val |= u32_t(EEPROM.read(adr+1) & 0xFF) << 16;
  val |= u32_t(EEPROM.read(adr+2) & 0xFF) <<  8;
  val |= u32_t(EEPROM.read(adr+3) & 0xFF) <<  0;
  return val;
}

// attention only write no commit !!!!
void EE_writeu32(u32_t adr,u32_t value){
  Serial.println("EE_writeu32");
  EEPROM.write(adr+0,(value >> 24) & 0xFF);
  EEPROM.write(adr+1,(value >> 16) & 0xFF);
  EEPROM.write(adr+2,(value >>  8) & 0xFF);
  EEPROM.write(adr+3,(value >>  0) & 0xFF);
  yield();
  EEPROM.commit();
  yield();
}

void EE_writeAll(){
  Serial.println("EE_writeAll");
  EE_writeu32(EE_ADR_MAGIC,EE_magicNumber);
  EE_writeu32(EE_ADR_VERSION,EE_version);
 
  EEPROM.write(EE_ADR_LENGTH_0,EE_length0);
  EEPROM.write(EE_ADR_LENGTH_1,EE_length1);
  EEPROM.write(EE_ADR_LENGTH_2,EE_length2);
  EEPROM.write(EE_ADR_LENGTH_3,EE_length3);
  EEPROM.write(EE_ADR_MODE,EE_mode);
  EEPROM.write(EE_ADR_COLOR,EE_color);
  EEPROM.write(EE_ADR_BRIGHT,EE_bright);
  EEPROM.write(EE_ADR_SPEED,EE_speed);
  yield();
  EEPROM.commit();
  yield();
}

void EE_readAll(){
  Serial.println("EE_readAll");
  EE_length0 = EEPROM.read(EE_ADR_LENGTH_0);
  EE_length1 = EEPROM.read(EE_ADR_LENGTH_1);
  EE_length2 = EEPROM.read(EE_ADR_LENGTH_2);
  EE_length3 = EEPROM.read(EE_ADR_LENGTH_3);
  EE_mode    = EEPROM.read(EE_ADR_MODE);
  EE_color   = EEPROM.read(EE_ADR_COLOR);
  EE_bright  = EEPROM.read(EE_ADR_BRIGHT);
  EE_speed   = EEPROM.read(EE_ADR_SPEED);
}

void EE_factoryResetValues(){
  Serial.println("EE_factoryResetValues");
  // init values
  EE_magicNumber= EE_MAGIC_NUMBER;
  EE_version    = EE_LAYOUT_VERSION;
  EE_length0    = 60;
  EE_length1    = 0;
  EE_length2    = 0;
  EE_length3    = 0;
  EE_mode       = FX_MODE_STATIC;
  EE_color      = 1;      // white
  EE_bright     = 50;     // 50%
  EE_speed      = 50;     // 50%

  
}

void EE_doFactoryReset(){
  Serial.println("EE_doFactoryReset");
  EE_factoryResetValues();
  
  // write to EEPROM
  EE_writeu32(EE_ADR_MAGIC,EE_magicNumber);
  EE_writeu32(EE_ADR_VERSION,EE_version);
  EE_writeAll();    // does commit for us
}

void EE_init(){
  Serial.println("EE_init");
  EEPROM.begin(512);  //Initialize EEPROM
  EE_magicNumber = EE_readu32(EE_ADR_MAGIC);
  EE_version     = EE_readu32(EE_ADR_VERSION);

  if ((EE_magicNumber != EE_MAGIC_NUMBER) || (EE_version != EE_LAYOUT_VERSION)){
    EE_doFactoryReset();
  } else {
    EE_readAll();
  }
  Serial.flush();
}

///////////////////////////////////
// create static LIB objects
///////////////////////////////////
//StateMachine                            machine = StateMachine();
//RotaryEncoder                           encoder(PIN_ROTARY_DT, PIN_ROTARY_CLK, RotaryEncoder::LatchMode::TWO03);
//Encoder                               encoder(ENCODER_DT,ENCODER_CLK);
//long altePosition = -999;  // Definition der "alten" Position (Diese fiktive alte Position wird benötigt, damit die aktuelle Position später im seriellen Monitor nur dann angezeigt wird, wenn wir den Rotary Head bewegen)

/**  DEBUG Class mainly for early init serial interface to enable debug prints during constructor phase 
 *  but the debug print is although been used in setup and loop function
 *  
 */
#define DEBUG_PRINT
#define DEBUG_BAUD_RATE         115200
#define DEBUG_LOOP_DELAY_TIME   1  
class Debug{    
#ifdef DEBUG_PRINT
  #define DEBUG_PRINT(...)    Serial.print(__VA_ARGS__)
  #define DEBUG_PRINTLN(...)  Serial.println(__VA_ARGS__)
  #define DEBUG_FLUSH()       Serial.flush()
  #define DEBUG_DELAY()       delay(DEBUG_LOOP_DELAY_TIME)
#else
  #define DEBUG_PRINT(...)
  #define DEBUG_PRINTLN(...)
  #define DEBUG_FLUSH()       
  #define DEBUG_DELAY()      
#endif

  
  private:


  public:
    Debug(){
      Serial.begin(DEBUG_BAUD_RATE);  // do this in all cases (define switches)
      DEBUG_PRINTLN("---Debug::Debug()");
      DEBUG_FLUSH();     
   }

    void init(){
      DEBUG_PRINTLN("---Debug::init()");
      DEBUG_FLUSH();     
    }

    void run(){
      //DEBUG_PRINT("---Debug::run()  milis:");
      //DEBUG_PRINTLN(millis(),DEC);
      DEBUG_FLUSH();  
      DEBUG_DELAY();   
    }
};
Debug  debug;


/** App class Leds
 * 
 * handle the RGB stripe by using the LIB WS2812FX
 * including functionality for select/change/remeber : mode, brightness, color, speed (music controlled)
 * 
 */
//#define LEDS_DEBUG
#define LEDS_MAX_MODE           FX_MODE_TRICOLOR_CHASE
#define LEDS_COUNT              16      
#define LEDS_MIN_BRIGHTNESS     10
#define LEDS_MAX_BRIGHTNESS     255
#define LEDS_SPEED_INC          5
#define LEDS_BRIGTHNESS_INC     5

typedef struct {
  char  *   name;
  u32_t  value;
}ColorListEntry;

const ColorListEntry colorList[] {  
                                  {"black",   BLACK},
                                  {"white",   WHITE},
                                  {"red",     RED},
                                  {"green",   GREEN},
                                  {"blue",    BLUE},
                                  {"yellow",  YELLOW},
                                  {"cyan",    CYAN},
                                  {"magenta", MAGENTA},
                                  {"pruple",  PURPLE},
                                  {"orange",  ORANGE},
                                  {"pink",    PINK},
                                  {"gray",    GRAY} 
                               };
         


class Leds{
#ifdef LEDS_DEBUG
  #define LEDS_PRINT(...)   Serial.print(__VA_ARGS__)
  #define LEDS_PRINTLN(...) Serial.println(__VA_ARGS__)
  #define LEDS_FLUSH()      Serial.flush()
#else
  #define LEDS_PRINT(...)
  #define LEDS_PRINTLN(...)
  #define LEDS_FLUSH() 
#endif
  
  private:
    WS2812FX *  mp_ws2812fx = NULL; 
    int         m_mode;         // modedef from WS2812FX
    String      m_modeFlag;     // ["","CYCLE"]   mode modification (by music)
    String      m_speedFlag;    // ["","MUSIC"]
    String      m_brightnessFlag;    // ["","MUSIC"]
    String      m_colorFlag;    // ["","MUSIC"]
    int         m_color;        // [0..m_colorCount[
    int         m_speed;        // [0..100]
    int         m_brightness;   // [0..100]
    int         m_colorCount;   // number of entries of colorList




    void updateAll(){
      mp_ws2812fx->setMode(m_mode);
      mp_ws2812fx->setColor(colorList[m_color].value);
      mp_ws2812fx->setBrightness(map(m_brightness,0,100,0,255));
      mp_ws2812fx->setSpeed(map(m_speed,0,100,5000,50));
    }


  public:
    Leds(){
      LEDS_PRINTLN("---Leds::Leds()");
      LEDS_FLUSH();

      m_modeFlag = "";
      m_speedFlag = "";
      m_colorFlag = "";
      m_brightnessFlag = "";
      m_colorCount = sizeof(colorList) / sizeof(ColorListEntry);
    }

    void init(){
      LEDS_PRINTLN("---Leds::init()");
      LEDS_FLUSH();

      m_mode    = EE_mode;
      m_color   = EE_color;   // blue
      m_speed   = EE_speed;
      m_brightness = EE_bright;

      if (mp_ws2812fx != NULL){
        mp_ws2812fx->stop();
        delete mp_ws2812fx;
      }  
      mp_ws2812fx = new WS2812FX(EE_length0, PIN_LEDS_DO, NEO_GRB + NEO_KHZ800);
      mp_ws2812fx->init();
      updateAll();
      mp_ws2812fx->start();
    }

    void start(){
      updateAll();
      mp_ws2812fx->start();
    }

    void run(){
      //LEDS_PRINTLN("---Leds::run()");
      mp_ws2812fx->service();
    }

    void switchOff(){
      mp_ws2812fx->stop();
    }


    /*
     * mode 
     */

    u8_t getMode(){
      return m_mode;
    }

    const char * getModeFlag(){
      return m_modeFlag.c_str();
    }

    const char * getModeName(u8_t mode){
      LEDS_PRINT("---Leds::getModeName(");
      LEDS_PRINT(mode,DEC);
      LEDS_PRINTLN(")");
      return (const char *) mp_ws2812fx->getModeName(mode);
    }

    const char * getModeName(){
      LEDS_PRINT("---Leds::getModeName()   current mode:");
      LEDS_PRINTLN(m_mode,DEC);
      return (const char *) mp_ws2812fx->getModeName(m_mode);
    }



    const char * toggleModeFlag(){
      LEDS_PRINT("---Leds::toggleModeFlag()   current Flag:");
      LEDS_PRINT(m_modeFlag.c_str());

      if (m_modeFlag == "CYCLE"){
        m_modeFlag = "";
        LEDS_PRINTLN("   new FLAG: ___ ");
      } else {
        m_modeFlag = "CYCLE";
        LEDS_PRINTLN("   new FLAG: CYCLE ");
      }
      return m_modeFlag.c_str();
    }

    u8_t incMode(int inc){
      LEDS_PRINT("---Leds::incMode(");
      LEDS_PRINT(inc,DEC);
      LEDS_PRINT(")  new mode :");
      
      m_mode += inc;
      if (m_mode > LEDS_MAX_MODE){
        m_mode = 0;
      }
      if (m_mode < 0){
        m_mode = LEDS_MAX_MODE;
      }
      
      LEDS_PRINTLN(m_mode,DEC);
      updateAll();
      return m_mode;
    }



    /*
     * Speed
     */
    u8_t getSpeed(){
      return m_speed;
    }

    const char * getSpeedFlag(){
      return m_speedFlag.c_str();
    }


    u8_t incSpeed(int inc){
      LEDS_PRINT("---Leds::incSpeed(");
      LEDS_PRINT(inc,DEC);
      LEDS_PRINT(")  new speed :");
      
      m_speed += inc * LEDS_SPEED_INC;
      if (m_speed > 100){
        m_speed = 0;
      }
      if (m_speed < 0){
        m_speed = 100;
      }
      
      LEDS_PRINTLN(m_speed,DEC);
      updateAll();
      return m_speed;
    }

    const char * toggleSpeedFlag(){
      LEDS_PRINT("---Leds::toggleSpeedFlag()   current Flag:");
      LEDS_PRINT(m_speedFlag.c_str());

      if (m_speedFlag == "MUSIC"){
        m_speedFlag = "";
        LEDS_PRINTLN("   new FLAG: ___ ");
      } else {
        m_speedFlag = "MUSIC";
        LEDS_PRINTLN("   new FLAG: CYCLE ");
      }
      return m_speedFlag.c_str();
    }


    /*
     * Brightness
     */

    u8_t getBrightness(){
      return m_brightness;
    }

    const char * getBrightnessFlag(){
      return m_brightnessFlag.c_str();
    }

    u8_t incBrightness(int inc){
      LEDS_PRINT("---Leds::incBrightness(");
      LEDS_PRINT(inc,DEC);
      LEDS_PRINT(")  new Brightness :");
      
      m_brightness += inc * LEDS_BRIGTHNESS_INC;
      if (m_brightness > 100){
        m_brightness = 0;
      }
      if (m_brightness < 0){
        m_brightness = 100;
      }
      
      LEDS_PRINTLN(m_brightness,DEC);
      updateAll();
      return m_brightness;
    }


    const char * toggleBrightnessFlag(){
      LEDS_PRINT("---Leds::toggleBrightnessFlag()   current Flag:");
      LEDS_PRINT(m_brightnessFlag.c_str());

      if (m_brightnessFlag == "MUSIC"){
        m_brightnessFlag = "";
        LEDS_PRINTLN("   new FLAG: ___ ");
      } else {
        m_brightnessFlag = "MUSIC";
        LEDS_PRINTLN("   new FLAG: CYCLE ");
      }
      return m_brightnessFlag.c_str();
    }


    /*
     * Color
     */

    u8_t getColor(){
      return m_color;
    }

    const char * getColorFlag(){
      return m_colorFlag.c_str();
    }

    u8_t incColor(int inc){
      LEDS_PRINT("---Leds::incColor(");
      LEDS_PRINT(inc,DEC);
      LEDS_PRINT(")  new color :");
    
      m_color += inc;
      if (m_color >= m_colorCount){
        m_color = 0;
      }
      if (m_color < 0){
        m_color = m_colorCount-1;
      }
      
      LEDS_PRINTLN(m_color,DEC);
      updateAll();
      return m_color;
    }


    const char * toggleColorFlag(){
      LEDS_PRINT("---Leds::toggleColorFlag()   current Flag:");
      LEDS_PRINT(m_colorFlag.c_str());

      if (m_colorFlag == "MUSIC"){
        m_colorFlag = "";
        LEDS_PRINTLN("   new FLAG: ___ ");
      } else {
        m_colorFlag = "MUSIC";
        LEDS_PRINTLN("   new FLAG: CYCLE ");
      }
      return m_colorFlag.c_str();
    }

    const char * getColorName(u8_t col){
      return colorList[m_color].name;
    }
};
Leds            led;


/** App class Button
 * 
 * handle the button of the rotary,
 * evaluate shortPress, longPres, doublePress
 * 
 */
//#define BUTTON_DEBUG
class Button;
extern Button btn;
class Button{
#ifdef BUTTON_DEBUG
  #define BUTTON_PRINT(...)   Serial.print(__VA_ARGS__)
  #define BUTTON_PRINTLN(...) Serial.println(__VA_ARGS__)
  #define BUTTON_FLUSH()      Serial.flush()
#else
  #define BUTTON_PRINT(...)
  #define BUTTON_PRINTLN(...)
  #define BUTTON_FLUSH()      
#endif

  
  private:
    OneButton m_oneButton = OneButton(  PIN_BUTTON,  /*active_low=*/  true, /*pull-up-internal=*/ true);    

    static  void handleShortPress(){
      BUTTON_PRINTLN("---static Button::handleShortPress()");
      BUTTON_FLUSH();
      btn.m_shortPress = true;
    }

    static  void handleDoublePress(){
      BUTTON_PRINTLN("---static Button::handleDoublePress()");
      BUTTON_FLUSH();
      btn.m_doublePress = true;
    }

    static void handleLongPress(){
      BUTTON_PRINTLN("---static Button::handleLongPress()    started");
      BUTTON_FLUSH();
      btn.m_longPress = true;
    }

  public:
    bool m_longPress;
    bool m_shortPress;
    bool m_doublePress;
    
  
    Button(){
      BUTTON_PRINTLN("---Button::Button()");
      BUTTON_FLUSH();
      m_oneButton.attachClick(handleShortPress);
      m_oneButton.attachDoubleClick(handleDoublePress);
      m_oneButton.attachLongPressStart(handleLongPress);
      m_longPress = false;
      m_shortPress = false;
      m_doublePress = false;
   }

    void init(){
      BUTTON_PRINTLN("---Button::init()");
      BUTTON_FLUSH();
      m_longPress = false;
      m_shortPress = false;
      m_doublePress = false;
    }

    void run(){
      BUTTON_PRINTLN("---Button::run()");
      m_oneButton.tick();
    }

    bool isLongPress()
    {
      BUTTON_PRINTLN("---Button::isLongPress()");
      if (m_longPress == true){
        m_longPress = false;
        return true;
      }
      return false;
    }

    bool wasShortPress()
    {
      BUTTON_PRINTLN("---Button::wasShortPress()");
      if (m_shortPress == true){
        m_shortPress = false;
        return true;
      }
      return false;
    }
  
    bool wasDoublePress()
    {
      BUTTON_PRINTLN("---Button::wasDoublePress()");
      if (m_doublePress == true){
        m_doublePress = false;
        return true;
      }
      return false;
    }
  
};
Button          btn;


/** App class Rotary
 * 
 * handle the rotary itself
 * main function is do get difference to last position in pollig modus
 * 
 */
//#define ROTARY_DEBUG
class Rotary{
#ifdef ROTARY_DEBUG
  #define ROTARY_PRINT(...)   Serial.print(__VA_ARGS__)
  #define ROTARY_PRINTLN(...) Serial.println(__VA_ARGS__)
  #define ROTARY_FLUSH()      Serial.flush()
#else
  #define ROTARY_PRINT(...)
  #define ROTARY_PRINTLN(...)
  #define ROTARY_FLUSH()      
#endif

  
  private:
    RotaryEncoder m_encoder = RotaryEncoder(PIN_ROTARY_DT, PIN_ROTARY_CLK, RotaryEncoder::LatchMode::TWO03);  

  public:
    long m_lastPos;
    
  
    Rotary(){
      ROTARY_PRINTLN("---Rotary::Rotary()");
      ROTARY_FLUSH();
      m_lastPos = 0;
      pinMode(PIN_ROTARY_DT, INPUT_PULLUP);
      pinMode(PIN_ROTARY_CLK, INPUT_PULLUP);
      m_encoder.setPosition(m_lastPos);
   }

    void init(){
      ROTARY_PRINTLN("---Rotary::init()");
      ROTARY_FLUSH();     
      m_lastPos = 0;
      m_encoder.setPosition(m_lastPos);
    }

    void run(){
      //ROTARY_PRINTLN("---Rotary::run()");
      m_encoder.tick();
    }

    long getChange(){
      //ROTARY_PRINTLN("---Rotary::getChange()");
      long diff = 0;
      long current = m_encoder.getPosition();
      if (current != m_lastPos){
        long diff    = current-m_lastPos;
        ROTARY_PRINT("---Rotary::getChange(): current = ");ROTARY_PRINTLN(current);
        ROTARY_PRINT("---Rotary::getChange(): m_lastPos = ");ROTARY_PRINTLN(m_lastPos);
        ROTARY_PRINT("---Rotary::getChange(): diff = ");ROTARY_PRINTLN(diff);
        m_lastPos = current;
        return diff;
      }
      return 0;
    }
};
Rotary          rot;



/** App class Display
 * 
 * handle the display by use off u8g2
 * the display has three featurs:
 * main line (big font), topic (top left small font), Flag (top right small font)
 * on update the content will be rendered to an buffer and than the display buffer will be updated
 * 
 */
// could not be init as private variable so it is now pseudo private
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C  u8g2(U8G2_R0, PIN_DISPLAY_CLK,  PIN_DISPLAY_DATA, PIN_DISPLAY_RESET);
//#define DISPLAY_DEBUG
#define DISPLAY_SMALL_FONT          u8g2_font_8x13B_mf  
#define DISPLAY_BIG_FONT            u8g2_font_8x13B_mf

class Display{
#ifdef DISPLAY_DEBUG
  #define DISPLAY_PRINT(...)    Serial.print(__VA_ARGS__)
  #define DISPLAY_PRINTLN(...)  Serial.println(__VA_ARGS__)
  #define DISPLAY_FLUSH()       Serial.flush()
#else
  #define DISPLAY_PRINT(...)
  #define DISPLAY_PRINTLN(...)
  #define DISPLAY_FLUSH()
#endif

  private:
    String  m_currentMainLine,m_lastMainLine;
    String  m_currentTopic,m_lastTopic;
    String  m_currentFlag,m_lastFlag;

    u8g2_u_t   m_sizeX,m_sizeY;
    u8g2_u_t   m_smallFontHeight,m_bigFontHeight;
    
    

  public:
    Display(){
      DISPLAY_PRINTLN("---Display::Display()  do nothing");
      DISPLAY_FLUSH();
    }

    void init(){
      DISPLAY_PRINTLN("---Display::init()");
      DISPLAY_FLUSH();

      // clear Display
      u8g2.begin();
      u8g2.clearBuffer();                               // clear the internal memory
      u8g2.sendBuffer();                                // transfer internal memory to the display

      // clear text buffers and shadows
      m_currentMainLine   = "";
      m_lastMainLine      = "";
      m_currentTopic      = "";
      m_lastTopic         = "";
      m_currentFlag       = "";
      m_lastFlag          = "";

      // get Display Infos
      m_sizeX = u8g2.getDisplayWidth();
      m_sizeY = u8g2.getDisplayHeight();
      
      u8g2.setFont(DISPLAY_BIG_FONT); 
      m_bigFontHeight = u8g2.getMaxCharHeight();
      
      u8g2.setFont(DISPLAY_SMALL_FONT); 
      m_smallFontHeight = u8g2.getMaxCharHeight();

      DISPLAY_PRINT("m_sizeX = ");
      DISPLAY_PRINTLN(m_sizeX,DEC);
      DISPLAY_PRINT("m_sizeY = ");
      DISPLAY_PRINTLN(m_sizeY,DEC);
      DISPLAY_PRINT("m_smallFontHeight = ");
      DISPLAY_PRINTLN(m_smallFontHeight,DEC);
      DISPLAY_PRINT("m_bigFontHeight = ");
      DISPLAY_PRINTLN(m_bigFontHeight,DEC);
      
    }

    void setMainLine(String val){
      DISPLAY_PRINT("---Display::setMainLine(");
      DISPLAY_PRINT(val.c_str());
      DISPLAY_PRINTLN(")");
      m_currentMainLine = val; 
    }
    
    void setTopic(String val){
      DISPLAY_PRINT("---Display::setTopic(");
      DISPLAY_PRINT(val.c_str());
      DISPLAY_PRINTLN(")");
      m_currentTopic = val;  
    }
    
    void setFlag(String val){
      DISPLAY_PRINT("---Display::setFlag(");
      DISPLAY_PRINT(val.c_str());
      DISPLAY_PRINTLN(")");
      m_currentFlag = val;
    }


    void run(){
        DISPLAY_PRINTLN("---Display::run()");
        if (( m_currentMainLine != m_lastMainLine) || 
            ( m_currentTopic    != m_lastTopic)    || 
            ( m_currentFlag     != m_lastFlag)        ){

           DISPLAY_PRINTLN("Display update");
           DISPLAY_PRINTLN(m_currentTopic.c_str());
           DISPLAY_PRINTLN(m_currentFlag.c_str());
           DISPLAY_PRINTLN(m_currentMainLine.c_str());
           
           // start on fresh buffer
           u8g2.clearBuffer();

           // show topic
           u8g2.setFont(DISPLAY_SMALL_FONT); 
           u8g2.drawStr(0,m_smallFontHeight,m_currentTopic.c_str());

           // show Flag
           u8g2_u_t  width =  u8g2.getStrWidth(m_currentFlag.c_str());
           u8g2.drawStr(m_sizeX-width-1,m_smallFontHeight,m_currentFlag.c_str());

           // show main content
           u8g2.setFont(DISPLAY_BIG_FONT); 
           u8g2.drawStr(0,m_sizeY-1,m_currentMainLine.c_str());
           
           m_lastMainLine = m_currentMainLine;
           m_lastTopic    = m_currentTopic;
           m_lastFlag     = m_currentFlag;

           // update display
           u8g2.sendBuffer(); 
        }
    }

    void welcome(){
      DISPLAY_PRINTLN("---Display::welcome()");
      clear();
      u8g2.setFont(DISPLAY_BIG_FONT);               // choose a suitable font
      String line = "SM "+String(EE_length0,DEC)+" "+String(EE_magicNumber,HEX);
      u8g2.drawStr(0,m_sizeY-1,line.c_str());       // write something to the internal memory
      u8g2.sendBuffer(); 
    }

    void clear(){
      DISPLAY_PRINTLN("---Display::clear()");
      m_currentMainLine   = "";
      m_lastMainLine      = "";
      m_currentTopic      = "";
      m_lastTopic         = "";
      m_currentFlag       = "";
      m_lastFlag          = "";
      u8g2.clearBuffer();
      u8g2.sendBuffer(); 
    }
};
Display         dis;



/** App class Sound input
 * 
 * the ad 0 channel is been used to sample the audio pegel and 
 * provide a peak level of the last 100ms
 * adaption to the backgound noise will be done as good as possible
 * 
 */
#define SOUND_IN_CYCLE_TIME   100
#define SOUND_IN_THRESHOLD    3
//#define SOUND_IN_DEBUG
class SoundIn{
#ifdef SOUND_IN_DEBUG
  #define SOUND_IN_PRINT(...)   Serial.print(__VA_ARGS__)
  #define SOUND_IN_PRINTLN(...) Serial.println(__VA_ARGS__)
  #define SOUND_IN_FLUSH()      Serial.flush()
#else
  #define SOUND_IN_PRINT(...)
  #define SOUND_IN_PRINTLN(...)
  #define SOUND_IN_FLUSH() 
#endif

  private:
    u16_t        m_quietLevel;
    u16_t        m_maxSample;
    u16_t        m_maxSampleEver;
    unsigned long   m_timer;
    u16_t        m_currentLevel;
  

  public:
    SoundIn(){
      SOUND_IN_PRINTLN("---SoundIn::SoundIn()  do nothing");
      SOUND_IN_FLUSH();
    }

    void init(){
      u16_t  level;
      SOUND_IN_PRINTLN("---SoundIn::init()");
      SOUND_IN_FLUSH();

      // take some initial audio measurements to establish the "quiet" level
      for (int i = 0; i < 20; i++) {
        level += analogRead(A0); // 0-1023
        delay(25);
      }
      m_quietLevel     = level/20;
      m_timer          = 0;
      m_maxSample      = 0;
      m_maxSampleEver  = 0;
      m_currentLevel   = 0;
    }

    void run(){
      SOUND_IN_PRINTLN("---SoundIn::run()");
      u16_t audioSample = abs(analogRead(A0) - m_quietLevel);  // take an audio sample
      if(audioSample > m_maxSample) {
        m_maxSample = audioSample;
      }

      // if the timer has expired, use the sampled audio to recalculate the LED brightness
      if(millis() > m_timer) {
        m_timer = millis() + SOUND_IN_CYCLE_TIME;      

        if(m_maxSample > SOUND_IN_THRESHOLD) { // ensure the audio is above the threshold to reduce LED flickering
          if(m_maxSample > m_maxSampleEver){
            m_maxSampleEver = m_maxSample;
          }
    
          // calculate new scaled audio level
          m_currentLevel = map(m_maxSample, SOUND_IN_THRESHOLD, m_maxSampleEver, 0, 100);
          SOUND_IN_PRINT("maxSample:");
          SOUND_IN_PRINT(m_maxSample,DEC); 
          SOUND_IN_PRINT(", maxSampleEver:");
          SOUND_IN_PRINT(m_maxSampleEver,DEC);
          SOUND_IN_PRINT(", currentLevel:");
          SOUND_IN_PRINTLN(m_currentLevel,DEC);
        } else {
          m_currentLevel = 0;
        }
    
        m_maxSample = 0;
      }
    }

    u8_t getCurrentLevel(){
      SOUND_IN_PRINT("---SoundIn::getCurrentLevel()  m_currentLevel");
      SOUND_IN_PRINTLN(m_currentLevel,DEC);
      return m_currentLevel;
    }
};
SoundIn         snd;


/** App class state machine derived from StateMachine
 * 
 * all state behavior/actions and transition including tests are defined in this class
 * 
 * 
 */

#define MY_STATE_DEBUG
#define MY_STATE_MACHINE_WELCOME_TIMEOUT    3000
class MyStateMachine;            // forward declaration to enable extern in next line
extern MyStateMachine  machine;  // define machine for access  of static members
class MyStateMachine : public StateMachine{
#ifdef MY_STATE_DEBUG
  #define MY_STATE_PRINT(...)   Serial.print(__VA_ARGS__)
  #define MY_STATE_PRINTLN(...) Serial.println(__VA_ARGS__)
  #define MY_STATE_FLUSH()      Serial.flush()
#else
  #define MY_STATE_PRINT(...)
  #define MY_STATE_PRINTLN(...)
  #define MY_STATE_FLUSH()     
#endif

  
  private:
    unsigned long   m_timer;
    unsigned long   m_welcomeTimer;
    bool            m_dumpDone;
    State*          mp_stateInit;
    State*          mp_stateMode;
    State*          mp_stateBrightness;
    State*          mp_stateColor;
    State*          mp_stateSpeed;
    State*          mp_stateOff;
    State*          mp_stateDump;
    State*          mp_stateConfig;


  public:   // define variables that are used in static function as public to give access via "machine."
    
    static MyStateMachine *   mp_this;

    ///////////////////////////////////
    //  transition functions
    ///////////////////////////////////
    
    static bool TRANS_longPress(){
      //MY_STATE_PRINTLN("---MyStateMachine::TRANS_longPress()  ");
      return btn.isLongPress();
    }
    
    static bool TRANS_shortPress(){
      //MY_STATE_PRINTLN("---MyStateMachine::TRANS_shortPress()  ");
      return btn.wasShortPress();
    }

    static void TRANS_initStartTimer(){
      //MY_STATE_PRINTLN("---MyStateMachine::TRANS_initStartTimer()  ");
      machine.m_welcomeTimer = millis()  + MY_STATE_MACHINE_WELCOME_TIMEOUT;
    }

    static bool TRANS_initDone(){
      //MY_STATE_PRINTLN("---MyStateMachine::TRANS_initDone()  ");
      if (millis() > machine.m_welcomeTimer){
        return true;
      }  else  {
        return false;
      }
    }

    static void TRANS_resetDumpDone(){
      MY_STATE_PRINTLN("---MyStateMachine::TRANS_resetDumpDone()  ");
      machine.m_dumpDone = false;
    }

    static void TRANS_setDumpDone(){
      MY_STATE_PRINTLN("---MyStateMachine::TRANS_setDumpDone()  ");
      machine.m_dumpDone = true;
    }

    static bool TRANS_dumpDone(){
      //MY_STATE_PRINTLN("---MyStateMachine::TRANS_dumpDone()  ");
      if (machine.m_dumpDone == true){
        machine.m_dumpDone = false;
        return true;
      } else {
        return false;
      }
    }


    

    ///////////////////////////////////
    //  state functions
    ///////////////////////////////////
    // for access to member vars from static functions use :  MyStateMachine::mp_this->

    /*
     * State Init
     */
    static void STATE_init(){
      if (machine.executeOnce){
        MY_STATE_PRINTLN("---MyStateMachine::STATE_init() entry action ");
        TRANS_initStartTimer();
        dis.welcome();
      }
    }


    /*
     * State mode
     */
    static void STATE_mode(){
      if (machine.executeOnce){
        MY_STATE_PRINT("---MyStateMachine::STATE_mode()  ");
        MY_STATE_PRINTLN("entry action");
        dis.setTopic("mode");
        dis.setFlag(led.getModeFlag());
        dis.setMainLine(led.getModeName());
        led.start();
      }

      // handle change/no change of mode FLAG
      if (btn.wasDoublePress()){
        // switch Flag of mode
        String newFlag = led.toggleModeFlag();
        dis.setFlag(newFlag);
      }

      // handle rotary change/no change
      long diff = rot.getChange();
      if (diff != 0){
        led.incMode(diff);
        dis.setMainLine(led.getModeName());
      }
    }

    /*
     * State brightness
     */
    static void STATE_brightness(){
      if (machine.executeOnce){
        MY_STATE_PRINT("---MyStateMachine::STATE_brightness()  ");
        MY_STATE_PRINTLN("entry action");
        dis.setTopic("bright.");
        dis.setFlag(led.getBrightnessFlag());
        int bright = led.getBrightness();
        String line = String(bright,DEC)+String("%");
        dis.setMainLine(line);
      }

      // handle change/no change of mode FLAG
      if (btn.wasDoublePress()){
        // switch Flag of mode
        String newFlag = led.toggleBrightnessFlag();
        dis.setFlag(newFlag);
      }

      // handle rotary change/no change
      long diff = rot.getChange();
      if (diff != 0){
        int bright = led.incBrightness(diff);
        String line = String(bright,DEC)+String("%");
        dis.setMainLine(line);
      }
    }

    /*
     * State color
     */
    static void STATE_color(){
      if (machine.executeOnce){
        MY_STATE_PRINT("---MyStateMachine::STATE_color()  ");
        MY_STATE_PRINTLN("entry action");
        dis.setTopic("color");
        dis.setFlag(led.getColorFlag());
        String line = led.getColorName(led.getColor());
        dis.setMainLine(line);
      }

      // handle change/no change of FLAG
      if (btn.wasDoublePress()){
        // switch Flag of mode
        dis.setFlag(led.toggleColorFlag());
      }

      // handle rotary change/no change
      long diff = rot.getChange();
      if (diff != 0){
        u32_t col = led.incColor(diff);
        dis.setMainLine(led.getColorName(col));
      }

    }
    
    /*
     * State speed
     */
    static void STATE_speed(){
      if (machine.executeOnce){
        MY_STATE_PRINT("---MyStateMachine::STATE_speed()  ");
        MY_STATE_PRINTLN("entry action");
        dis.setTopic("speed");
        dis.setFlag(led.getSpeedFlag());
        int speed = led.getSpeed();
        String line = String(speed,DEC)+String("%");
        dis.setMainLine(line);
      }
      
      // handle change/no change of FLAG
      if (btn.wasDoublePress()){
        // switch Flag of mode
        String newFlag = led.toggleBrightnessFlag();
        dis.setFlag(newFlag);
      }

      // handle rotary change/no change
      long diff = rot.getChange();
      if (diff != 0){
        int speed = led.incSpeed(diff);
        String line = String(speed,DEC)+String("%");
        dis.setMainLine(line);
      }
    }

    /*
     * State config
     */
    static void STATE_config(){
      if (machine.executeOnce){
        MY_STATE_PRINT("---MyStateMachine::STATE_config()  ");
        MY_STATE_PRINTLN("entry action");
        dis.setTopic("config");
        dis.setFlag("stripe0");
        String line = String(EE_length0,DEC)+String(" Leds");
        dis.setMainLine(line);
      }
      
      // handle change/no change of FLAG
      if (btn.wasDoublePress()){
        // only one stripe for now supported
        EE_mode   = led.getMode();
        EE_color  = led.getColor();
        EE_bright = led.getBrightness();
        EE_speed  = led.getSpeed();
               
        EE_writeAll();
        dis.setFlag("saved");
      }

      // handle rotary change/no change
      long diff = rot.getChange();
      if (diff != 0){
        EE_length0 += diff;
        if (EE_length0 < 0){
          EE_length0 = 0;
        }
        if (EE_length0 > 250){
          EE_length0 = 250;
        }
        String line = String(EE_length0,DEC)+String(" Leds");
        dis.setMainLine(line);
      }
    }

    
    /*
     * State Off
     */
    static void STATE_off(){
      if (machine.executeOnce){
        MY_STATE_PRINT("---MyStateMachine::STATE_off()  ");
        MY_STATE_PRINTLN("entry action");
        dis.clear();
        led.switchOff();
      }
    }

    /*
     * State dump
     */
    static void STATE_dump(){
      static int  adr;
      u8_t     data;
      if (machine.executeOnce){
        MY_STATE_PRINTLN("---MyStateMachine::STATE_dump() entry action ");
        adr = 0;
        TRANS_resetDumpDone();
        dis.setFlag("dump");
      }

      if (adr < 0xf0){
        String line = String(adr,HEX)+": ";
        for(int i=0;i < 0x10; i++){
          data = EEPROM.read(adr+i);
          line += String(data,HEX)+" ";
        }
        MY_STATE_PRINTLN(line.c_str());
        adr += 0x10;
      } else {
        TRANS_setDumpDone();
      }
    
    }




  public:
    MyStateMachine(){
      MY_STATE_PRINTLN("---MyStateMachine::MyStateMachine()  construct state machine with transition");
      MY_STATE_FLUSH();

      // create states
      mp_stateInit        = StateMachine::addState(&STATE_init);
      mp_stateMode        = StateMachine::addState(&STATE_mode);
      mp_stateBrightness  = StateMachine::addState(&STATE_brightness);
      mp_stateColor       = StateMachine::addState(&STATE_color);
      mp_stateSpeed       = StateMachine::addState(&STATE_speed);
      mp_stateConfig      = StateMachine::addState(&STATE_config);
      mp_stateOff         = StateMachine::addState(&STATE_off);
      mp_stateDump        = StateMachine::addState(&STATE_dump);

      // on
      mp_stateOff->addTransition(&TRANS_shortPress,mp_stateMode);

      // off
      mp_stateMode->addTransition(&TRANS_longPress,mp_stateOff);
      mp_stateBrightness->addTransition(&TRANS_longPress,mp_stateOff);
      mp_stateColor->addTransition(&TRANS_longPress,mp_stateOff);
      mp_stateSpeed->addTransition(&TRANS_longPress,mp_stateOff);
      mp_stateConfig->addTransition(&TRANS_longPress,mp_stateDump);
      
      // workflow (short press)
      mp_stateMode->addTransition(&TRANS_shortPress,mp_stateBrightness);
      mp_stateBrightness->addTransition(&TRANS_shortPress,mp_stateColor);
      mp_stateColor->addTransition(&TRANS_shortPress,mp_stateSpeed);
      mp_stateSpeed->addTransition(&TRANS_shortPress,mp_stateConfig);
      mp_stateConfig->addTransition(&TRANS_shortPress,mp_stateMode);

      // workflow (timer & reaction)
      mp_stateInit->addTransition(&TRANS_initDone,mp_stateMode);
      mp_stateDump->addTransition(&TRANS_dumpDone,mp_stateConfig);
      

    
    }

    void init(){
      // set start conditions
      MY_STATE_PRINTLN("---MyStateMachine::init()  set start condition/state");
      MY_STATE_FLUSH();
      transitionTo(mp_stateInit);
    }

    void run(){
      StateMachine::run();   
    }
  
};
MyStateMachine  machine;




void setup(void) {
  
  //Serial.begin(115200);  <== done in constructor of Debug Class to enable debug prints before setup call
  
  
  DEBUG_PRINTLN("universal light controller V0.8");
  DEBUG_PRINTLN("setup function");
  DEBUG_FLUSH();

  randomSeed(analogRead(0));
  debug.init();
  EE_init();

  led.init();
  dis.init();
  snd.init();
  btn.init();
  rot.init();
  machine.init();
}

void loop(void) {
  // use EVA with timeslicer polling 
  // loop runs as fast as possible
  // timebase is created in moduls if needed
  
  // E: Poll inputs
  //encoder.tick();
  btn.run();
  snd.run();
  rot.run();

  // V: do/calc logic
  machine.run();

  // A: Set ouputs
  dis.run();
  led.run();

  // DEBUG infos (to check min possible time base)
  debug.run();        // including delay !!
}



/*
 * void loop() {
  now = millis();

  ws2812fx.service();

  // trigger on a regular basis
  if(now - last_trigger > TIMER_MS) {
    ws2812fx.trigger();
    last_trigger = now;
  }

  // trigger, if analog value is above threshold
  // this comes in handy, when using a microphone on analog input
  if(analogRead(ANALOG_PIN) > ANALOG_THRESHOLD) {
    ws2812fx.trigger();
  }
}
*/
