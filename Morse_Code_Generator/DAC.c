// Functions for initializing and using DAC for outputting sound to headphones
// Port E bits 5-0 have the 6-bit DAC

#include "DAC.h"
#include "..//tm4c123gh6pm.h"

// Initialize 6-bit DAC on Port E
void DAC_Init(void){
	unsigned long volatile delay;
	SYSCTL_RCGCGPIO_R |= (1<<4); // (a) activate clock for port E
	delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTE_DIR_R |= 0x3F;    // (c) make PE5-0 outputs
	GPIO_PORTE_DR8R_R |= 0x0F;
  GPIO_PORTE_AFSEL_R &= ~0x3F;  //     disable alt funct on PE5-0
  GPIO_PORTE_DEN_R |= 0x3F;     //     enable digital I/O on PE5-0   
  GPIO_PORTE_PCTL_R &= ~0x00FFFFFF; // configure PE5-0 as GPIO
  GPIO_PORTE_AMSEL_R = 0;       //     disable analog functionality on PE
}

// output to DAC
// data is 6-bit data, 0 to 63 (2^6 = 64)
void DAC_Out(unsigned char data){
  GPIO_PORTE_DATA_R = data&0x3F;	// mask with 0x3F for only 6 bit data
}
