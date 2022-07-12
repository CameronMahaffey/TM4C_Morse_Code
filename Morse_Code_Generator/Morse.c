// Functions for turning the sound on/off initializing the Systick, and encoding characters in Morse code

#include "Morse.h"
#include "DAC.h"
#include "delay.h"
#include "PortF_LED_SW.h"
#include "..//tm4c123gh6pm.h"

// globals for setting the output mode, selecting the words per minute, and sound pitch
unsigned char mode, wpm;
unsigned long freq;

// 128 samples of a basic sine wave for DAC (headphones)
const unsigned long SineWave[128] = {32,33,35,36,38,39,41,42,44,45,46,48,49,50,51,53,
																	   54,55,56,57,58,59,59,60,61,61,62,62,62,63,63,63,
																		 63,63,63,63,62,62,62,61,61,60,59,59,58,57,56,55,
																		 54,53,51,50,49,48,46,45,44,42,41,39,38,36,35,33,
																		 32,30,28,27,25,24,22,21,19,18,17,15,14,13,12,10,
																		  9, 8, 7, 6, 5, 4, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0,
																		  0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 8,
																		  9,10,12,13,14,15,17,18,19,21,22,24,25,27,28,30};

// globals for cycling through SineWave array and for turning the sound on/off in Systick handler
unsigned long Index;									
unsigned char OnOff;

// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
void Sound_Init(void){
  DAC_Init();		// initialize the DAC
	Index = 0;
	// initialize systick
	NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
	NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
                              // enable SysTick with core clock and interrupts
  NVIC_ST_CTRL_R = 0x07;
}


// Change Systick period to start sound output
void Sound_Tone(unsigned long period){
		if (mode != 3){		// if TX or Buzzer mode
		period *= 128;		// headphones need systick to be 128 times faster for same Hz as buzzer/tx mode
		}
	// this routine sets the RELOAD and makes SysTick handler operable
		NVIC_ST_RELOAD_R = period-1;// set reload value to period
		NVIC_ST_CURRENT_R = 0;      // any write to current clears it
		OnOff = 1;		// allows sound to play in Systick handler
}

// Stop sound output
// buzzer is connected to PF3 and Tx is connected to PF2
void Sound_Off(void){
	OnOff = 0;		// make Systick handler ineffective
 // this routine stops the sound output
	if (mode == 2){		// if Tx mode
		if (GPIO_PORTF_DATA_R&BLU){  // toggle PF2
			Toggle_PF_LED(BLU);
		}
	}
	else if (mode == 1){		// else if buzzer mode
		if (GPIO_PORTF_DATA_R&GRN){		// toggle PF3
			Toggle_PF_LED(GRN);
		}
	}
	else {		// if headphones mode
		Index = 0;		// reset index 
		DAC_Out(0);		// make headphone output 0V
	}
}

// Play a dot at selected frequency
void Play_Dot(void){
	Sound_Tone(freq);
	delay_ms(wpm);
	Sound_Off();
}

// Play a dash at selected frequency, a dash is 3 times longer than a dot
void Play_Dash(void){
	Sound_Tone(freq);
	delay_ms(3*wpm);
	Sound_Off();
}

// Interrupt service routine
// Executed every 20ns*(period)
void SysTick_Handler(void){
	if (OnOff){		// check if sound shoud be on or off
		if (mode == 1){		// if buzzer mode
			Toggle_PF_LED(GRN);
		}
		else if (mode == 2){	// else if Tx mode
			Toggle_PF_LED(BLU);
		}
		else if (mode == 3){		// else if headphones mode
			Index = (Index+1)&0x7F;      // reset at 127 (0-128 elements in SineWave array)
			DAC_Out(SineWave[Index]);    // output once each interrupt
		}
	}
}

// Function for setting the WPM on reset
void set_WPM(unsigned char w){
	wpm = w;
}

// Function for setting the sound frequency on reset
void set_Freq(unsigned long f){
	freq = f;
}

// Function for setting the output source on reset
void set_Mode(unsigned char m){
	mode = m;
}

// Function for encoding each character into morse code during transmit
void character_To_Morse(unsigned char character, unsigned char next_char){
	int is_space;	// if 1, then next character is a space
	is_space = 0;
	if (next_char == ' ') is_space = 1;		// checking next character in array
	switch (character){
		case ' ':
		{
			delay_ms(7*wpm);		// a space between words is 7 times longer than a dot
			break;
		}
		case 'a':
		case 'A':		// . _
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case 'b':
		case 'B':		// _ . . .
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case 'c':
		case 'C':		// _ . _ .
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case 'd':
		case 'D':		// _ . .
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case 'e':
		case 'E':		// .
		{
			Play_Dot();
			break;
		}
		case 'f':
		case 'F':		// . . _ .
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();			
			break;
		}
		case 'g':
		case 'G':		// _ _ .
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case 'h':
		case 'H':		// . . . .
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case 'i':
		case 'I':		// . .
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case 'j':
		case 'J':		// . _ _ _
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case 'k':
		case 'K':		// _ . _
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();			
			break;
		}
		case 'l':
		case 'L':		// . _ . .
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case 'm':
		case 'M':		// _ _
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();			
			break;
		}
		case 'n':
		case 'N':		// _ .
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case 'o':
		case 'O':		// _ _ _
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case 'p':
		case 'P':		// . _ _ .
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case 'q':
		case 'Q':		// _ _ . _
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case 'r':
		case 'R':		// . _ .
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case 's':
		case 'S':		// . . .
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case 't':
		case 'T':		// _
		{
			Play_Dash();
			break;
		}
		case 'u':
		case 'U':		// . . _
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case 'v':
		case 'V':		// . . . _
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case 'w':
		case 'W':		// . _ _ 
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case 'x':
		case 'X':		// _ . . _
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case 'y':
		case 'Y':		// _ . _ _
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case 'z':
		case 'Z':		// _ _ . .
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case '0':		// _ _ _ _ _
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case '1':		// . _ _ _ _
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case '2':		// . . _ _ _
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case '3':		// . . . _ _
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case '4':		// . . . . _
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case '5':		// . . . . .
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case '6':		// _ . . . .
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case '7':		// _ _ . . .
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case '8':		// _ _ _ . .
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case '9':		// _ _ _ _ .
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case '!':		// _ . _ . _ _
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case '?':		// . . _ _ . .
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case ',':		// _ _ . . _ _
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case '.':		// . _ . _ . _
		{
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			break;
		}
		case ';':		// _ . _ . _ .
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
		case ':':		// _ _ _ . . .
		{
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dash();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			delay_ms(wpm);
			Play_Dot();
			break;
		}
	}
	if (is_space == 0) delay_ms(3*wpm);	// if space between characters of same word, silent for 3 times longer than dot
}
