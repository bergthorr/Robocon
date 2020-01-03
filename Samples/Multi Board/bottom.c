/*
	This is a sample ps2 code for testing
	
	PS2 pins are :- 
	i.   PC1 - data (pin1)
	ii.  PC2 - command (pin2)
	iii. PC6 - attention (pin6)
	iv.  PC7 - clock (pin7)
	v.   GND - ground (pin4)
	vi.  VCC - Vcc (pin5)

*/
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include<avr/io.h>
#include<util/delay.h>
#include"ps2.c"
#include"UART.c"
int main(void)
{
	DDRJ = 0xff;
	usart0_init(9600);
	init_PS2();
	while(1)
	{	usart0_send_char(0xAA);
		_delay_ms(500);
	}
	while(1)
	{
		PS2_commn();
	if (data[0]==UP)		
	{
		PORTJ = 0x01;
		usart0_send_char(0x01);
	}
	else if (data[0]==DOWN)		
	{
		PORTJ = 0x02;
		usart0_send_char(0x02);
	}
	else if(data[0]==LEFT)		
	{
		PORTJ = 0x03;
		usart0_send_char(0x03);
	}
	else if (data[0]==RIGHT)		
	{
		PORTJ = 0x04;
		usart0_send_char(0x04);
	}
	else if(data[1]==CIRCLE)	
	{
		PORTJ = 0x05;
	}
	else if(data[1]==SQUARE)	
	{	
		PORTJ = 0x06;
	}
	else if(data[3]==RJ_UP)	
	{
		PORTJ = 0x07;
	}
	else if(data[3]==RJ_DOWN)		
	{
		PORTJ = 0x08;
	}
	else if (data[1]==R_ONE)		
	{
		PORTJ = 0x09;
	}
	else if(data[1]==L_ONE)			
	{
		PORTJ = 0x0A;
	}
	else if(data[1]==TRIANGLE)				
	{
		PORTJ = 0x0B;
	}
	else if(data[5]==LJ_UP)
	{
		PORTJ = 0x0C;
	}
	else if(data[5]==LJ_DOWN)
	{
		PORTJ = 0x0D;
	}
	else if (data[0]==SELECT)		
	{
		PORTJ = 0x0E;
	}
	else if(data[0]==START)
	{
		PORTJ = 0x0F;
	}
	else if(data[2]==RJ_LEFT)		
	{
		PORTJ = 0x10;
	}
	else if (data[2]==RJ_RIGHT)		
	{
		PORTJ = 0x20;
	}
	else if (data[1]==L_TWO)		
	{
		PORTJ = 0x30;
	}
	else if (data[1]==R_TWO)		
	{	
		PORTJ = 0x40;
	}
	else
	{
		PORTJ = 0x00;
	}
	}
}
