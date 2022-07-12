// Function definitions for setting up PLL

#include "PLL.h"
#include "..//tm4c123gh6pm.h"

// Function to initialize and enable PLL
void PLL_Init(void){
	// Configure to use RCC2 Register for advanced features like 400MHz PLL
	SYSCTL_RCC2_R |= 0x80000000; //bit 31 USERCC2 set
	// Bypass PLL for setup in RCC2
	SYSCTL_RCC2_R |= (1<<11); // bit 11 BYPASS2 set
	// Clear XTAL field [6:10] and select 16MHZ (0x15) crystal in RCC
	SYSCTL_RCC_R = (SYSCTL_RCC_R&0xFFFFF83F)+(0x15<<6);
	// Clear OSCSRC2 field [4:6] and select MOSC (0x0) in RCC2
	SYSCTL_RCC2_R &= ~0x00000070;
	// Activate PLL by clearing PWRDN2 bit in RCC2
	SYSCTL_RCC2_R &= ~(1<<13);  // bit 13 PWRDN2 cleared
	// Set DIV400 to configure PLL frequency
	SYSCTL_RCC2_R |= (1<<30);	// bit 30 DIV400 set
	// Clear then set SYSDIV2 field [22:28] (when DIV400 set) to configure PLL frequency
	SYSCTL_RCC2_R = (SYSCTL_RCC2_R&0xE03FFFFF)+(SYSDIV2<<22); // SYSDIV2 = 7
	// Wait for PLL to lock (PLLLRIS bit 6) in RIS
	while((SYSCTL_RIS_R&0x00000040) == 0);
	// Enable PLL
	SYSCTL_RCC2_R &= ~(1<<11); // bit 11 BYPASS2 cleared
}
