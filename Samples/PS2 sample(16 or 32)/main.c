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
#define F_CPU 1000000UL		//set this freq according to the freq of microcontroller
#endif

#include<avr/io.h>
#include<util/delay.h>
#include"ps2.c"
int main(void)
{
	DDRA = 0xff;
	init_PS2();
	while(1)
	{
		PS2_commn();
		PORTA = data[0];
	}
}
