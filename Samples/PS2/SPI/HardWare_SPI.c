#ifndef F_CPU
#define F_CPU 1000000UL
#endif
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#define SS 4					//Attention
#define MOSI 5					//Command
#define MISO 6					//DATA
#define SCK 7
unsigned char data,data1;					//Clock
void initiate_SPI()
{	
	DDRB|=(1<<MOSI)|(1<<SCK);		//Set Mosi clock and attention as output
	//SPCR|=(1<<DORD);			//Byte is transmitted LSB first
	SPCR|=(1<<MSTR);			//Master mode
	//SPCR|=(1<<CPOL);			//Clock is high initially
	//SPCR|=(1<<CPHA);	
	SPCR|=(1<<SPR0);
	SPCR|=(1<<SPE);				//Enable SPI		               			
}
unsigned char transfer(unsigned char cmd){
	SPDR = cmd;
	while(!(SPSR & (1<<SPIF)));
	return (SPDR);	
}
void send(){
	PORTA &= ~(1<<1);
	data = transfer(0xf0);
	//data1 = transfer(0x42);
	PORTA |= (1<<1);
}
void main()
{
	initiate_SPI();
	unsigned char data;
	DDRC = 0xff;
	DDRA = 0xff;
	PORTA |= (1<<1);
	while(1)
	{	send();
		PORTC = data;
	}

}
