// Function declarations for initializing and using LEDS on Port F

#ifndef __PORTF_LED_SW_H__
#define __PORTF_LED_SW_H__


// Macros for PF LEDS 
#define RED 0x02	// red LED is PF1
#define BLU 0x04  // blue LED is PF2
#define GRN 0x08  // green LED is PF3

// Functions to enable PortF LEDs for debugging
void PortF_Init(void);

// Toggle corrsponding PF LED
void Toggle_PF_LED(unsigned char LED);

#endif //__PORTF_LED_SW_H__
