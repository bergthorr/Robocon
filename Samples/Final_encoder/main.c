/***********************************************************************************************************

Author : Shubham Bidkar

Date : 20/02/2017

This code is for using encoder for measuring RPM of mechanism motor.

Encoder Connections:

White --->  +Vdc  
Black--->   Comm 
Brown----> Channel A


Timers Used:
Timer 0 ----> Timer 0 is used as for generating elapsed time of 1 sec.
Timer 5 ----> Timer 5 is used as a 16 bit counter.

 

*********************************************************************************************************/
#ifndef F_CPU
#define F_CPU 14745600UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "max7219_spi.c"


volatile unsigned int tim = 0;
volatile unsigned long int pps = 0;
unsigned long rpm;

ISR(TIMER0_COMPA_vect)
{
	tim++;
	if(tim >25)
	{
		pps = TCNT5;
		TCNT5=0;
		tim=0;
	}
	
}

//--------------------------------------------------------------------------------------//

int main()
{
	init_spi();
	
	
	TCCR0A = 0x82;
	TCCR0B = 0x05;
	
	OCR0A = 72;
	
	TCCR5A = 0x00;
	TCCR5B = 0x06; //Timer 5 as a counter.
	
	TIMSK0 = (1 << OCIE0A);

	sei();
	float a = 25;
	while(1)
	{
		rpm=(pps*240)/500;
		MAX7219_displayNumber(a*100000000000000);				
	}

	return 0;
}

