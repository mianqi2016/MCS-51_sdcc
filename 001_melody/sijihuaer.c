//Source code under CC0 1.0
//A C code for playing melody "sijihuaer" on STC89C516RD+. 

#include <8052.h>
#include <stdbool.h>
#include "pitches.h"
#include <compiler.h>

//Configure timer for 11.0592 Mhz default SYSCLK

//1=F means:
//Low 6 = NOTE_D4 - 294 hz
//LOw 7 = NOTE_E4 - 340 hz 
//1-do = NOTE_F4 - 349 hz // Doe, a deer, a female deer.
//2-re = NOTE_G4 - 392 hz // Ray, a drop of golden sun.
//3-mi = NOTE_A4 - 440 hz // Me, a name I call myself.
//4-fa = NOTE_AS4 - 466 hz // Far, a long long way to run.
//5-so = NOTE_C5 - 523 hz // Sew, a needle pulling thread.
//6-la = NOTE_D5 - 587 hz // La, a note to follow so.
//7-ti = NOTE_E5 - 659 hz // Tea, a drink with jam and bread, That will bring us back to do, oh, oh, oh.
//High 1 = NOTE_F5 - 698 hz
//High 2 = NOTE_G5 - 784 hz

//Configure timer for 11.0592 Mhz default SYSCLK
//Clock-out Frequency = Oscillator Frequency / (4 × (65536 - RCAP2H,RCAP2L))
//(RCAP2H,RCAP2L) = 65536 - ((Oscillator Frequency / Clock-out Frequency) ÷ 4)
//Pitch(tone)		Frequence 		Register value (RCAP2H,RCAP2L) - Decimal
//Low  6		    220			      52968
//Low  7		    247			      54342
//     1		    262			      54983
//     2		    294			      56132
//     3		    330			      57158
//     4		    349			      57614
//     5		    392			      58483
//     6		    440			      59252
//     7		    494			      59939
//High 1		    523			      60250
//High 2		    587			      60826

#define toneb6  56132
#define toneb7  57404
#define  tone1  57614
#define  tone2  58483
#define  tone3  59252
#define  tone4  59603
#define  tone5  60249
#define  tone6  60826
#define  tone7  61340
#define toneu1  61575
#define toneu2  62009

//for notes durations
#define QUAVER 125 //ms
#define QUARTER 250 //ms
#define SEMI_QUAVER 62 //ms
#define DEMI_SEMI_QUAVER 31 //ms

//Configure timer for 11.0592 Mhz default SYSCLK
//In the Timer function, the TLx registeris incremented every machine cycle. 
//Thus one can think of it as counting machine cycles. Since a machine cycle consists of 12
//oscillator periods, the count rate is 1/12 of the oscillator frequency.
//11059200 ÷ 12 ÷  65536 = 14.0625 //1000 ms
//Note			          Duration		No. of Interrupts
//QUAVER		          125 ms			1
//QUARTER		          250 ms			1
//SEMI_QUAVER	 	      62 ms			  1
//DEMI_SEMI_QUAVER	  31 ms		  	1

//11059200 ÷ 12 = 921600  //1000 ms
//921600 ÷ 8 = 115200  //125 ms
//921600 ÷ 4 = 230400  //250 ms
//921600 ÷ 16 = 57600  //62 ms
//921600 ÷ 32 = 28800  //31 ms

//Mode 2 configures the Timer0 registeras an 8-bit Counter(TLx) with automatic reload, Overlow 
//from TLx not only sets TFx, but also reloads TLx with the ecmtentsof THx. which is oreaetbv software. 
//The reload leaves THx tichanged.

//Note                  Duration                Value for being reloaded into TH0	    No. of interrupts(overflow)
//QUAVER                125 ms                  255-250=5				                      480
//QUARTER               250 ms                  255-250=5				                      961
//SEMI_QUAVER            62 ms                  255-250=5				                      240
//DEMI_SEMI_QUAVER       31 ms                  255-250=5				                      120

#define Rreload 5
#define th8 1920  //480 × 2
#define th4 3840  //
#define th16 960
#define th32 480

