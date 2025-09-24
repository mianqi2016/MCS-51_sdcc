//A program for testing uart on mcu STC89C516RD+
//return back rx in uart 1 second after the last tx of uart on keyboard.
//11.0592 MHz default SYSCLK

#include "stdbool.h"
#include "8051.h"


unsigned char data_received = 0x00;
bool OK_SEND = true;
unsigned int ticks_timer0 = 0x00;

//Timer0 is used for 1000 ms delay
//Timer1 is used for uart baud rate generation

//Enable Timer0 and UART interrupts
//Enable general interrupt
//The Timer 1 interrupt should be disabled in this application.
//Bit Addressable: EA  EC  ET2  ES  ET1  EX1  ET0  EX0
//	      Bit:  7   6    5   4    3    2    1    0
//IE = 0x92;	//1001 0010B 

void uart_init(unsigned char baud);


void main(void)
{
	IE = 0x92;	//1001 0010B
	
	uart_init(0xFD);
	
	while(1)
	{
		//if(ticks_timer0 >= 1350)		//when presacler=32, 1350 times overflow equals to 1 second.
		//{
		//	SBUF = data_received;
		//	TR0 = 0x00;	//stop Timer0
		//	ticks_timer0 = 0x00;		//clear ticks_timer0
		//	while(TI);
		//} else
		//{
			;
		//}
	}
}

//**********************timer.c**********************
//Timer in mode 0 - Either Timer (Timer0/Timer1) in Mode O is an 8-bit Counter with a divide-by-32 preacaler.
//The 13-Bit register consists of all 8 bits of TH0 and the lower 5 bits of TL0.
//The upper 3 bits of TL0 are indeterminate and should be ignored.
//Settingthe run flag (TR0) does not clear the registers.
//Timer0 run in mode 0 with prescaler=32 - 11059200/256=43200, 43200/32=1350, 
//that is: when presacler=32, 1350 times overflow equals to 1 second.
//Timer0 run in mode 0 with prescaler=32 - TL0 = 0001 1111B 
	
//Timer in mode 1 - 16-bit counter -Mode 1 is the same as Mode 0, except that the Timer registeris being run with all 16 bits.
//Timer0 run in mode 1 - TMOD |= 0000 0001B

//Timer run in mode 2 - Mode 2 configures the Timer register as an 8-bit Counter (TL1) with automatic reload.
//Overfiow from TL1 not only sets TF1, but also reloads TL1 with the contents of TH1, which is preset by aoftware.
//The reload leaves TH1 unchanged.
//Mode 2 operationis the same for Timer/Counter 0/1
//Timer1 run in mode 2 - TMOD |= 0010 0000B

//TMOD: Timer/Counter Mode Control Register - GATE C/!T M1 M0 GATE C/!T M1 M0
//                                     Bits -    7    6  5  4    3    2  1  0
//                                   Timers -     Timer 1 <--    --> Timer 0

//Timer0 run in mode 0 with presacler=32
//TL0 = 0x1F;
//TH1 = 0xFD;	//Timer1 run in mode 2 with auto-reload value = 0xFD. UART run in 8N1, with baudrate=9600.
//The Timer 1 interrupt should be disabled in this application.
//TMOD = 0010 0000B = 0x20;

//**********************uart.c**********************
//SCON - The serial port control and status register Special Function Register.
//This register mntains not only the mode selection bits, but also the 9th data bit for transmit and receive (TB8 and RB8),
//and the serial port interrupt bits (TI and RI).
//SCON - SM0 SM1 SM2 REN TB8 RB8 TI RI
//Bits -   7   6   5   4   3   2  1  0
//SCON = 0101 0000B = 0x50.

void uart_init(unsigned char baud)
{
	TMOD = 0x20;
	SCON = 0x50;
        PCON = 0x00;	//bit SMOD=0
	TH1 = baud;
	TR1 = 0x01;	//start Timer1
	TL0 = 0x1F;
	TR0 = 0x01;	//start Timer0
}

//************************interrupts.c*****************************

// In order to use any of the interrupts in the MCS-51, the following 3 steps must be taken:
// 1. Set the EA (enable all) bit in the IE register to 1.
// 2. Set the corresponding individual interrupt enable bit in the IE register to 1.
// Begin the interrupt service routine at the corresponding Vector Address of that interrupt. 
// See Table below:
// Interrupt Source	Vector Address
// 		IE0		0003H
// 		TF0		000BH
// 		IE1		0013H
// 		TF1		001BH
// 		RI & TI		0023H
// 		TF2 & EXF2	002BH
// In addition, for external interrupts, pins !INT0 and !INT1 (P3.2 and P3.3) must be set to 1, 
// and depending on whether the interrupt is to be level or transition activated, 
// bits IT0 or IT1 in the TCON register may need to be set to 1.

//Set EXTI0 low level triggered
//Set EXTI1 falling edge triggered
//TCON_IT0 = 0;
//TCON_IT1 = 1;

//Timer0 OVERFLOW ISR:
void timer0_overflowed(void)  __interrupt(1)
{
	ticks_timer0++;
	//if(ticks_timer0 >= 995)		//1.005381*995=1000.354095 micro-seconds
	if(ticks_timer0 >= 1350)		//when presacler=32, 1350 times overflow equals to 1 second.
	{
		//OK_SEND = true;
		ticks_timer0 = 0x00;
		//TR0 = 0x00;	//stop Timer0
		SBUF = 'A';
		while(!TI);
		TI = 0x00;
	} else {
		;
	}
}

//UART RECEIVE ISR:
void uart_received(void)  __interrupt(4)
{
	RI = 0x00;
	data_received = SBUF;

	//ticks_timer0 = 0x00;	//clear ticks_timer0
	//TR0 = 0x01;	//start Timer0
		
	SBUF = data_received+1;
	while(!TI);
	TI = 0x00;

        /*if(OK_SEND == true)
	{
		SBUF = data_received;
		OK_SEND = false;
		while(TI);
	} else {
		;
	}*/
}


