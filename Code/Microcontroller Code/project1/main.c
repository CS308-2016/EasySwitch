/*

* Author List : Ch Nitish Chandra, D Sai Charan, Siddhanth R , T P Varun.

* Filename: main.c 

* Functions: ledPinConfig(), UARTIntHandler(), main()  

* Global Variables: ui32Load, ui32PWMClock, ui8Adjust, ui8Adjust2, color, state, i1, i2, inp, ui32Status

*/



#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"
#include "inc/hw_ints.h"
#include"driverlib/uart.h"
#include "driverlib/interrupt.h"

#define PWM_FREQUENCY 55

	volatile uint32_t ui32Load;
	volatile uint32_t ui32PWMClock;
	volatile int ui8Adjust = 83;	//Base value of motor for switch 1
	volatile int ui8Adjust2 = 83;  //Base value of motor for switch 2

/*

* Function Name : ledPinConfig()

* Input: void

* Output: void 

* Logic: Setting pins 1,2,3 of PORT F as ouputs.

* Example Call: Called in main function to set the outputs.

*/




void ledPinConfig(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); 
	 // Pin-1 of PORT F set as output. 
}


uint32_t color=2; 	//Each bit of this variable(in binary form) represents the on/off status of that color
int state = 1;		//State is the input recieved from the app
int i1=0;			//Used to adjust the rotation of motor 1
int i2 = 0;			//Used to adjust the rotation of motor 2
int count =0 ;
unsigned char inp;	//Input character recieved from app
uint32_t ui32Status;

/*

* Function Name : UARTIntHandler()

* Input: void

* Output: void 

* Logic: Whenever a UART0 interrupt comes, this handler is called. This function takes input(a character) from the bluetooth app through UART. Depending on the input, the handler goes into a certain state which controls the rotation of the motor of switch. Each state rotates the required motor through a certain angle.

* Example Call: UARTIntHandler()

*/


void UARTIntHandler(void){

	ui32Status = UARTIntStatus(UART1_BASE, true);
	UARTIntClear(UART1_BASE, ui32Status);



	if(UARTCharsAvail(UART1_BASE)){
		inp = UARTCharGet(UART1_BASE);
	}

	if(inp == '\0'){
		return;
	}
	count++;
	if (count%2 == 1){

		if (inp == 'a') state = 1;			// 1 - on - pd0
		else if (inp == 'b') state = 2;		// 1 - off - pd0
		else if (inp == 'c') state = 3;		// 2 - 0n - pd1
		else if (inp == 'd') state = 4;		// 2 - off - pd1



		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, color);


		if (color !=8){
			color = color*2;
		}
		else{
			color = 2;
		}
		i1 =0;
		i2 =0;
		
		if(state == 1)
		{
			while(i1<50){
				i1++;
				ui8Adjust--;
				PWMPulseWidthSet(PWM1_BASE,PWM_OUT_0 , ui8Adjust * ui32Load / 1000);	//rotating the motor
				SysCtlDelay(5000);
			}
			ui8Adjust = 83;

		}
		else if (state ==2) {
			while (i1<50){
				i1++;
				ui8Adjust++;
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
				SysCtlDelay(5000);
			}
			ui8Adjust = 83;
		}

		else if(state == 3)
		{
			while(i2<50){
				i2++;

				ui8Adjust2--;
				PWMPulseWidthSet(PWM1_BASE,PWM_OUT_1 , ui8Adjust2 * ui32Load / 1000);
				SysCtlDelay(100);
			}
			ui8Adjust2 = 83;

		}
		else if (state ==4) {
			while (i2<50){
				i2++;
				ui8Adjust2++;
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_1, ui8Adjust * ui32Load / 1000);
				SysCtlDelay(100);
			}
			ui8Adjust2 = 83;

		}

	}

}

/*

* Function Name : main()

* Input: void

* Output: 0 

* Logic: Setting up all the pheripherals, ports, inputs, outputs, interrupts and required configurations.

*/

int main(void)
{


	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);




	GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
	GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_1);
	GPIOPinConfigure(GPIO_PD0_M1PWM0);		// pd0 - module 1, gen 0
	GPIOPinConfigure(GPIO_PD1_M1PWM1);		// pd1 - module 1, gen 1

	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
	PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Load);

	PWMGenConfigure(PWM1_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_1, ui32Load);

	PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_0);

	PWMOutputState(PWM1_BASE, PWM_OUT_1_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_1);
	ledPinConfig();
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600,UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |UART_CONFIG_PAR_NONE);
	IntMasterEnable();
	IntEnable(INT_UART1);
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
	while(1)
	{
		SysCtlDelay(100000);
	}
	return 0;

}