const unsigned int melodyFreq[161] = {
         tone6, tone6, toneu1, tone6, tone5, tone3, tone5,     tone2, tone2, tone3, tone5, tone5, tone3,                   tone2, tone3, tone5,  tone3, tone2, tone1, toneb7,   toneb6, toneb6,
         tone3, tone6, tone3,  tone6, tone3, tone2,            tone3, tone2, tone1, toneb7, toneb6,                        tone6, tone6, tone7,  tone6, tone5, tone3, tone5,    tone6,  tone6,
         tone6, tone6, tone7,  tone6, tone5, tone3, tone5,     tone6, tone6,                                               tone6, tone6, toneu2, tone6, tone5,                  tone6,  tone6, tone5, tone3,
         tone6, tone6, toneu1, tone6, tone5, tone3,            tone2, tone2, tone3, tone5, tone5, tone6, tone5, tone3,     tone2, tone3, tone5,  tone3, tone2, tone1, toneb7,   toneb6, toneb6,

         tone6, tone6, toneu1, tone6, tone5, tone3,            tone2, tone2,                                               tone6, tone6, toneu1, tone6, tone5, tone3,           tone2,  tone2,
         tone6, tone6, toneu1, tone6, tone5, tone3,            tone2, tone2, tone3, tone5, tone5, tone6, tone5, tone3,     tone2, tone3, tone5,  tone3, tone2, tone1, toneb7,   toneb6, toneb6,
         tone6, tone6, toneu1, tone6, tone5, tone3,            tone2, tone2,                                               tone6, tone6, toneu1, tone6, tone5, tone3,           tone2,  tone2,
         tone6, tone6, toneu1, tone6, tone5, tone3,            tone2, tone2, tone3, tone5, tone5, tone6, tone5, tone3,     tone2, tone3, tone5,  tone3, tone2, tone1, toneb7,   toneb6, toneb6 };

const unsigned int melodyDura[161] = {
         th8, th16, th16, th16, th16, th16, th16,     th8, th16, th16, th8,  th16, th16,                                     th8, th16, th16,  th16, th16, th16, th16,           th4, th4,
         th8,  th8, th16, th16, th16, th16,          th16, th16, th16, th16, th4,                                            th8, th16, th16,  th16, th16, th16, th16,           th4, th4,
         th8, th16, th16, th16, th16, th16, th16,     th4, th4,                                                              th8,  th8,  th8,  th16, th16,                       th8, th8, th8, th8,
         th8, th16, th16, th16, th16, th8,            th8, th16, th16, th16, th16, th32, th16, th16,                         th8, th16, th16,  th16, th16, th16, th16,           th4, th4,
        
	 th8, th16, th16, th16, th16, th8,            th4, th4,                                                              th8,  th16, th16, th16, th16, th8,              	 th4, th4,
         th8, th16, th16, th16, th16, th8,            th8, th16, th16, th16, th32, th32, th16, th16,                    th8,  th16, th16, th16, th16, th16, th16,      	 th4, th4,
         th8, th16, th16, th16, th16, th8,            th4, th4,                                                              th8,  th16, th16, th16, th16, th8,             	 th4, th4,
         th8, th16, th16, th16, th16, th8,            th8, th16, th16, th16, th32, th32, th16, th16,                    th8,  th16, th16, th16, th16, th16, th16,           th4, th4 };

void playMelody(unsigned char index);
void stop(void);
void setTone(unsigned char index);
void play(unsigned char index);

unsigned int getTicks(void);

//PROGRAMMABLE CLOCK OUT
__sfr __at(0x80) PORT1; //A 50% duty cycle clock can be programmed to come out on P1.O.
		        //This pin, besides being a regular I/O pin, has two alternate functions. 
		        //It can be programmed (1) to input the external clock for Timer/Counter 2 
		        //or (2) to output a 50% duty cycle clock ranging from 61 Hz
		        //to 4 MHz at a 16 MHz operating frequency.
__sbit __at(0x80^0) BEEP; //Beep connected to pin 1.0

//Timer0 used for note qurations:
//Mode 1 is the same as Mode O, except that the Timer register uses all 16 bita.
//In this mode, THx and TLx are cascaded; there is no prescaler.
//Mode 2 configures the Timer registeras an 8-bit Counter(TLx) with automatic reload, Overlow 
//from TLx not only sets TFx, but also reloads TLx with the ecmtentsof THx. which is oreaetbv soft-
//ware. The reload leaves THx tichanged.
__sfr __at(0x88) TCON;  //Timer0/Timer1 Control
__sfr __at(0x89) TMOD;  //Timer0/Timer1 Modes
__sfr __at(0x8A) TL0;   //Timer0 Low Byte
__sfr __at(0x8C) TH0;   //Timer0 High Byte

