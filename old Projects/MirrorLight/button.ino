bool BUT_Pressed[BUT_MAX];
int  BUT_LastState[BUT_MAX];
int  BUT_Debounce[BUT_MAX];
int  BUT_List[BUT_MAX];
String BUT_Code[BUT_MAX];

#define BUT_DEBOUNCE   3

void BUT_init(){
  int i;
  Serial.println("BUT init()");
  pinMode(PIN_BUT_1,INPUT_PULLUP);
  pinMode(PIN_BUT_2,INPUT_PULLUP);
  pinMode(PIN_BUT_3,INPUT_PULLUP);

  for(i=0;i < BUT_MAX;i++){
    BUT_Pressed[i]   = false;
    BUT_Debounce[i]  = 0;
    BUT_LastState[i] = HIGH;
  }
  BUT_List[0] = PIN_BUT_1;
  BUT_List[1] = PIN_BUT_2;
  BUT_List[2] = PIN_BUT_3;

  BUT_Code[0] = "A";
  BUT_Code[1] = "B";
  BUT_Code[2] = "C";

  Serial.println("BUT init() done.");

}


void BUT_loop(){
  int i;
  int state;
  for (i=0;i < BUT_MAX; i++){
    state = digitalRead(BUT_List[i]);

    if (BUT_LastState[i] == HIGH){
      // button was NOT pressed
      if(state == LOW){
        // now pressed
        BUT_Debounce[i]++;
        if (BUT_Debounce[i] > BUT_DEBOUNCE){
          // valid press
          Serial.println("but pressed:" + BUT_Code[i]);
          BUT_Debounce[i] = 0;
          BUT_LastState[i] = LOW;
        }
      } else {
        // debounce !!
        BUT_Debounce[i] = 0; 
      }
    } else {
      // button was pressed
      if (state == HIGH){
        // now released again
        BUT_Debounce[i]++;
        if (BUT_Debounce[i] > BUT_DEBOUNCE){
          // valid release
          Serial.println("but released:" + BUT_Code[i]);
          BUT_Debounce[i] = 0;
          BUT_LastState[i] = HIGH;
          BUT_Pressed[i] = true;
        }
      }
    }
  }
}

bool BUT_getPressed(int nr){
  bool temp;
  if (nr > BUT_MAX){
   return false;
  }
  temp = BUT_Pressed[nr];
  BUT_Pressed[nr] = false;
  return temp;
}
