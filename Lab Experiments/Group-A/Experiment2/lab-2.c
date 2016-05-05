/*

* Team Leader's Name : Nitish Chandra
* Author List: Saicharan, Siddhant
* Filename: lab-2.c
* Functions: setup(void), led_pin_config(void), timer_config(void), switch_config(void), int_enable(void),
 , main(void), Timer0IntHandler(void)
* Global Variables: sw2Status,sw2var,ui8LED,sw1status


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
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))
int sw2status = 0; //state for switch2
int sw2var = 0; //count of number of times switch 2 is pressed in lab2
int ui8LED = 2; //controls the colour of the led
int sw1status = 0; //state for switch 1

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
* Function Name: timer_config
* Input: void
* Output: void
* Logic: Enables timer 0
*/
void timer_config(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
}
/*
* Function Name: switch_config
* Input: void
* Output: void
* Logic: removes the lock on switch 2;
* sets pin-4 (switch 2) as the input for the port F;
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

/*
* Function Name: int_enable
* Input: void
* Output: void
* Logic: Enables the interrupt and syncs it with timer0
*/
void int_enable(void)
{
	uint32_t ui32Period;
	timer_config();
	ui32Period = (SysCtlClockGet() / 100) ;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE	, TIMER_A);
}

/*
* Function Name: main
* Input: void
* Output: int
* Logic: Calls setup related functions and runs an infinite while loop waiting for interrupts to arrive
*/
int main(void)
{

	setup();
	switch_config();
	led_pin_config();

	int_enable();

	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
	while(1)
	{

	}
}

/*
* Function Name: Timer0IntHandler
* Input: void
* Output: void
* Logic: This is the interrupt handler function. Inside this the state diagram interrupt logic is implemented.
		 Which ensures the bugs in lab1 are handled. That is a press is only counted as one.
*/
void Timer0IntHandler(void)
{
// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);


// Read the current state of the GPIO pin and
// write back the opposite state
// these set of if statements are used to create the state diagram and the transitions as specified in the questions diagram to ensure a press isnt counted multiple times
	if (sw1status == 0)
	{
		if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4))
			sw1status = 1;
	}
	else if(sw1status == 1)
	{
		if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4))
		{
			sw1status = 0;
		}
		else
		{
			sw1status = 2;
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
		}
	}
	else if(sw1status == 2)
	{
		if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4))
		{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
			sw1status = 0;
			if (ui8LED == 8)
			{
				ui8LED = 2;
			}
			else
			{
				ui8LED = ui8LED*2;
			}
		}

	}


	if (sw2status == 0)
	{
		if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0))
			sw2status = 1;
	}
	else if(sw2status == 1)
	{
		if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0))
		{
			sw2status = 0;
		}
		else
		{
			sw2status = 2;
			sw2var = sw2var + 1;
		}
	}
	else if(sw2status == 2)
	{
		if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0))
			sw2status = 0;


	}


}
