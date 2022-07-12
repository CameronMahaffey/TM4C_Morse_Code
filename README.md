# TM4C_Morse_Code
Created using Keil uVision 4.7


Using a Tiva TM4C123GH6PM launchpad, characters are sent to the microcontroller using UART0 and encoded into Morse Code. The output source for the Morse code can be either a buzzer, headphones, or be transmitted through a 433MHz transmitter. The sound pitch and speed in words per minute can also be selected on startup. Watch the video below for a demo or refer to the images in the folder.

Program Flow: Wait for characters to to sent through UART, add characters to buffer and onto LCD. If character is enter key, prepare buffer for transmission by removing extra spaces between words and at the front of buffer. If buffer is empty, don't transmit anything and go back to waiting for characters through UART, else begin tranmission. Once finished, Clear the LCD and buffer, then repeat.

Video Link -- Soon

## TOC:
1. uC Pinout
2. UART0
3. DAC
4. PLL
5. Morse
6. LCD
7. Delay
8. Menu


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
Pins PA0 and PA1 are connected through the usb cable to the PC to form a virual serial COM port. Using a software like PuTTY, we can communicate between the PC and the microcontroller using the UART protocol. Text is sent to the uC through the virtual COM port and echoed back to the screen. Certain keys, like enter, backspace, [, ], and esc, perform other actions. The function UART_InChar() polls the Rx FIFO Empty register until data is present, then it receives the character. Likewise, the UART_OutChar() function checks to see if the Tx FIFO Full register is full and waits for it to make space before sending data back to the PC.

UART Settings: 115,200 baud rate, 8 data bits, 1 stop bit, no parity.

In PuTTY - change the backspace key to Control-H mode. Use device manager to find out which COM port your uC is connected to.

## DAC
The 6-bit DAC is created using Port E on the microcontroller. Pins PE0 - PE5 are connected to resisters such that each resistor value is twice the resistance as the previous one. PE5 is connected to 3kOhms, PE4 to 6kOhms, PE3 to 12kOhms, PE2 to 24kOhms, PE1 to 48kOhms, and PE0 to 96kOhms. The other end of each resistor is connected in parallel. In this way, there is a linear relationship between each level of the DAC and the voltage output. With 6 bits, there are 64 discrete levels. With 3.3V being the maximum voltage, this gives us a resolution of 3.3/64 = 51.6mV. Using the DAC, we can output a 6-bit sine wave to the headphones.

## PLL
The PLL is initialized for a system clock of 50MHz. All delay calculations for outputting to the LCD and encoding Morse takes this into consideration.

## Morse
Once the enter key is pressed, the string of characters is prepared for transmitting and encoded into the morse code equivalent - as long as the string is not empty. Using the PARIS test, the speed in WPM was calculated, with a dot being the shortest element. Other elements, like the dash, spaces between characters in the same word, and spaces between words, was calculated using the dot as the base element.

Each character goes through the function character_To_Morse(char), which uses a switch statement to find the Morse equivalent.

- dot - base element
- dash - 3 dots
- space b/t elements in character - 1 dot
- space b/t characters in word - 3 dot
- space b/t words - 7 dots

## Delay
The delay functions are useful for timing the Morse code and for waiting between outputting to the LCD. Most of the LCD commands need 40 microseconds to execute, but two of them take at least 1.5 milliseconds, so it's important to make sure the uC doesn't send data or commands to the LCD during this time.

## LCD
The LCD displays each typed character, as well as action messages. The Menu is also displayed on the screen during startup or reset. The LCD works in 4-bit display mode, so only 4 data lines are needed instead of 8. The display allows for up to 80 characters to be displayed between both lines and thus, a maximum of only 80 characters can be transmitted in one message. The display will shift to follow the cursor.

## Menu
The Menu is displayed on the LCD on reset and startup. The Menu is used for selecting the output source, frequency, and speed in WPM. Pressing 1,2,3, or 4 will select the respective option.

Options:
- Speed(WPM): 10, 15, 20, 25
- Frequency(Hz): 600, 800, 900, 1000
- Output Source: Headphones(DAC), Buzzer(Square wave), 433MHz Transmitter(Square wave)

