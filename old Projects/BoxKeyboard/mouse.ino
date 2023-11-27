/*
 * Pin 5 is the mouse data pin, pin 6 is the clock pin
 * Feel free to use whatever pins are convenient.
 */

#define MDATA 7     //green
#define MCLK  6     //white
// orange GND
// blue   VCC

// #define PS2_DEBUG 1

/*
 * mouse interface
 */



void mouseSetup(void){
    Serial.println("init ps2");
    ps2_init();

    Serial.println("init mouse USB HID interface");
    Mouse.begin();
  
}


void mouseLoop(void){
    
    char mstat;
    char mx;
    char my;

    if (_appMode == MODE_MOUSE){
  
      /* get a reading from the mouse */
      ps2_write(0xeb);  /* give me data! */
      ps2_read();      /* ignore ack */
      mstat = ps2_read();
      mx = ps2_read();
      my = ps2_read();
  
      my *= -1;
  
      mx = scale(mx);
      my = scale(my);
        
      if(_debugMode == MODE_RUN){
       
          Mouse.move(mx,my,0);
      } else {
          /* send the data back up */
          if(mx+my != 0)
          {
            Serial.print(mstat, BIN);
            Serial.print("\tX=");
            Serial.print(mx, DEC);
            Serial.print("\tY=");
            Serial.print(my, DEC);
            Serial.println("");        
          }
      }
    }    
}


/*
 * 
 * application logic
 * 
 */

char scale(char in){
  switch(in){
    case  0:   return 0;
    case  1:   return 1;
    case -1:   return -1;
    case  2:   return 3;
    case -2:   return -3;
    case  3:   return 5;
    case -3:   return -5;
    case  4:   return 9;
    case -4:   return -9;
    case  5:   return 12; 
    case -5:   return -12; 
    
  }
  return in*3;
}



/*
 *    low level driver
 */


void releaseHigh(int pin){ //PS2 Pin H (input an Pullup)
  pinMode(pin, INPUT_PULLUP);
  digitalWrite(pin, HIGH);
}
void driveLow(int pin){  //PS2 PIN  L  (output and set to LOW)
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

/*
 *  PS2 Layer
 */

void ps2_write(char data){ // write byte to device
  char i;
  char parity = 1;
  char ack;

#ifdef PS2_DEBUG
  Serial.print("Sending ");
  Serial.print(data, HEX);
  Serial.print(" to mouse");
  Serial.println("");
#endif

  // keep clk line low for at least 100�sec
  driveLow(MCLK);        // should be already
  delayMicroseconds(100);

  // start clk  set data low as start bit ==> write request 
  driveLow(MDATA);
  releaseHigh(MCLK); 
  delayMicroseconds(5); // wait until MCLK has been rised
  
  // send data, set data line on/after falling edge
  for (i=0; i < 8; i++) {
    while (digitalRead(MCLK) == HIGH) {};
    if (data & 0x01) {
      releaseHigh(MDATA);
    }
    else {
      driveLow(MDATA);
    }
    parity = parity ^ (data & 0x01);
    data = data >> 1;
    while (digitalRead(MCLK) == LOW) {};
  }
  
  // send parity bit, set bit on/after falling edge of clock line   
  while (digitalRead(MCLK) == HIGH) {};
  if (parity) {
    releaseHigh(MDATA);
  }
  else {
    driveLow(MDATA);
  }
  while (digitalRead(MCLK) == LOW) {};

  // set stop bit on/after falling edge
  while (digitalRead(MCLK) == HIGH) {};
  releaseHigh(MDATA);
  while (digitalRead(MCLK) == LOW) {};
  
  // read ack on rising edge
  while (digitalRead(MCLK) == HIGH) {};
  while (digitalRead(MCLK) == LOW) {};
  ack = digitalRead(MDATA);
  while (digitalRead(MDATA) == LOW) {};

  driveLow(MCLK);
  driveLow(MDATA);  
  
#ifdef PS2_DEBUG
  Serial.print("send done ACK:");
  Serial.print(ack, HEX);
  Serial.println("");
#endif
}

char ps2_read(void){ // read byte from device
  char data = 0x00;
  int i;
  char bit = 0x01;
  char start,stop,parity;

#ifdef PS2_DEBUG
  Serial.println("reading byte from mouse");
#endif

  // start clk; data high ==> device can send data 
  releaseHigh(MDATA);
  releaseHigh(MCLK);
  delayMicroseconds(5); // wait until MCLK has been rised
  
  // read start bit on falling edge of clock
  while (digitalRead(MCLK) == HIGH) {};
  start = digitalRead(MDATA);
  while (digitalRead(MCLK) == LOW) {};

  // read 8 bit data  on/after falling edge of clock
  for (i=0; i < 8; i++) {
    while (digitalRead(MCLK) == HIGH) {};
    if (digitalRead(MDATA) == HIGH) { 
        data = data | bit;  
    }
    bit = bit << 1;
    while (digitalRead(MCLK) == LOW) {};
  }
  
  // read parity, but ignore it
  while (digitalRead(MCLK) == HIGH) {};
  parity = digitalRead(MDATA);
  while (digitalRead(MCLK) == LOW) {};

  // read stop bit (do not check value of stop bit)
  while (digitalRead(MCLK) == HIGH) {};
  stop = digitalRead(MDATA);
  while (digitalRead(MCLK) == LOW) {};

  // drive and hold clock LOW until ready for next data 
  driveLow(MCLK);


#ifdef PS2_DEBUG
  Serial.print("Recvd data ");
  Serial.print(data, HEX);
  Serial.print(" from mouse\n");
  Serial.print("start :");
  Serial.print(start,DEC);
  Serial.print("  parity :");
  Serial.print(parity,DEC);
  Serial.print("  stop :");
  Serial.print(stop,DEC);
  
  Serial.println("");
#endif

  return data;
}

void ps2_init(void){ // init PS2 interface
  releaseHigh(MCLK);
  releaseHigh(MDATA);
  
  
  
#ifdef PS2_DEBUG
  Serial.println("Sending reset to mouse");
#endif
  ps2_write(0xff);   // RESET
  ps2_read();  /* ack byte */
  ps2_read();  /* blank */
  ps2_read();  /* blank */

/*
#ifdef PS2_DEBUG
  Serial.println("mouse setup resolution");
#endif
  ps2_write(0xE8);   // set resolution
  ps2_write(0x02);  // 00: 1/mm  01: 2/mm  02: 4/mm  03  8/mm 
  ps2_read(); 
  ps2_read(); 
*/


#ifdef PS2_DEBUG
  Serial.println("Sending remote mode code");
#endif
  ps2_write(0xf0);  /* remote mode */
  ps2_read();       /* ack */
  
  //delayMicroseconds(100);
}
