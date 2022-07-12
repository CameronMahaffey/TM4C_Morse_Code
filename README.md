# TM4C_Morse_Code
Created using Keil uVision 4.7


Using a Tiva TM4C123GH6PM launchpad, characters are sent to the microcontroller using UART0 and encoded into Morse Code.
The output source for the Morse code can be either a buzzer, headphones, or be transmitted through a 433MHz transmitter.
Watch the video below for a demo or refer to the images in the folder.

Video Link -- Soon

## uC Pinout:

- Port A - UART Tx and Rx

  - PA0 - Rx
  
  - PA1 - Tx
  
- Port B - LCD Display Controls and Data Lines
  - PB0 - Register Select (RS on LCD)
  - PB1 - Not used
  - PB2 - Enable (E on LCD)
  - PB3 - Not used
  - PB4 - D4 on LCD
  - PB5 - D5 on LCD
  - PB6 - D6 on LCD
  - PB7 - D7 on LCD
- Port E - 6-bit DAC output
  - PE0 - 96kOhms
  - PE1 - 48kOhms
  - PE2 - 24kOhms
  - PE3 - 12kOhms
  - PE4 -  6kOhms
  - PE5 -  3kOhms
- Port F - signals for buzzer/tx output and LCD command received indicator
  - PF1 - Red LED   (LCD command sent indicator)
  - PF2 - Blue LED and Tx Signal (square wave)
  - PF3 - Green LED and Buzzer Signal (square wave)

## UART0

## DAC

## PLL
PLL is initialized for a system clock of 50MHz. All delay calculations for outputting to LCD and encoding Morse takes this into consideration.

## Morse

## Delay

## LCD

## Menu


