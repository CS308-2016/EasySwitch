/*
* Team Leader's Name : Nitish Chandra
* Author List: Saicharan, Siddhant
* Filename: lab-4.c
* Functions: UARTIntHandler(void), main(void)
* Global Variables:  ui32ADC0Value[4], ui32TempAvg, setTemp, ui32TempValueC, ui32TempValueF, mode
*/

#include<stdint.h>
#include<stdbool.h>
#include"inc/hw_ints.h"
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/gpio.h"
#include"driverlib/interrupt.h"
#include"driverlib/pin_map.h"
#include"driverlib/sysctl.h"
#include"driverlib/uart.h"
#include"driverlib/adc.h"
#include"driverlib/uart.h"
#include"utils/uartstdio.h"
#include"utils/uartstdio.c"


uint32_t ui32ADC0Value[4];
volatile uint32_t ui32TempAvg;
int32_t setTemp = 0;
volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF;
int32_t mode = 2;


/*
* Function Name: UARTIntHandler
* Input: void
* Output: void
* Logic: The handler function for UART on RX and TX interrupts;
* Handles transitions various states, between pressing of 's' key for setting temp and taking the input;
*/


void UARTIntHandler(void)
{
	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status
	UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts
	while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
	{
		char c = UARTCharGetNonBlocking(UART0_BASE);
		if(c == 's' && mode == 1){
			mode = 2; // set temp command is issued
		}
		else if(mode == 3){ // reading and printing the temperature
			setTemp = (c-'0')*10;
			UARTCharPutNonBlocking(UART0_BASE, c);
			mode = 4;
		}
		else if(mode == 4){ //updating the set temperature
			UARTCharPutNonBlocking(UART0_BASE, c);
			setTemp = setTemp + (c-'0');
			UARTCharPutNonBlocking(UART0_BASE, '\n');
			UARTCharPut(UART0_BASE, 'S');
			UARTCharPut(UART0_BASE, 'e');
			UARTCharPut(UART0_BASE, 't');
			UARTCharPut(UART0_BASE, 'T');
			UARTCharPut(UART0_BASE, 'e');
			UARTCharPut(UART0_BASE, 'm');
			UARTCharPut(UART0_BASE, 'p');
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, 'u');
			UARTCharPut(UART0_BASE, 'p');
			UARTCharPut(UART0_BASE, 'd');
			UARTCharPut(UART0_BASE, 'a');
			UARTCharPut(UART0_BASE, 't');
			UARTCharPut(UART0_BASE, 'e');
			UARTCharPut(UART0_BASE, 'd');
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, 't');
			UARTCharPut(UART0_BASE, 'o');
			UARTCharPut(UART0_BASE, ' ');
			UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
				(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
			UARTCharPut(UART0_BASE, setTemp/10 + '0');
			UARTCharPut(UART0_BASE, setTemp%10 + '0');
			UARTCharPut(UART0_BASE, 'C');
			UARTCharPut(UART0_BASE, '\n');
			mode = 1;
		}
	}
}
int main(void) {
	//clock setup and other peripheral enable functions
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO port for LED
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); //enable pin for LED PF2
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
	(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	IntMasterEnable(); //enable processor interrupts
	IntEnable(INT_UART0); //enable the UART interrupt
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 , 0);
	while (1) //let interrupt handler do the UART echo function
	{

		// if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
		if(mode == 2){
			UARTCharPut(UART0_BASE, 'E');
			UARTCharPut(UART0_BASE, 'n');
			UARTCharPut(UART0_BASE, 't');
			UARTCharPut(UART0_BASE, 'e');
			UARTCharPut(UART0_BASE, 'r');
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, 'S');
			UARTCharPut(UART0_BASE, 'e');
			UARTCharPut(UART0_BASE, 't');
			UARTCharPut(UART0_BASE, 'T');
			UARTCharPut(UART0_BASE, 'e');
			UARTCharPut(UART0_BASE, 'm');
			UARTCharPut(UART0_BASE, 'p');
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, ':');
			UARTCharPut(UART0_BASE, ' ');
			mode = 3;
		}
		else if(mode == 1){
			ADCIntClear(ADC0_BASE, 1);
			ADCProcessorTrigger(ADC0_BASE, 1);
			while(!ADCIntStatus(ADC0_BASE, 1, false)) // computing the average temperature
			{
				ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
				ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
				ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
				ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
			}
			UARTCharPut(UART0_BASE, 'C');			// displaying the current temperature
			UARTCharPut(UART0_BASE, 'u');
			UARTCharPut(UART0_BASE, 'r');
			UARTCharPut(UART0_BASE, 'r');
			UARTCharPut(UART0_BASE, 'e');
			UARTCharPut(UART0_BASE, 'n');
			UARTCharPut(UART0_BASE, 't');
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, 'T');
			UARTCharPut(UART0_BASE, 'e');
			UARTCharPut(UART0_BASE, 'm');
			UARTCharPut(UART0_BASE, 'p');
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, '=');
			UARTCharPut(UART0_BASE, ' ');
			UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
				(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
			UARTCharPut(UART0_BASE, ui32TempValueC/10 + '0');
			UARTCharPut(UART0_BASE, ui32TempValueC%10 + '0');
			UARTCharPut(UART0_BASE, 'C');
			UARTCharPut(UART0_BASE, ',');
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, 'S');
			UARTCharPut(UART0_BASE, 'e');
			UARTCharPut(UART0_BASE, 't');
			UARTCharPut(UART0_BASE, 'T');
			UARTCharPut(UART0_BASE, 'e');
			UARTCharPut(UART0_BASE, 'm');
			UARTCharPut(UART0_BASE, 'p');
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, '=');
			UARTCharPut(UART0_BASE, ' ');
			UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
				(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
			UARTCharPut(UART0_BASE, setTemp/10 + '0');
			UARTCharPut(UART0_BASE, setTemp%10 + '0');
			UARTCharPut(UART0_BASE, 'C');
			UARTCharPut(UART0_BASE, '\n');
			if(setTemp > ui32TempValueC){
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 , 8); //blink green LED accordingly
			}
			else{
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 , 2); //blink red LED accordingly
			}
		}
		SysCtlDelay(SysCtlClockGet()/3);
	}
}