//Timer2 used for note frequences:
//Timer 2 has three modes of opera-tion: Capture, Auto-Reload, and Baud Rate Generator.
//A 50% duty cycle clock can be programmed to come out on P1.O.This pin, besides being a regular I/O pin,
//has two alternate functions. It can be programmed (1)to input the external clock for Timer/Counter 2 or 
//(2)to output a 50% duty cycle clock ranging from 61 Hz to 4 MHz at a 16 MHz operating frequency. 
//The Clock-out frequency depends on the oscillator frequency and the reload value of Timer 2 capture 
//registers (RCAP2H, RCAP2L) as shown in this equation:
//Clock-out Frequency = Oscillator Frequency / (4 × (65536 - RCAP2H,RCAP2L))
//CP/RL2 T2OE TR2 - 0 1 1 - Clock-Out on P 1.0
//T2CON - TF2 EXF2 RCLK TCLK EXEN2 TR2 C/_T2 CP/_RL2
//TF2 - Timer2 overflow flag set by a Timer2 overflow and must be cleared by software.
__sfr __at(0xC8) T2CON;  //Timer2 Control
//To configure the Timer/Counter 2 as a clock generator, bit C/_T2 (in T2CON) must be cleared end bit T20E in
//T2MOD must be set. Bit TR2 (T2CON.2) also must be set to start the timer.
//T2MOD - xxxx xx T20E DCE
__sfr __at(0xC9) T2MOD;  //Timer2 Modes
__sfr __at(0xCA) RCAP2L;
__sfr __at(0xCB) RCAP2H;
//__sfr __at(0xCC) TL2;  //Timer2 Low Byte
//__sfr __at(0xCD) TH2;  //Timer2 High Byte

__sfr __at(0xa8) IE;  //Interrupt Enable

volatile unsigned int ticks = 0;
volatile _Bool clockUpdate = true;

void ticksUpdate(void) __interrupt(1)
{
        ticks++;
        clockUpdate = true;
}

unsigned int getTicks(void)
{
        unsigned int clockTemp;

        do
        {
                clockUpdate = false;
                clockTemp = ticks;
        } while(clockUpdate);
  
        return(clockTemp);
}


void main(void)
{
  // Config timer0 for 11.0592 Mhz default SYSCLK
  // 11059200/256/256=168.75 ticks per second
  // 1ms = 1000/168.75 = 5.9259... = 6 ticks

	TMOD = 0x02; //Timer0 in Mode 2 - Overtlow from TLx not only sets TFx, but also reloads TLx with the ecmtentsof THx.
	TH0 = 0x05;  //250; 
        IE = 0x82;   //EA IE.7 EA = 1, Enables all interrupts.
                     //ET0 IE.1 Timer 0 interrupt enable bit.

  // Config timer2 for 11.0592 Mhz default SYSCLK
  // A 50% duty cycle clock can be programmed to come out on P1.O.This pin, besides being a regular 1/0 pin,
  // has two alternate functions. It can be programmed (1) to input the external clock for Timer/Counter 2 or (2)
  // to output a so~o duty cycle clock ranging from 61 Hz to 4 MHz at a 16 MHz operating frequency.
  // To configure the Timer/Counter 2 as a clock generator,bit C/T2 (in T2CON) must be cleared end bit T20E in
  // T2MOD must be set. Bit TR2 (T2CON.2) also must be set to start the timer (see Table 6 for operating modes).
  // 11059200/32/256=1350 ticks per second
  // Time2 run in 0 mode
  //T2CON |= 0x04;   //TR2 T2CON.2 must be set to start Timer2
  //C/T2 T2CON.1 must be cleared (OSC/2 in baud rate generator mod)
	T2MOD = 0x02;  //T2OE T2MOD.1 - Timer2 Output Enable
	
	//The Clock-out frequency depends on the oscillator frequency 
	//and the reload value of Timer 2 capture registers (RCAP2H, RCAP2L) as shown in this equation:
	//Clock-out Frequency = Oscillator Frequency / (4 × (65536 - RCAP2H,RCAP2L))

  while(1)
        {
		    for(unsigned char i = 0; i < 161; ++i)
            {
              playMelody(i);
            }
        }   
}

void playMelody(unsigned char index)
{	
	stop();
	setTone(index);
	play(index);
}

void stop(void)
{
	ticks = 0;
  T2CON = 0x00;   //TR2 T2CON.2 must be set to start Timer
	TCON = 0x00; //TimerO Run control bit. Set/cleared by software to turn Timer/ConterO on/off.
}

void setTone(unsigned char index)
{
	unsigned int freq = melodyFreq[index];
	//RCAP2L = freq & 0x00FF;
	RCAP2L = freq % 256;
	RCAP2H = freq / 256;
	//TL2 = freq & 0x00FF;
	//TH2 = freq / 256; 
}

void play(unsigned char index)
{
	unsigned int dura = melodyDura[index];
  T2CON = 0x04;   //TR2 T2CON.2 must be set to start Timer2
	TCON = 0x10; //TimerO Run control bit. Set/cleared by software to turn Timer/ConterO on/off.
	while(getTicks() < dura);
}
