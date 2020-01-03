#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
void spi_init_master (void)
{
	DDRB |= (1<<5)|(1<<7);				//connect the ss(PB4) to vcc, MOSI and SCK are output
	//SPCR |= (1<<DORD)|(1<<CPOL)|(1<<CPHA);
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0); 
}
unsigned char spi_tranceiver (unsigned char data)
{	
	SPDR = data;                                  //Load data into buffer
	while(!(SPSR & (1<<SPIF)));                   //Wait until transmission complete
	return(SPDR);                                 //Return received data
}

int main(void)
{
	
	spi_init_master();
	DDRC = 0xff;
	int x = 0x00;
	unsigned char data;
	SPDR = 0x00;
	int tmp=SPDR; 
	while(1)
	{
		data = spi_tranceiver(x);
		PORTC = data;
		x++;
		_delay_ms(50);
	}
}
