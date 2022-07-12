// DAC.h
// Header file for the 6-bit digital to analog converter


#ifndef __DAC_H__
#define __DAC_H__

// Initialize 6-bit DAC on Port E
void DAC_Init(void);

//output to DAC
void DAC_Out(unsigned char data);

#endif // __DAC_H__
