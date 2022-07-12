// Function definitions for initializing and using LEDS on Port F

#include "PortF_LED_SW.h"
#include "../tm4c123gh6pm.h"

// Functions to enable PortF1 for debugging
void PortF_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGCGPIO_R |= (1<<5);     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay         
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) Make PF1,2,3 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x0E;          // enable pullup resistors on PF1,2,3      
  GPIO_PORTF_DEN_R = 0x0E;          // 7) enable digital pins PF1,2,3
}

// Toggle corresponding PF LED
void Toggle_PF_LED(unsigned char LED){
	GPIO_PORTF_DATA_R ^= LED;
}
