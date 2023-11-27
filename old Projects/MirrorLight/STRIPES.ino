// Initialize changeable global variables.
u8_t max_bright = 255;                                     // Overall brightness definition. It can be changed on the fly.

struct CRGB topBar[LED_TOP_COUNT];
struct CRGB leftBar[LED_SIDE_COUNT];
struct CRGB rightBar[LED_SIDE_COUNT];
struct CRGB ambBarLeft[LED_AMB_LEFT_COUNT];
struct CRGB ambBarRight[LED_AMB_RIGHT_COUNT];
    

int   STRIPE_offset[STRIPE_MAX+1];
int   STRIPE_count[STRIPE_MAX];
struct CRGB* STRIPE_list[STRIPE_MAX];




void STRIPE_init(){
  Serial.println("STRIPES init()");

  // setup LED
  pinMode(PIN_LED_DT_TOP_BAR,OUTPUT);
  pinMode(PIN_LED_DT_LEFT_BAR,OUTPUT);
  pinMode(PIN_LED_DT_RIGHT_BAR,OUTPUT);
  pinMode(PIN_LED_DT_AMB_LEFT,OUTPUT);
  pinMode(PIN_LED_DT_AMB_RIGHT,OUTPUT);
  

  //add all stripes to LED Driver
  LEDS.addLeds<LED_TYPE, PIN_LED_DT_AMB_LEFT,  COLOR_ORDER>(ambBarLeft,  LED_AMB_LEFT_COUNT);
  LEDS.addLeds<LED_TYPE, PIN_LED_DT_LEFT_BAR,  COLOR_ORDER>(leftBar,     LED_SIDE_COUNT);
  LEDS.addLeds<LED_TYPE, PIN_LED_DT_TOP_BAR,   COLOR_ORDER>(topBar,      LED_TOP_COUNT);
  LEDS.addLeds<LED_TYPE, PIN_LED_DT_RIGHT_BAR, COLOR_ORDER>(rightBar,    LED_SIDE_COUNT);
  LEDS.addLeds<LED_TYPE, PIN_LED_DT_AMB_RIGHT, COLOR_ORDER>(ambBarRight, LED_AMB_RIGHT_COUNT);

  FastLED.setBrightness(max_bright);
  set_max_power_in_volts_and_milliamps(5, 10000);               // FastLED Power management set at 5V, 10A.


  STRIPE_offset[STRIPE_AMB_LEFT]  = 0;
  STRIPE_offset[STRIPE_LEFT]      = LED_AMB_LEFT_COUNT;
  STRIPE_offset[STRIPE_TOP]       = LED_AMB_LEFT_COUNT + LED_SIDE_COUNT;
  STRIPE_offset[STRIPE_RIGHT]     = LED_AMB_LEFT_COUNT + LED_SIDE_COUNT + LED_TOP_COUNT;
  STRIPE_offset[STRIPE_AMB_RIGHT] = LED_AMB_LEFT_COUNT + LED_SIDE_COUNT + LED_TOP_COUNT + LED_SIDE_COUNT ;
  STRIPE_offset[STRIPE_MAX]       = LED_MAX;

  STRIPE_count[STRIPE_AMB_LEFT]  = LED_AMB_LEFT_COUNT;
  STRIPE_count[STRIPE_LEFT]      = LED_SIDE_COUNT;
  STRIPE_count[STRIPE_TOP]       = LED_TOP_COUNT;
  STRIPE_count[STRIPE_RIGHT]     = LED_SIDE_COUNT;
  STRIPE_count[STRIPE_AMB_RIGHT] = LED_AMB_RIGHT_COUNT ;

  STRIPE_list[STRIPE_AMB_LEFT]    = ambBarLeft;
  STRIPE_list[STRIPE_LEFT]        = leftBar;
  STRIPE_list[STRIPE_TOP]         = topBar;
  STRIPE_list[STRIPE_RIGHT]       = rightBar;
  STRIPE_list[STRIPE_AMB_RIGHT]   = ambBarRight;
  
  Serial.println("STRIPES init() done.");

}

void STRIPE_setAll(CRGB color){
  int stripe,led;
  struct CRGB * pStripe;
  for (stripe = 0;stripe<STRIPE_MAX;stripe++){
    pStripe = STRIPE_list[stripe]; 
    for(led=0;led < STRIPE_count[stripe];led++){
      *(pStripe+led)=color;
    }
  }
}


void STRIPE_setStripes(CRGB c1,CRGB c2,CRGB c3,CRGB c4,CRGB c5){
  STRIPE_setStripe(0,c1);
  STRIPE_setStripe(1,c2);
  STRIPE_setStripe(2,c3);
  STRIPE_setStripe(3,c4);
  STRIPE_setStripe(4,c5);
  
}


void STRIPE_setStripe(int nr,CRGB color){
  int i;
  struct CRGB * pStripe;
  if ((nr < 0) || (nr >= STRIPE_MAX)){
    Serial.println("STRIPE_setStripe() invalid index");
    return;
  }
  pStripe = STRIPE_list[nr]; 
  for(i=0; i < STRIPE_count[nr]; i++){
    *(pStripe+i)=color;
  }  
  
}

void STRIPE_setLed(int index,CRGB color){
  int i,delta;
  struct CRGB * pStripe;
  //Reihenfolge   left_AMB  :  left_Bar  :  top_Bar  :  right_Bar  :  right_AMB
  if ((index < 0) || (index >= LED_MAX)){
    Serial.println("STRIPES_setLed() invalid index");
    return;
  }

  for (i=0; i < STRIPE_MAX; i++)
  {
    if (index < STRIPE_offset[i+1]){
      pStripe = STRIPE_list[i]; 
      delta   = index-STRIPE_offset[i];

      if (delta >= STRIPE_count[i]){
        Serial.println("STRIPES_setLed() logic error");
      } else {
        *(pStripe+delta)=color;
      }
      return;
    }
  }
  Serial.println("STRIPES_setLed() logic error");
}
