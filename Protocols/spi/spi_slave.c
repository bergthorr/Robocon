#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
void spi_init_slave (void)
{
	DDRB=(1<<6);                                  		 //MISO as OUTPUT	//connect the ss(PB4) to grd
	SPCR|=(1<<SPE)|(1<<SPR0);                                //Enable SPI
}
unsigned char spi_tranceiver (unsigned char data)
{
	SPDR = data;                                  //Load data into buffer
	while(!(SPSR & (1<<SPIF) ));                  //Wait until transmission complete
	return(SPDR);                                 //Return received data
}

int main(void)
{
	spi_init_slave();
	DDRC = 0xff; 
	x = 0x00; 
	unsigned char data;
	SPDR = 0x00;
	int tmp=SPDR;                        
	while(1)
	{
		data = spi_tranceiver(x);               //Receive data, send ACK
		PORTC = data;
		x++;
		_delay_ms(50);
	}
}
