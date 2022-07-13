/*
The purpose of this project is to initialize and use a 16x2 LCD with the TM4C using 4-bit mode.
Characters will be sent from the PC using UART0 through the virtual com port, where they will be
written onto the LCD screen. Once an ENTER is received from the UART, the message is prepared and 
transmitted as Morse code through one of the selected output sources: headphones, buzzer, or 433Mhz
transmitter. The frequency and WPM can also be set in the Menu displayed on reset.

The clock is sourced through the PLL set to 50MHz.
The baud rate for the UART is 115,200 bps, 8 bit data mode, 1 stop bit, no parity
*/

#include "LCD.h"
#include "PLL.h"
#include "UART.h"
#include "delay.h"
#include "Morse.h"
#include "PortF_LED_SW.h"

// arrays to store characters on LCD and for preparing transmission of Morse
extern unsigned char String[80];
unsigned char TxBuffer[80];

// placeholder function SystemInit, startup file purposes, not needed in this project
void SystemInit(void);

// Function for preparing the Transmit buffer, deleting unnecessary spaces
unsigned char Prepare_Buffer(void);

// Transmit function that loops through each character in TxBuffer and plays the Morse code equivalent
void Transmit(void);

// After each transmission, clear both arrays to prepare for next tranmission
void Clear_Buffer(unsigned char arr[]);

// On reset and startup, the Menu allows for selecting the output source, speed in WPM, and sound pitch
void Menu(void);

// Functions for enabling and disabling interrupts in processor, see startup file
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

int main(void){
	unsigned char character;
	unsigned char firstChar;
	unsigned char txReady;
	
	// Initialize all Ports, Peripherals and LCD screen
  PortF_Init();
	UART_Init();
	PLL_Init();
	LCD_Init();
	Sound_Init();
	
	Clear_LCD();
	Menu();		// Display Menu options in UART and LCD
	LCD_String("Type Here...");
	UART_OutString("Type to Send Data: ");
	LCD_Send_Cmd(firstRow);		// move cursor to first LCD index
	firstChar = 0;
	txReady = 0;
	EnableInterrupts();
	
	while(1){
		// Get new character
		character = UART_InChar();	// most time is spent inside UART_InChar function waiting for characters from UART
		// if its first character, clear_LCD before putting on screen to remove "Type Here..."
		if (firstChar == 0){	// clear LCD when first character is typed
			firstChar = 1;
			Clear_LCD();
		}
		UART_OutChar(character);		// echo character back through UART
		txReady = Check_Char(character);		// series of functions to determine next action
		if (txReady){ // if character was an enter, then prepare transmit buffer for playing Morse
			if (Prepare_Buffer()){		// checks to see if String array was empty, if not, then transmit message
				Transmit();
			}
			Clear_Buffer(String);		// clear String array for next message
			UART_OutString("Type to Send Data: ");
		}
	}
}

// Function to prepare the Tx Buffer array, remove extra spaces in between words and at beginning of sequence
// Once the first null is encountered in String, preparation is finished
unsigned char Prepare_Buffer(void){
	int previous_Space;
	int index;
	int index_offset;		
	int first_sequence;
	index_offset = previous_Space = 0;
	first_sequence = 1;
	for (index = 0; index < 80; index++){
		if (String[index] != 0){
			if ((String[index] == ' ') && (first_sequence == 1)){ 	// removes spaces in beginning
				index_offset++;
				continue;
			}
			else if ((String[index] == ' ') && (previous_Space == 1)){		// removes extra spaces between words
				index_offset++;
				continue;
			}
			else if (String[index] == ' '){		// set previous_Space for next iteration to check if multiple spaces
				previous_Space = 1;
			}
			else {
				first_sequence = 0;	
				previous_Space = 0;
			}
				TxBuffer[index-index_offset] = String[index];
		}
		else {		// if no data present in String array, cannot transmit anything
			if (index == 0){
				Clear_LCD();
				LCD_String("Error! Empty");
				UART_OutString("\n\rError! Empty\n\r");
				delay_ms(1000);
				Clear_LCD();
				return 0;
			}
			break;
		}
	}
	return 1;
}

// Function for clearing arrays after tranmission to prepare for next message
void Clear_Buffer(unsigned char arr[]){
	int index;
	for (index = 0; index < 80; index++){		// set all elements in array to NULL
		if (arr[index] == 0) break;
		arr[index] = 0;
	}
}

// Function for transmitting message character by character
void Transmit(void){
	int index;
	Clear_LCD();
	LCD_String("Transmitting...");
	UART_OutString("\n\rTransmitting...\n\r");
	delay_ms(1000);
	Clear_LCD();
	LCD_String(TxBuffer);
	LCD_Send_Cmd(firstRow);
	for (index = 0; index < 80; index++){		// for each character, play its Morse equivalent
		if (TxBuffer[index] == 0) break;
		character_To_Morse(TxBuffer[index], TxBuffer[index+1]);
		cursor_Right();
	}
	Clear_LCD();
	LCD_String("Finished.");
	UART_OutString("Finished.\n\r");
	Clear_Buffer(TxBuffer);
	delay_ms(1000);
	Clear_LCD();
}

// Function for choosing Menu options - Setting WPM, frequency, and output source
// defaults to buzzer, 15WPM, 600Hz
void Menu(void){
	unsigned char choice;
	LCD_String("Buzz-1  Tx-2");
	LCD_Send_Cmd(secondRow);
	LCD_String("Headphones-3");
	UART_OutString("Choose Output: Buzzer - 1    Tx - 2    Headphones - 3\n\r"); 
	choice = UART_InChar();
	
	if (choice == '1') set_Mode(1);
	else if (choice == '2') set_Mode(2);
	else if (choice == '3') set_Mode(3);
	else set_Mode(1);
	
	Clear_LCD();
	LCD_String("10WPM-1 15WPM-2");
	LCD_Send_Cmd(secondRow);
	LCD_String("20WPM-3 25WPM-4");
	UART_OutString("Choose WPM: 10WPM-1   15WPM-2   20WPM-3   25WPM-4\n\r");
	choice = UART_InChar();
	
	if (choice == '1') set_WPM(122);
	else if (choice == '2') set_WPM(81);
	else if (choice == '3') set_WPM(60);
	else if (choice == '4') set_WPM(48);
	else set_WPM(81);
	
	Clear_LCD();
	LCD_String("600Hz-1 800Hz-2");
	LCD_Send_Cmd(secondRow);
	LCD_String("900Hz-3 1000Hz-4");
	UART_OutString("Choose Frequency: 600Hz-1   800Hz-2   900Hz-3   1000Hz-4\n\r");
	choice = UART_InChar();
	
	if (choice == '1') set_Freq(41600);
	else if (choice == '2') set_Freq(31232);
	else if (choice == '3') set_Freq(27776);
	else if (choice == '4') set_Freq(25088);
	else set_Freq(651);
	
	Clear_LCD();
}


// Placeholder function, not utilized for this project
void SystemInit(void){
}
