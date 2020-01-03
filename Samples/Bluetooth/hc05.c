// PORT D LCD Data
// PH0 - RX2 
// PH1 - TX2

#define F_CPU 8000000
#define BAUD 38400

#define RS PC0
#define RW PC1
#define EN PC2

#include <avr/io.h>
#include <util/delay.h>

void init_uart()
{
	unsigned int UBBR_VAL = (F_CPU/(2*BAUD)-1);

	UCSR2B = ((1<<RXEN2)|(1<<TXEN2));
	UCSR2C = ((3<<UCSZ20));

	UBRR2H = (unsigned char)(UBBR_VAL>>8);
	UBRR2L = (unsigned char)UBBR_VAL;

}

unsigned char uart_recieve()
{
	while(!(UCSR2A & (1<<RXC2)));
	return UDR2;
}

void LCD_cmd(unsigned char cmd)
{
	PORTC=0x00;
	PORTD=cmd;
	PORTC=(1<<EN);
	_delay_ms(50);
	PORTC=0x00;
}

void LCD_data(unsigned char data)
{
	PORTC=0x00;
	PORTD=data;
	PORTC=((1<<RS)|(1<<EN));
	_delay_ms(50);
	PORTC=0x00;
}

void init_LCD()
{
	LCD_cmd(0x38);
	LCD_cmd(0x01);
	LCD_cmd(0x0e);
	LCD_cmd(0x0c);
	LCD_cmd(0x80);
}

int main()
{
	unsigned char recieved_byte;
	int count=1;
	DDRD = 0xFF;
	init_uart();
	init_LCD();
	
	while(1)
	{
		recieved_byte = uart_recieve();
		LCD_data(recieved_byte);
		_delay_ms(50);
	}
	
}
