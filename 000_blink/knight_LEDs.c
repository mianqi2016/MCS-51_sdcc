// Source code under CC0 1.0
#include <stdbool.h>

__sfr __at(0xa0) P2;

__sfr __at(0x88) TCON;  //Timer0/Timer1 Control
__sfr __at(0x89) TMOD;  //Timer0/Timer1 Modes
__sfr __at(0x8a) TL0;   //Timer0 Low Byte
__sfr __at(0x8c) TH0;   //Timer0 High Byte

__sfr __at(0xa8) IE;  //Interrupt Enable


volatile unsigned long int ticks;
volatile _Bool clockUpdate;

void ticksUpdate(void) __interrupt(1)
{

	ticks++;
	clockUpdate = true;
}

unsigned long int clock(void)
{
	unsigned long int clockTemp;

	do
	{
		clockUpdate = false;
		clockTemp = ticks;
	} while(clockUpdate);

	return(clockTemp);
}

unsigned char cror (unsigned char num)
{
	unsigned char num1 = 0x01 & num;
	num = num >> 1;
	num = num | ( num1 << 7);

	return(num);

}

unsigned char crol (unsigned char num)
{
        unsigned char num1 = 0x80 & num;
        num = num << 1;
        num = num | ( num1 >> 7);

        return(num);

}

void main(void)
{
	// Config timer0 for 11.0592 Mhz default SYSCLK
	// 11059200/32/256=1350 ticks per second
	// Timer0 run in 0 mode
	
	TMOD = 0x00;  //Timer0 in Mode 0 - 8-bit Timer/Counter.THx with TLx as 5-bit prescele 
	IE |= 0x82;   //EA IE.7 EA = 1, Enables all interrupts.
		      //ET0 IE.1 Timer 0 interrupt enable bit.
	TCON |= 0x10; //Timer O Run control bit. Set/cleared by software to turn Timer/Conter O on/off. 
		      
	P2 = (unsigned char)~0x01;

	unsigned char position = 0;

        _Bool direction = true;// rotate rightward, else leftward.	

	while(1)
	{
		if(clock() >= 112) //11059200/256/32/12=1350/12 = 112.5
		{
			//P2 = (unsigned char)~(0x01 << 1);
		
			if(direction == true)
			{
				P2 = crol(P2);
				position++;
				if(position == 7)
				    direction = false; // change rotate direction.
			}
			else
			{
				P2 = cror(P2);
				position--;
				if(position == 0)
				    direction = true;
	                }	
			ticks = 0;
		}
	}
}
