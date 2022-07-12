//Declarations for UART0

#ifndef __UART0_H__
#define __UART0_H__

// Function to initialize UART0 on Port A, to allow communication from PC through virtual COM port
void UART_Init(void);

// Function to send characters through UART0 to PC
void UART_OutChar(unsigned char character);

// Function to send a null-terminated string through UART0 to PC
void UART_OutString(unsigned char *str);

// Function to receive a character from UART0
unsigned char UART_InChar(void);

#endif // __UART0_H__
