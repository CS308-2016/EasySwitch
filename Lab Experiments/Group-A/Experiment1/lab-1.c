/*
* Team Leader's Name : Nitish Chandra
* Author List: Saicharan, Siddhant
* Filename: lab-1.c
* Functions: setup(void), led_pin_config(void), switch_config(void), main(void)
* Global Variables: sw2Status, LOCK_F, CR_F
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"

#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))


int sw2Status = 0;

/*
* Function Name: setup
* Input: void
* Output: void
* Logic: sets the clock and enables the peripheral on port F
*/

void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}


/*
* Function Name: led_pin_config
* Input: void
* Output: void
* Logic: declares output of port F on pin 1,2,3 which correspond to leds
*/
void led_pin_config(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}

/*
* Function Name: switch_config
* Input: void
* Output: void
* Logic: removes the lock on sw2;
* sets pin-4 (sw2) as the input for the port F;
* set the IO current and mode on pin-4 and pin-0;
* sets pin-4 and pin-0 as input to port-F
*/
void switch_config(void)
{
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0,GPIO_DIR_MODE_IN); // Set Pin-4 of PORT F as Input. Modifiy this to use another switch
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}

int main(void)
{
	uint8_t h1,h = 0;
	uint8_t ui8LED=2;
	setup();
	switch_config();
	led_pin_config();
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
	while(1)
	{
		// incrementing sw2Status
		if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0))
					//Set PF1 as high. 2 ~ 0b00000010-->mask PF1
		{
			if(h1==0){
				sw2Status = sw2Status + 1;
				h1 = 1;	//setting h1 to 1 make sures that sw2Status is incremented only once for the entire key press
			}
		}
		else{
			h1 = 0;
		}
		// lighting up the LED
		if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)){
					GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
					h = 0; //setting h to 0 make sures that LED keeps glowing for the entire press period
		}
		else{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
			if(h == 0){ // changing the color
				if (ui8LED == 8)
				{
					ui8LED = 2;
				}
				else
				{
					ui8LED = ui8LED*2;
				}
				h = 1; //a new color is set and is ready for the next key press
			}
		}
	}
}
