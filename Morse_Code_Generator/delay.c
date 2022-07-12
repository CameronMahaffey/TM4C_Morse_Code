// Delay functions to give LCD time to finish processing commands and for sending Morse codes
#include "delay.h"

// Delays in units of microseconds
// 50MHz is 20ns, takes 6 clock cycles per while loop, 50/6 * 20ns = ~8
void delay_us(unsigned short usec){
	unsigned short count;
	while(usec){
		count = 8; //approx 1us at 50MHz
		while(count){
			count--;
		}
		usec--;
	}
}

// Delays in units of milliseconds
// 50MHz is 20ns, takes 6 clock cycles per while loop, 50000/6 * 20ns = ~8333
void delay_ms(unsigned short msec){
	unsigned short count;
	while(msec){
		count = 8333;	//approx 1ms at 50MHz
		while(count){
			count--;
		}
		msec--;
	}
	}
