/*************************************************************************************************************************
Author : Shubham Bidkar

Date : 7/2/17

This code is for measuring RPM of mechanism motor and display on SPI module.

Microcontroller Used : Atmega 32.

Encoder Pins :

Red 	----> Vcc
Black 	----> Gnd
White 	----> Channel A
Green 	----> Channel B

In this program two timers are used.
Timer 0 ----> For indication of 1 sec elapsed time.
Timer 1 ----> Used as a counter to count external pulses coming on pin PB1 of Channel A of Encoder.


Encoder Connections :

PB1 		        ----> Channel A.
GND(on board)		----> Gnd.
Vcc(on board) 		----> Vcc.

3 Pin relimate required.

*******************************************************************************************************************/
#ifndef F_CPU
#define F_CPU 1000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "max7219_spi.c"
volatile unsigned long int pps;
volatile unsigned int i=0;
unsigned int counter = 0;
int main()
{
	TCCR2 = 0x2f; 							
	OCR2  = 10;		//Value for 0.01 second.
	TCCR1A = 0x00;
	TCCR1B = 0x06; //Timer 1 as a counter.
	TIMSK = (1 << OCIE2);
	init_spi();
	//sei();
	//MAX7219_clearDisplay(); 
	while(1)
	{		
		counter=(long)(240*pps)/500;
		MAX7219_writeData(1,1);
		MAX7219_writeData(2,2);
		MAX7219_writeData(3,3);
		//MAX7219_writeData(4,4);
		//MAX7219_writeData(5,5);
		//MAX7219_writeData(6,6);
		//MAX7219_writeData(7,7);
		//MAX7219_writeData(8,8);
		MAX7219_writeData(9,9);
		//MAX7219_writeData(10,10);
	}
}
ISR(TIMER2_COMP_vect)
{	
	i++;
	if(i>25)
	{
		pps=TCNT1;
		TCNT1=0;
		i=0;
	}	
}
