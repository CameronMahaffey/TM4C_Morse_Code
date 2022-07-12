/* Functions for initializing and interacting with the 16x2 LCD display in 4-bit mode
   LCD Datasheet - https://www.openhacks.com/uploadsproductos/eone-1602a1.pdf

	PINOUTS:
	 1 - GND
	 2 - 5V
	 3 - Contrast through 10K potentiometer
	 4 - Register Select, are we sending data or commands?
	 5 - Read/Write
	 6 - Enable
	 7 - Data 0
	~
	14 - Data 7
	15 - LED Positive (Anode)
	16 - LED Negative (Cathode)
*/

#ifndef __LCD_H__
#define __LCD_H__

// Structure to hold values important for displaying on the LCD Display
typedef struct LCD{
	signed char length;
	unsigned char offset;
	unsigned char top, bottom;
} LCD;

/* bit masks for Register Select and Enable bits */
#define RS 0x01 /* PORTB BIT0 mask */
#define RW 0x02 /* PORTB BIT1 mask */
#define EN 0x04 /* PORTB BIT2 mask */

/*define useful symbolic names for LCD commands */
#define clearDisplay      								0x01
#define returnHome        								0x02
#define entryModeCursorLNoShift        		0x04
#define entryModeCursorRNoShift        		0x06
#define entryModeCursorRShiftL        		0x07
#define displayOff    	  								0x08
#define displayOn   	    								0x0C
#define cursorBlink       								0x0F
#define cursorOff         								0x0C
#define cursorOn          								0x0E
#define shiftDisplayRight 								0x1C
#define shiftDisplayLeft  								0x18
#define set4BitMode				 								0x28
#define set8BitMode	 											0x38
#define set5x7FontSize    								0x20
#define firstRow          								0x80
#define secondRow         								0xC0

// Function for initializing the Port and setting up the LCD
void LCD_Init(void);

//Function for writing to the display
void LCD_Send_Nibble(unsigned char data, unsigned char control);

// Function for sending commands to the LCD controller
void LCD_Send_Cmd(unsigned char command);

// Function for writing characters to the LCD display
void LCD_Write_Char(unsigned char letter);

// Function for writing a string to the LCD Display
void LCD_String(unsigned char str[]);

// Function to check each character to determine the next sequence of events
unsigned char Check_Char(unsigned char character);

// Function for implementing display shift and switching rows, as well as setting bounds for under/overflow
unsigned char Check_Index(void);

// Function for clearing LCD and resetting the length/top/bottom parameters for display shifting purposes
void Clear_LCD(void);

// Function to implement backspace on LCD, deletes previous index
void backspace_Key(void);

// Function to implement escape key on LCD, deletes current index
void escape_Key(void);

// Function to move cursor left on LCD, without deleting current index
void cursor_Left(void);

// Function to move cursor right on LCD, without deleting current index
void cursor_Right(void);

#endif //LCD_H
