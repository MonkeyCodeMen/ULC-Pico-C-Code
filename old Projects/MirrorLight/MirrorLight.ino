
#define LOG_OUT 1

#include <FastLED.h>                                         // FastLED library. Preferably the latest copy of FastLED 2.1.
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif




// PIN definitions
#define PIN_BUT_1     2
#define PIN_BUT_2     3
#define PIN_BUT_3     4
#define BUT_MAX       3


#define PIN_LED_DT_TOP_BAR    5
#define PIN_LED_DT_LEFT_BAR   6     
#define PIN_LED_DT_RIGHT_BAR  7
#define PIN_LED_DT_AMB_LEFT   8
#define PIN_LED_DT_AMB_RIGHT  9

#define PIN_RELAIS_1          10



// LED/STRIPE  definition 

#define LED_TYPE          WS2812B
#define COLOR_ORDER       GRB


#define LED_SIDE_COUNT      60
#define LED_TOP_COUNT       112
#define LED_AMB_LEFT_COUNT  27
#define LED_AMB_RIGHT_COUNT 45
#define LED_MAX             (LED_AMB_LEFT_COUNT + LED_SIDE_COUNT + LED_TOP_COUNT + LED_SIDE_COUNT + LED_AMB_RIGHT_COUNT)

#define STRIPE_AMB_LEFT     0
#define STRIPE_LEFT         1
#define STRIPE_TOP          2
#define STRIPE_RIGHT        3
#define STRIPE_AMB_RIGHT    4
#define STRIPE_MAX          5


#define BLACK   CRGB::Black
#define WHITE   CRGB::White
#define GREEN   CRGB::Green
#define RED     CRGB::Red
#define BLUE    CRGB::Blue
#define PINK    CRGB::Pink
#define PURPLE  CRGB::Purple






void setup() {
  int i;
  // setup relais
  pinMode(PIN_RELAIS_1,OUTPUT);
  digitalWrite(PIN_RELAIS_1,HIGH);

  Serial.begin(115200);                                         // use the serial port
  Serial.println("setup");

  STRIPE_init();
  BUT_init();

  digitalWrite(PIN_RELAIS_1,HIGH);
  STRIPE_setStripes(BLACK,WHITE,BLACK,WHITE,BLACK);
  Serial.println("setup done.");

}




void loop() {
  static int i =2;
 
  BUT_loop();

  if (BUT_getPressed(0) == true){
    i++;

    switch(i){
      case 1: 
        digitalWrite(PIN_RELAIS_1,HIGH);
        STRIPE_setStripes(BLACK,BLACK,BLACK,BLACK,BLACK);
        Serial.println("case 1");
        break;

      case 2: 
        digitalWrite(PIN_RELAIS_1,HIGH);
        STRIPE_setStripes(BLACK,WHITE,BLACK,WHITE,BLACK);
        Serial.println("case 2");
        break;

      case 3:
        digitalWrite(PIN_RELAIS_1,HIGH);
        STRIPE_setStripes(BLACK,WHITE,WHITE,WHITE,BLACK);
        Serial.println("case 3");
        break;
      
      case 4:
        digitalWrite(PIN_RELAIS_1,LOW);
        STRIPE_setAll(BLUE);
        Serial.println("case 4");
        break;
      
      case 5:
        digitalWrite(PIN_RELAIS_1,LOW);
        STRIPE_setAll(RED);
        Serial.println("case 5");
        break;

      case 6:
        digitalWrite(PIN_RELAIS_1,LOW);
        STRIPE_setAll(GREEN);
        Serial.println("case 6");
        break;

      case 7:
        digitalWrite(PIN_RELAIS_1,LOW);
        STRIPE_setAll(PINK);
        Serial.println("case 7");
        break;

      case 8:
        digitalWrite(PIN_RELAIS_1,LOW);
        STRIPE_setAll(PURPLE);
        Serial.println("case 8");
        break;

      default:
        i=0;
        Serial.println("case default");
        break;    

    }
  }
  //show_at_max_brightness_for_power();
  FastLED.show(); 
  delay(50);    
}
