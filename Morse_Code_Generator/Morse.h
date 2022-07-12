// Functions for turning the sound on/off initializing the Systick, and encoding characters in Morse code

#ifndef __MORSE_H__
#define __MORSE_H__
																			
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
void Sound_Init(void);

// Change Systick period to start sound output
void Sound_Tone(unsigned long period);

// Stop sound output
// buzzer is connected to PF3 and Tx is connected to PF2
void Sound_Off(void);

// Play a dot at selected frequency
void Play_Dot(void);

// Play a dash at selected frequency, a dash is 3 times longer than a dot
void Play_Dash(void);

// Function for setting the WPM on reset
void set_WPM(unsigned char wpm);

// Function for setting the sound frequency on reset
void set_Freq(unsigned long freq);

// Function for setting the output source on reset
void set_Mode(unsigned char mode);

// Interrupt service routine
// Executed every 20ns*(period)
void SysTick_Handler(void);

// Function for encoding each character into morse code during transmit
void character_To_Morse(unsigned char character, unsigned char next_char);

#endif //__MORSE_H__
