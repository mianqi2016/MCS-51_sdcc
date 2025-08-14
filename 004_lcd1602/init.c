//Source code under CC0 1.0


#define Nop()\
        __asm\
        nop\
        __endasm\

/*
 * LCD1602 pins
 * VSS VDD V0 RS RW E D0 D1 D2 D3 D4 D5 D6 D7 A K
 */


// Pin 0 - 7 on Port 0 connected to D0 - D7
__sfr __at(0x80) P0;
#define LCD1602_DATAPORT P0

// Pin 5-7 on Port 2 connected to LCD1602 RW/RS/EN
__sbit __at(0xA0+5) P2_5;
__sbit __at(0xA0+6) P2_6;
__sbit __at(0xA0+7) P2_7;
__sbit __at(0xA0+0) P2_0;

#define LCD1602_RW P2_5
#define LCD1602_RS P2_6
#define LCD1602_EN P2_7
#define LED P2_0

void lcd1602_write_cmd(unsigned char cmd);
void lcd1602_write_date(unsigned char data);
void lcd1602_init(void);
void lcd1602_clear(void);
void lcd1602_show_string(unsigned char x, unsigned char y, unsigned char *str);
void lcd1602_show_data(unsigned char x, unsigned char y, unsigned char data);

void delay_200ms(void);   // @11.0592 MHz
void delay_ms(unsigned int _ms);        // @11.0592 MHz

void delay(void);

void lcd1602_write_cmd(unsigned char cmd)
{
	LCD1602_RS = 0;
	LCD1602_RW = 0;
	LCD1602_EN = 0;
	LCD1602_DATAPORT = cmd;
	delay_200ms();
	LCD1602_EN = 1;
	delay_200ms();
	LCD1602_EN = 0;
}

void lcd1602_write_data(unsigned char data)
{
	LCD1602_RS = 1;
	LCD1602_RW = 0;
	LCD1602_EN = 0;
	LCD1602_DATAPORT = data;
	delay_200ms();
	LCD1602_EN = 1;
	delay_200ms();
	LCD1602_EN = 0;
}

void lcd1602_init(void)
{
	lcd1602_write_cmd(0x38); // 8-bit data interface, 2-line display, 5×7 dot matrix
	lcd1602_write_cmd(0x0C); // Turn on display, do not show cursor
	lcd1602_write_cmd(0x06); // Move cursor right after writing a character
	lcd1602_write_cmd(0x01); // Clear screen
        lcd1602_write_cmd(0x80); // Move cursor to the start of the first line
}

void lcd1602_clear(void)
{
	lcd1602_write_cmd(0x01);
}


// x: 0-15
// y: 0-1
void lcd1602_show_string(unsigned char x, unsigned char y, unsigned char *str)
{
	if(y>1 || x>15) return;

	unsigned char i=0;

	if(y<1)		//1 line
	{
		while(*str != '\0')
		{
			if(i+x<16)
			{
				lcd1602_write_cmd(0x80+x+i);	// line 1
				lcd1602_write_data(*str);
			}
			else
			{
				lcd1602_write_cmd(0x80+0x40+x+i-16);	// line 2
				lcd1602_write_data(*str);
			}

			str++;
			i++;
		}

	}
	else		//2 lines
	{
		while(*str != '\0')
		{
			if(i+x<16)
			{
				lcd1602_write_cmd(0x80+0x40+x+i);	// line 2
				lcd1602_write_data(*str);
			}
			else
			{
				lcd1602_write_cmd(0x80+x+i-16);		// line 1
				lcd1602_write_data(*str);
			}

			str++;
			i++;
		}

	}
}


void lcd1602_show_data(unsigned char x, unsigned char y, unsigned char data)
{
	if(y>1 || x>15) return;

	if(y<1)
	{
		lcd1602_write_cmd(0x80+x);
	}
	else
	{
		lcd1602_write_cmd(0x80+0x40+x);
	}

	lcd1602_write_data(data);
}



void main(void)
{
// init:
	LCD1602_RS = 0;
	LCD1602_RW = 0;
	LCD1602_EN = 0;


	LCD1602_DATAPORT = 0x38;
	delay_200ms();
	//delay();
	LCD1602_EN = 1;
	delay_200ms();
	//delay();
	LCD1602_EN = 0;


	LCD1602_DATAPORT = 0x0C;
	delay_200ms();
	//delay();
	LCD1602_EN = 1;
	delay_200ms();
	//delay();
	LCD1602_EN = 0;


	LCD1602_DATAPORT = 0x06;
	delay_200ms();
	//delay();
	LCD1602_EN = 1;
	delay_200ms();
	//delay();
	LCD1602_EN = 0;


	LCD1602_DATAPORT = 0x01;
	delay_200ms();
	//delay();
	LCD1602_EN = 1;
	delay_200ms();
	//delay();
	LCD1602_EN = 0;


// start location:(0,0)
	
	LCD1602_RS = 0;
	LCD1602_RW = 0;
	LCD1602_EN = 0;

	LCD1602_DATAPORT = 0x80;
	delay_200ms();
	//delay();
	LCD1602_EN = 1;
	delay_200ms();
	//delay();
	LCD1602_EN = 0;


// print: a char, 'a' or 'b' or ...

	LCD1602_RS = 1;
	LCD1602_RW = 0;
	LCD1602_EN = 0;
	LCD1602_DATAPORT = 0x30 + 0x32;
	delay_200ms();
	//delay();
	LCD1602_EN = 1;
	delay_200ms();
	//delay();
	LCD1602_EN = 0;

	unsigned char str[] = "Hello, World!";

	lcd1602_show_string(0, 1, str);

	while(1)
	{
		//lcd1602_show_data(0, 1, 'c');
		LED = 1;
		delay_ms(5);
		LED = 0;
		delay_ms(5);
	}

}


void delay_200ms(void)
{
	unsigned char i, j;

	i=2;
	j=199;

	do
	{
		do
		{
			Nop();
		} while(--j);

		j=199;

	} while(--i);
}


void delay_ms(unsigned int _ms)
{
	unsigned char i, j;
	i=2;
	j=199;

	while(_ms--)
	{
		do
		{
			do
			{
				Nop();
			} while(--j);

			j=199;
		} while(--i);
	}

}


void delay(void)
{
	for(unsigned int i=10; i>0; i--)
	{
		for(unsigned int j=0; j<200; j++);
	}
}

