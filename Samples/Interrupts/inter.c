#ifndef F_CPU
#define F_CPU 1000000L
#endif
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
ISR(TIMER0_OVF_vect)
{
	TCNT0 = -32;
	PORTB ^= 0x20;
}
int main()
{
	DDRB |= 0x20;
	TCNT0 = -32;
	TCCR0 = 0x01;
	TIMSK = (1<<TOIE0);
	sei();
	DDRC = 0x00;
	DDRD = 0xff;
	while(1)
		PORTD = PINC;
} 
