//Defining functions for using and initializing UART0 on PortA

#include "UART.h"
#include "..//tm4c123gh6pm.h"

// Function to initialize UART0 on Port A, to allow communication from PC through virtual COM port
// baud rate of 115200 bps, at 50MHz clock speed, 8 bit word lenth, enable FIFOs, one stop bit
void UART_Init(void){
	unsigned long delay;
	// Set up GPIO Port A pins
	SYSCTL_RCGCUART_R |= 1;		// enable clock for UART0
	SYSCTL_RCGCGPIO_R |= 1;		// enable clock for Port A
	delay = SYSCTL_RCGCGPIO_R;	// delay for enable clock
	GPIO_PORTA_AMSEL_R &= ~0x03;	// disable analog mode on PA0, PA1
	GPIO_PORTA_AFSEL_R |= 0x03;	  // enable alternate functions for PA0, PA1
	GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00) + 0x11; // set PA0, PA1 to use UART0 Rx and Tx
	GPIO_PORTA_DEN_R = 0x03;			// digital enable PA0, PA1 
	//Set up UART0
	UART0_CTL_R &= ~(1);      // disable UARTEN bit 0
  UART0_IBRD_R = 27;                    // IBRD = int(50,000,000 / (16 * 115200)) = int(27.1267)
  UART0_FBRD_R = 8;                     // FBRD = round(0.1267 * 64) = 8
                                        // 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART0_LCRH_R = (7<<4);				// 111 into bits [4:6]
  UART0_CTL_R |= 1;       // enable UART
	
}

// Function to send characters through UART0 to PC
void UART_OutChar(unsigned char character){
	while(UART0_FR_R&0x20); // if 1, the transmit FIFO is full
	UART0_DR_R = character;
	
}

// Function to send a null-terminated string through UART0 to PC
void UART_OutString(unsigned char *str){
	
	while(*str != '\0'){
		UART_OutChar(*str);
		str++;
	}
}

// Function to receive a character from UART0
unsigned char UART_InChar(void){
  while((UART0_FR_R&UART_FR_RXFE) != 0);	// if 1, receive FIFO is empty
  return((unsigned char)(UART0_DR_R&0xFF));
}
