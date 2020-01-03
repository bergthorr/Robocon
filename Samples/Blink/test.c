#include <avr/io.h>
int main()
{
	DDRA = 0xff;
	PORTA = 0xff;
	DDRB = 0xff;
	PORTB = 0xff;
	DDRC = 0xff;
	DDRD = 0xff;
	PORTC = 0xff;
	PORTD = 0xff;
	while(1);
	return 0;
}
