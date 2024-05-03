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
4   | D2  :         | GP2  : SPI0_SCK   : I2C1_SDA  : UART0_CTS | LED_D01
5   | D3  :         | GP3  : SPI0_TX    : I2C1_SCL  : UART0_RTS | LED_DO0
6   | D4  :WIRE0_SDA| GP4  : SPI0_RX    : I2C0_SDA  : UART1_TX  | I2C_SDA
7   | D5  :WIRE0_SCL| GP5  : SPI0_CSn   : I2C0_SCL  : UART1_RX  | I2C_SCL
8   |---------------|------ GND --------------------------------|------ GND -----------
9   | D6  :         | GP6  : SPI0_SCK   : I2C1_SDA  : UART1_CTS | /RESET_NANO
10  | D7  :         | GP7  : SPI0_TX    : I2C1_SCL  : UART1_RTS | /RESET_TFT
11  | D8  : SER2_TX | GP8  : SPI1_RX    : I2C0_SDA  : UART1_TX  | TFT_LED
12  | D9  : SER2_RX | GP9  : SPI1_CSn   : I2C0_SCL  : UART1_RX  | TFT_DC
13  |---------------|------ GND --------------------------------|------ GND -----------
14  | D10 :         | GP10 : SPI1_SCK   : I2C1_SDA  : UART1_CTS | SPI_CS_TOUCH
15  | D11 :         | GP11 : SPI1_TX    : I2C1_SCL  : UART1_RTS | TOUCH_IRQ
16  | D12 :SPI1_MISO| GP12 : SPI1_RX    : I2C0_SDA  : UART0_TX  | SPI_MISO
17  | D13 :SPI1_CS  | GP13 : SPI1_CSn   : I2C0_SCL  : UART0_RX  | SPI_CS_TFT
18  |---------------|------ GND --------------------------------|------ GND -----------
19  | D14 :SPI1_SCK | GP14 : SPI1_SCK   : I2C1_SDA  : UART0_CTS | SPI_SCLK
20  | D15 :SPI1_MOSI| GP15 : SPI1_TX    : I2C1_SCL  : UART0_RTS | SPI_MOSI
----+---------------+-------------------------------------------+----------------------
----+---------------+-------------------------------------------+----------------------
21  | D16 :SPI0_MISO| GP16 : SPI0_RX    : I2C0_SDA  : UART0_TX  | SD_MISO
22  | D17 :SPI0_CS0 | GP17 : SPI0_CSn   : I2C0_SCL  : UART0_RX  | SD_CS
23  |---------------|------ GND --------------------------------|------ GND -----------
24  | D18 :SPI0_SCK | GP18 : SPI0_SCK   : I2C1_SDA  : UART0_CTS | SD_SCLK
25  | D19 :SPI0_MOSI| GP19 : SPI0_TX    : I2C1_SCL  : UART0_RTS | SD_MOSI
26  | D20 :         | GP20 : SPI0_RX    : I2C0_SDA  :           | LED_DO2
27  | D21 :         | GP21 : SPI0_CSn   : I2C0_SCL  :           | LED_DO3
28  |---------------|------ GND --------------------------------|------ GND -----------
29  | D22 :         | GP22 : SPI0_SCK   :           : UART1_CTS | I2S_SD
30  |---------------|------ RUN --------------------------------|------ RUN -----------
31  | A0  :WIRE1_SDA| GP26 : SPI1_SCK   : I2C0_SDA  : UART1_CTS | A0
32  | A1  :WIRE1_SCL| GP27 : SPI1_TX    : I2C0_SCL  : UART1_RTS | I2S_SCK
33  |---------------|------ AGND -------------------------------|------ AGND ----------
34  | A2  :         | GP28 : SPI1_SCK   : I2C0_SDA  : UART1_CTS | I2S_WS
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
15  | D12 :     :SPI_MISO:      | - :PCINT4 :       :       : PB4   | PWM-10 .. free1
----+---------------------------+-----------------------------------+----------------------
16  | D13 :     :SPI_CLK:       | - :PCINT5 :       :       : PB5   | PWM-11 .. free2
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


SD card and TFT does not work on same SPI bus !!!
SD card has special SPI operation 


*/

/*
==> these pin's are moved to the Arduino NANO 
#define PIN_LED_SWITCH_1  	
#define PIN_LED_SWITCH_2    
#define PIN_LED_SWITCH_3  	
#define PIN_LED_SWITCH_4    

#define PIN_RGB1_LED_R      
#define PIN_RGB1_LED_G      
#define PIN_RGB1_LED_B      

#define PIN_RGB2_LED_R      
#define PIN_RGB2_LED_G      
#define PIN_RGB2_LED_B      
*/


// for now (to compile .. I2C slave will be added later)
#define PIN_LED_SWITCH_1    14 	
#define PIN_LED_SWITCH_2    14
#define PIN_LED_SWITCH_3  	14
#define PIN_LED_SWITCH_4    14

#define PIN_RGB1_LED_R      14
#define PIN_RGB1_LED_G      14
#define PIN_RGB1_LED_B      14

#define PIN_RGB2_LED_R      14
#define PIN_RGB2_LED_G      14
#define PIN_RGB2_LED_B      14

////////////////////////////////////////
// GPIO
#define PIN_RESET_NANO      6


////////////////////////////////////////
// LED
#define PIN_STRIPE_1        3 
#define PIN_STRIPE_2        2 

#define PIN_MATRIX_1        20
#define PIN_MATRIX_2        21

////////////////////////////////////////
// Micro
#define PIN_MIC_ANALOG      26
#define PIN_I2S_SD          22
#define PIN_I2S_SCK         27
#define PIN_I2S_WS          28

////////////////////////////////////////
// TFT & Touch / SPI
#define PIN_TFT_RESET       7
#define PIN_TFT_LED         8
#define PIN_TFT_DC          9
#define PIN_TOUCH_IRQ       11

#define PIN_SPI1_CS_TOUCH   10
#define PIN_SPI1_CS_TFT     13

/* defined in common.h for PICO board :
//SPI 1 
#define PIN_SPI1_SS         (13u)
#define PIN_SPI1_MISO       (12u)
#define PIN_SPI1_SCK        (14u)
#define PIN_SPI1_MOSI       (15u)
*/




////////////////////////////////////////
// SD
/* defined in common.h for PICO board :
//SPI 0 
#define PIN_SPI0_SS         17  
#define PIN_SPI0_MOSI       19
#define PIN_SPI0_MISO       16
#define PIN_SPI_SCK         18
*/

#define PIN_SD_CS           17
#define PIN_SD_MISO         16
#define PIN_SD_SCK          18
#define PIN_SD_MOSI         19


////////////////////////////////////////
// I2C (Nano & Buttons)
/* defined in common.h for PICO board :
// I2C
#define PIN_WIRE0_SDA       4
#define PIN_WIRE0_SCL       5
*/







