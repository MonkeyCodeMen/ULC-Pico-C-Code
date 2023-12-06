#pragma once


/*

communication channels  and adress map:

main communication chain:
USER (LAN/WLAN) <==> (LAN/WLAN) Raspberry Pi (USB-serial) <==> (USB-serial) Raspberry Pico  (I2C0-Master) <==> (I2C-slave)  Arduino Nan



Raspberry Pico:
(I2C0-Master) <==> (I2C-slave)

SPI0    <==>    CS0: SD card
SPI1    <==>    CS1: TFT (/ Touch + GPIO pin used)
                CS2: SPI-slave-device (free)
I2C0   Pico master <==>    Slaves  (7Bit adr + R/W Bit):
                0x20: MPC23017 
                0x3X: Display  OLED or LCD 
                0x50: AT24C32
                0x58: arduino Nano - PWM LED expander
                0x68: RTC DS1307
GPIO    ==>     2 x neoLed Stripe
                2 x neoLed Matrix
GPIO    <==>    I2s  Micro
Analog  <==     Micro


Arduino Nano  (IO) ==>  4 x LED Switch  (12V)  
                   ==>  2 x RGB LED (12V)


Pin mapping Pico:
=================

----+---------------+-------------------------------------------+----------------------
PIN | Arduino       |   Raspberry PI PICO                       | Application 
    | DXX : default | GPXX : SPI        : I2C       : UART      | pin assignment
----+---------------+-------------------------------------------+----------------------
1   | D0  : SER1_TX | GP0  : SPI0_RX    : I2C0_SDA  : UART0_TX  | DEBUG_SER_TX
2   | D1  : SER1_RX | GP1  : SPI0_CSn   : I2C0_SCL  : UART0_RX  | DEBUG_SER_RX
3   |---------------|------ GND --------------------------------|------ GND -----------
4   | D2  :         | GP2  : SPI0_SCK   : I2C1_SDA  : UART0_CTS |
5   | D3  :         | GP3  : SPI0_TX    : I2C1_SCL  : UART0_RTS |
6   | D4  :WIRE0_SDA| GP4  : SPI0_RX    : I2C0_SDA  : UART1_TX  | I2C_SDA
7   | D5  :WIRE0_SCL| GP5  : SPI0_CSn   : I2C0_SCL  : UART1_RX  | I2C_SCL
8   |---------------|------ GND --------------------------------|------ GND -----------
9   | D6  :         | GP6  : SPI0_SCK   : I2C1_SDA  : UART1_CTS |
10  | D7  :         | GP7  : SPI0_TX    : I2C1_SCL  : UART1_RTS |
11  | D8  : SER2_TX | GP8  : SPI1_RX    : I2C0_SDA  : UART1_TX  |
12  | D9  : SER2_RX | GP9  : SPI1_CSn   : I2C0_SCL  : UART1_RX  |
13  |---------------|------ GND --------------------------------|------ GND -----------
14  | D10 :         | GP10 : SPI1_SCK   : I2C1_SDA  : UART1_CTS |
15  | D11 :         | GP11 : SPI1_TX    : I2C1_SCL  : UART1_RTS |
16  | D12 :SPI1_MISO| GP12 : SPI1_RX    : I2C0_SDA  : UART0_TX  | SPI_MISO
17  | D13 :SPI1_CS  | GP13 : SPI1_CSn   : I2C0_SCL  : UART0_RX  | SPI_CS_TFT
18  |---------------|------ GND --------------------------------|------ GND -----------
19  | D14 :SPI1_SCK | GP14 : SPI1_SCK   : I2C1_SDA  : UART0_CTS | SPI_SCLK
20  | D15 :SPI1_MOSI| GP15 : SPI1_TX    : I2C1_SCL  : UART0_RTS | SPI_MOSI
----+---------------+-------------------------------------------+----------------------
21  | D16 :SPI0_MISO| GP16 : SPI0_RX    : I2C0_SDA  : UART0_TX  | SD_MISO
22  | D17 :SPI0_CS0 | GP17 : SPI0_CSn   : I2C0_SCL  : UART0_RX  | SD_CS
23  |---------------|------ GND --------------------------------|------ GND -----------
24  | D18 :SPI0_SCK | GP18 : SPI0_SCK   : I2C1_SDA  : UART0_CTS | SD_SCLK
25  | D19 :SPI0_MOSI| GP19 : SPI0_TX    : I2C1_SCL  : UART0_RTS | SD_MOSI
26  | D20 :         | GP20 : SPI0_RX    : I2C0_SDA  :           |
27  | D21 :         | GP21 : SPI0_CSn   : I2C0_SCL  :           |
28  |---------------|------ GND --------------------------------|------ GND -----------
29  | D22 :         | GP22 : SPI0_SCK   :           : UART1_CTS |
30  |---------------|------ RUN --------------------------------|------ RUN -----------
31  | A0  :WIRE1_SDA| GP26 : SPI1_SCK   : I2C0_SDA  : UART1_CTS |
32  | A1  :WIRE1_SCL| GP27 : SPI1_TX    : I2C0_SCL  : UART1_RTS |
33  |---------------|------ AGND -------------------------------|------ AGND ----------
34  | A2  :         | GP28 : SPI1_SCK   : I2C0_SDA  : UART1_CTS |
35  |---------------|------ AREF -------------------------------|------ AREF ----------
36  |---------------|------ 3V3_OUT ----------------------------|------ 3V3_OUT -------
37  |---------------|------ 3V3_EN -----------------------------|------ 3V3_EN --------
38  |---------------|------ GND --------------------------------|------ GND -----------
39  |---------------|------ V_SYS ------------------------------|------ V_SYS ---------
40  |---------------|------ V_BUS ------------------------------|------ V_BUS ---------
----+---------------+-------------------------------------------+----------------------



Pin mapping Nano:
=================

I2C simple protocoll

ADR:        0x58    master-read     master-write
register:   0..11   N/A             PWM value 0..255

SOF | 7Bit Adr + R/W | Register Adr 8 bit | Register Value 8 Bit | EOF
SOF | AAAA AAA   R/W |  0..11             | 0..255               | EOF  

----+---------------------------+-----------------------------------+----------------------
PIN | Arduino Nano              |   Atmega328                       | Application 
    | DXX : ANA : COM   : INT   |PWM:INT-C  : Timer : ANA-C : Pin   |
----+---------------------------+-----------------------------------+----------------------
1   | D1  :     :SER0_TX:       | - :PCINT17:       :       : PD1   | DEBUG_SER_TX (reserved)
2   | D0  :     :SER0_RX:       | - :PCINT16:       :       : PD0   | DEBUG_SET_RX (reserved)
3   |---------------------------|------ RESET-----------------------|------ RESET ---------
4   |---------------------------|------ GND ------------------------|------ GND -----------
5   | D2  :     :       : INT0  | - :PCINT18:       :       : PD2   | PWM-0 .. LED_SW0
6   | D3  :     :       : INT1  | x :PCINT19: OC2B  :       : PD3   | PWM-1 .. LED_SW1
7   | D4  :     : XCK   :       | - :PCINT20: T0    :       : PD4   | PWM-2 .. LED_SW2
8   | D5  :     :       :       | x :PCINT21:OC0B/T1:       : PD5   | PWM-3 .. LED_SW3
9   | D6  :     :       :       | x :PCINT22: OC0A  : AIN0  : PD6   | PWM-4 .. LED_RGB0_R
10  | D7  :     :       :       | - :PCINT23:       : AIN1  : PD7   | PWM-5 .. LED_RGB0_G
11  | D8  :     :       :       | - :PCINT0 :CLKO/ICP1:     : PB0   | PWM-6 .. LED_RGB0_B
12  | D9  :     :       :       | x :PCINT1 : OC1A  :       : PB1   | PWM-7 .. LED_RGB1_R
13  | D10 :     :SPI_CS :       | x :PCINT2 : OC1B  :       : PB2   | PWM-8 .. LED_RGB2_G
14  | D11 :     :SPI_MOSI:      | x :PCINT3 : OC2A  :       : PB3   | PWM-9 .. LED_RGB3_B
15  | D12 :     :SPI_MISO:      | - :PCINT4 :       :       : PB4   | PWM-10 .. free
----+---------------------------+-----------------------------------+----------------------
16  | D13 :     :SPI_CLK:       | - :PCINT5 :       :       : PB5   | PWM-11 .. free
17  |---------------------------|------ 3V3 ------------------------|------ 3V3 -----------
18  |---------------------------|------ AREF -----------------------|------ AREF ----------
19  | D14 : A0  :       :       | - :PCINT8 :       :       : PC0   | 
20  | D15 : A1  :       : INT0  | - :PCINT9 :       :       : PC1   | 
21  | D16 : A2  :       : INT1  | - :PCINT10:       :       : PC2   | 
22  | D17 : A3  :       :       | - :PCINT11:       :       : PC3   | 
23  | D18 : A4  :I2C_SDA:       | - :PCINT12:       :       : PC4   | I2C_SDA slave
24  | D19 : A5  :I2C_SCL:       | - :PCINT13:       :       : PC5   | I2C_SCL slave
25  | D20 : A6  :       :       | - :       :       :       :       | 
26  | D21 : A7  :       :       | - :       :       :       :       | 
27  |---------------------------|------ 5V -------------------------|------ 5V ------------
28  |---------------------------|------ RESET-----------------------|------ RESET ---------
29  |---------------------------|------ GND ------------------------|------ GND -----------
30  |---------------------------|------ VIN-------------------------|------ VIN -----------
----+---------------------------+-----------------------------------+----------------------


simple program on Arduino nano:

#include <Wire.h>

void receiveEvent(int count){
    u8_t channel = Wire.read();
    u8_t data = Wire.read();
    
}

void setup() 
{
    * D2-D13  :  Analog out
    * Wire.begin(0x58)
    * Wire.onReceive(receiveEvent);
    * Serial debug??
}

void loop()
{
    delay(100);
}







To DO 

Pico picture and pin usage here

SD card and TFT does not work on same SPI bus !!!
SD card has special SPI operation 


*/

#define PIN_LED_SWITCH_1  	7
#define PIN_LED_SWITCH_2    6

#define PIN_RGB1_LED_R      10
#define PIN_RGB1_LED_G      11
#define PIN_RGB1_LED_B      12

#define PIN_STRIPE_1        2 
#define PIN_STRIPE_2        3 

#define PIN_MATRIX_1        20
#define PIN_MATRIX_2        21

#define PIN_SPI0_CS_SD      9 
#define PIN_SPI0_CS_TFT     13
#define PIN_TFT_LED         8 
#define PIN_TFT_DC          15   
#define PIN_TFT_RST         14   

/* defined in common.h for PICO board :

//SPI 0
#define PIN_SPI0_CS0        17  
#define PIN_SPI0_MOSI       19
#define PIN_SPI0_MISO       16
#define PIN_SPI_SCK         18

// I2C
#define PIN_WIRE0_SDA       4
#define PIN_WIRE0_SCL       5
*/





