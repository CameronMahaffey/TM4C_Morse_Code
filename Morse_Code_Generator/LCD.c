// Functions for initializing and interacting with the 16x2 LCD display in 4-bit mode
//LCD Datasheet - https://www.openhacks.com/uploadsproductos/eone-1602a1.pdf

#include "LCD.h"
#include "..//tm4c123gh6pm.h"
#include "delay.h"
#include "PortF_LED_SW.h"

LCD Display;		// LCD struct defined in LCD.h
// arrays to store characters on LCD and for preparing transmit of Morse
unsigned char String[80];


/* Function for initializing the Port B and setting up the LCD
   B0 - Register Select
   B1 - Read/Write
   B2 - Enable
	 B4 through B7 - Data 4 through 7
*/
void LCD_Init(void){
	// initializing GPIO
	unsigned long delay;
	SYSCTL_RCGCGPIO_R |=(1<<1);  // Turn on Port B Clock
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTB_DIR_R = 0xFF;  	 // all pins on Port B are outputs
	GPIO_PORTB_DEN_R = 0xFF;		 // all pins on Port B are digital
	GPIO_PORTB_AMSEL_R = 0x00;	 // not analog
	GPIO_PORTB_AFSEL_R = 0x00;	 // not alternate functions
  GPIO_PORTB_PCTL_R = 0x00000000;   // 4) not setting alt function for GPIO	
	
	//LCD commands
	LCD_Send_Cmd(set5x7FontSize);  /* select 5x7 font size and 2 rows of LCD */
	LCD_Send_Cmd(set4BitMode); /* Select 4-bit Mode of LCD */
	LCD_Send_Cmd(entryModeCursorRNoShift); /* shift cursor right */
	//LCD_Send_Cmd(clearDisplay); /* clear whatever is written on display */
	LCD_Send_Cmd(cursorBlink);  /* Enable Display and cursor blinking */
}


//Function for sending data or commands to the LCD display in 4 bit mode
void LCD_Send_Nibble(unsigned char data, unsigned char control){
	data &= 0xF0; // needed to extract the upper 4 bits of data
	GPIO_PORTB_DATA_R = data | control;  // set the Port B pins to send data and set control bit
	GPIO_PORTB_DATA_R = data | control | EN;						 // pulse enable bit
	delay_us(1);
	GPIO_PORTB_DATA_R = data;						
	GPIO_PORTB_DATA_R = 0;
	
}

// Function for sending commands to the LCD controller
void LCD_Send_Cmd(unsigned char command){
	LCD_Send_Nibble(command, 0);   /* Write upper nibble to LCD */
  LCD_Send_Nibble(command << 4, 0);     /* Write lower nibble to LCD */
	
	if (command < 4){
		delay_ms(2);		// first two commands take max of 1.5ms to complete
	}
	else {
		delay_us(40);		// other commands take 37us to complete
	}
}

// Function for writing characters to the LCD display
void LCD_Write_Char(unsigned char letter){
	Toggle_PF_LED(RED);
	LCD_Send_Nibble(letter & 0xF0, RS);   /* Write upper nibble to LCD */
  LCD_Send_Nibble(letter << 4, RS);     /* Write lower nibble to LCD */
	delay_us(40);
	Toggle_PF_LED(RED);
}

void LCD_String(unsigned char str[]){ /* Send string to LCD function */
	int i;
	for(i=0;str[i]!=0;i++)  /* Send each char of string till the NULL */
	{
		LCD_Write_Char(str[i]);  /* Call LCD data write */
		//delay_ms(300);
	}
}

// Function for clearing LCD and resetting the length/top/bottom parameters for display shifting purposes
void Clear_LCD(void){
	LCD_Send_Cmd(clearDisplay);
	Display.length = 0;
	Display.top = 15;
	Display.bottom = 0;
}

// Function to check each character to determine the next sequence of events
unsigned char Check_Char(unsigned char character){
	if (character == 0x08){			// if character is backspace
		backspace_Key();
	}
	else if (character == 0x1b){	// if character is escape, delete current character
		escape_Key();
	}
	else if (character == 0x5b){	// if character is [, move cursor left 1 character
		cursor_Left();
	}
	else if (character == 0x5d){	// if character is ], move cursor right 1 character
		cursor_Right();
	}
	else if (character == 0x0d){	// if character is enter, prepare transmit buffer for sending Morse code
		return 1;
	}
	else {	// if none above apply, then we add character to String array and wait for next character
		Display.length++;
		if (Check_Index()){
			LCD_Write_Char(character);
			String[Display.length-1] = character;
		}
	}
		return 0;
}

// Function for implementing display shift and switching rows, as well as setting bounds for under/overflow
unsigned char Check_Index(void){
	if (Display.length < 0){		// no backspacing or moving cursor past 0th index
		Display.length = 0;
		return 0;
	}
	if (Display.length > 80){		// no adding chars or moving cursor past 79th index
		Display.length = 80;
		return 0;
	}
	if (Display.length > 39){		// there is an offset of 24 between first and second row
		Display.offset = 24;
	}
	else {
		Display.offset = 0;
	}
	if (Display.length > Display.top){		// if length moves past or below top/bottom index, shift display accordingly
		LCD_Send_Cmd(shiftDisplayLeft);
		Display.top++;
		Display.bottom++;
	}
	if (Display.length < Display.bottom){
		LCD_Send_Cmd(shiftDisplayRight);
		Display.top--;
		Display.bottom--;
	}
	return 1;
}

// Function to implement backspace on LCD, deletes previous index
void backspace_Key(void){
	Display.length--;		// decrease length
		if (Check_Index()){	// make sure length is not negative and shift display if needed
			LCD_Send_Cmd(firstRow+Display.length+Display.offset);		//go to previous character index
			LCD_Write_Char(' ');		// delete character off of LCD
			String[Display.length] = ' ';	 // replace character with ' ' in array
			LCD_Send_Cmd(firstRow+Display.length+Display.offset);		// writing auto increments so go back again
		}
}

// Function to implement escape key on LCD, deletes current index
void escape_Key(void){
	LCD_Write_Char(' ');
	String[Display.length] = ' ';
	LCD_Send_Cmd(firstRow+Display.length+Display.offset);		//writing auto increments so go to previous character index
}

// Function to move cursor left on LCD, without deleting current index
void cursor_Left(void){
	Display.length--;
	if (Check_Index()){
		LCD_Send_Cmd(firstRow+Display.length+Display.offset);		//go to previous character index
	}
}

// Function to move cursor right on LCD, without deleting current index
void cursor_Right(void){
	Display.length++;
	if (Check_Index()){
		LCD_Send_Cmd(firstRow+Display.length+Display.offset);		//go to previous character index
	}
}
