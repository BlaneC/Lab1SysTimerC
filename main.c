

/*counter main.c

Runs on TM4C1294 LaunchPad
Using four LEDs to as a 15-section timer
Using system timer to implement 1s time delay

Author: zhou zhou
Date: July 22, 2017
PN0 LED indicates nQ
PN1 LED indicates Q
*/

#include "TM4C1294NCPDT.h"
#define GPIO_PORTN_DATA_R (*((volatile uint32_t *) 0x400643FC))
#define GPIO_PORTN_DIR_R (*((volatile uint32_t *) 0x40064400))
#define GPIO_PORTN_DEN_R  (*((volatile uint32_t *) 0x4006451C))

#define GPIO_PORTF_DATA_R	(*((volatile uint32_t *) 0x4005D3FC))
#define GPIO_PORTF_DIR_R	(*((volatile uint32_t *) 0x4005D400))
#define GPIO_PORTF_DEN_R	(*((volatile uint32_t *) 0x4005D51C))
#define GPIO_PORTF_AFSEL_R	(*((volatile uint32_t *)	0x4005D420))

#define SYSCTL_RCGCGPIO_R (*((volatile uint32_t *) 0x400FE608))

#define NVIC_ST_CTRL_R		(*((volatile uint32_t *) 0xE000E010))
#define	NVIC_ST_RELOAD_R	(*((volatile uint32_t *) 0xE000E014))
#define	NVIC_ST_CURRENT_R	(*((volatile uint32_t *) 0xE000E018))

void delay(int);
void GPIO_Init(void);
void SysTick_Init(void);
void SysTick_Wait(unsigned long);
void SysTick_Wait1s(unsigned long);
void DisplayCount(unsigned int count);


void GPIO_Init(void){
  SYSCTL_RCGCGPIO_R = 0x1120; // enable clock
	while((SYSCTL_RCGCGPIO_R &0x1120) ==0){}
	GPIO_PORTN_DIR_R |= 3;						// enable both LED pins for output
	GPIO_PORTN_DEN_R |= 3;						// enable both LEDs for digital output
	
	GPIO_PORTF_DIR_R |= 0x0011; 			// enable both LED pins for output
	GPIO_PORTF_DEN_R |= 0x0011; 			// enable both LEDs for digital output
}
/****************SysTick_Init**********
clock:16 MHz
Maximum reload value: 2^24-1
***************************************/
void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;							// disable SysTick
	NVIC_ST_RELOAD_R = 0xFFFFFF; // the maximum value
	NVIC_ST_CURRENT_R = 0;				// set current value is 0
	NVIC_ST_CTRL_R = 1;				// enable SysTick
}


void SysTick_Wait(unsigned long delay){
	NVIC_ST_RELOAD_R = 0; // reset RELOAD register value 
	NVIC_ST_CURRENT_R = delay;	// set current register=0 make the timer to start counting
	while((NVIC_ST_CTRL_R & 16) == 0){		//check if the count flag has set (We have finished the count)
	}
}

void SysTick_Wait1s(unsigned long delay){
	unsigned long i;
	for(i=0;i<delay;i++){
		SysTick_Wait(16000000); // wait for 1 second
	}
}


void DisplayCount(unsigned int count){
	
	uint8_t LED1and2;
	
	LED1and2 = 0x03 & count;		// Mask the first two bits
	GPIO_PORTN_DATA_R = LED1and2;		// Set LEDs (Quick and dirty. Should not reset every GPIO on this register, needs a proper mask)

	//Both LED
	if(count & 0xC)
		GPIO_PORTF_DATA_R = 0x0011;		// Set LEDs
	
	// LED 4
	else if(count & 8)
		GPIO_PORTF_DATA_R = 0x0010;		// Set LEDs
	
	// LED 3
	else if(count & 4)
		GPIO_PORTF_DATA_R = 0x0001;		// Set LEDs

}//DisplayCount


int main(){
	GPIO_Init();
	SysTick_Init();
	//needs 4 variables to store bit value for each LED (Do I really?)
	unsigned int value = 0;
	while(1){
		while(value<15){
			SysTick_Wait1s(1);
			DisplayCount(value);
			value++;
		}
		value = 0;
	}
}

 
