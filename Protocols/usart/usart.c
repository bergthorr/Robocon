#include<avr/io.h>
#include<util/delay.h>
#include<stdlib.h>
#include "usart.h"

/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 8000000UL
#endif


void usart0_init(uint32_t baud){
	/* RXD0: PE0, TXD0: PE1 */
	uint16_t ubrr = F_CPU / 16UL / baud - 1;
	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

uint8_t usart0_recv_char(){
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

void usart0_send_char(unsigned char ch){
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = ch;
}

void usart0_send_string(char *str){
	while(*str != 0){
		usart0_send_char(*(str++));
		_delay_ms(1);
	}
}

void usart0_send_int16(int16_t val){
	char buff[7];
	itoa(val, buff, 10);
	usart0_send_string(buff);
}

void usart1_init(uint32_t baud){
	/* RXD1: PD2, TXD1: PD3 */
	uint16_t ubrr = F_CPU / 16 / baud -1;
	/* Set baud rate */
	UBRR1H = (unsigned char)(ubrr>>8);
	UBRR1L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	/* Set frame format: 8data, 2stop bit */
	UCSR1C = (1<<USBS1)|(3<<UCSZ10);
}

void usart1_send_char(unsigned char ch){
	while(!(UCSR1A & (1 << UDRE1)));
	UDR1 = ch;
}

uint8_t usart1_recv_char(){
	while(!(UCSR1A & (1<<RXC1)));
	return UDR1;
}

void usart1_send_string(char *str){
	while(*str != 0){
		usart1_send_char(*(str++));
		_delay_ms(1);
	}
}

void usart1_send_int16(int16_t val){
	char buff[7];
	itoa(val, buff, 10);
	usart1_send_string(buff);
}

void usart2_init(uint32_t baud){
	/* RXD2: PH0, TXD2: PH1 */
	uint16_t ubrr = F_CPU / 16 / baud -1;
	/* Set baud rate */
	UBRR2H = (unsigned char)(ubrr>>8);
	UBRR2L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR2B = (1<<RXEN2)|(1<<TXEN2);
	/* Set frame format: 8data, 2stop bit */
	UCSR2C = (1<<USBS2)|(3<<UCSZ20);
}

void usart2_send_char(unsigned char ch){
	while(!(UCSR2A & (1 << UDRE2)));
	UDR2 = ch;
}

uint8_t usart2_recv_char(){
	while(!(UCSR2A & (1<<RXC2)));
	return UDR2;
}

void usart2_send_string(char *str){
	while(*str != 0){
		usart2_send_char(*(str++));
		_delay_ms(1);
	}
}

void usart2_send_int16(int16_t val){
	char buff[7];
	itoa(val, buff, 10);
	usart2_send_string(buff);
}

void usart3_init(uint32_t baud){
	/* RXD3: PJ0, TXD3: PJ1 */
	uint16_t ubrr = F_CPU / 16 / baud -1;
	/* Set baud rate */
	UBRR3H = (unsigned char)(ubrr>>8);
	UBRR3L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR3B = (1<<RXEN3)|(1<<TXEN3);
	/* Set frame format: 8data, 2stop bit */
	UCSR3C = (1<<USBS3)|(3<<UCSZ30);
}

void usart3_send_char(unsigned char ch){
	while(!(UCSR3A & (1 << UDRE3)));
	UDR3 = ch;
}

uint8_t usart3_recv_char(){
	while(!(UCSR3A & (1<<RXC3)));
	return UDR3;
}

void usart3_send_string(char *str){
	while(*str != 0){
		usart3_send_char(*(str++));
		_delay_ms(1);
	}
}

void usart3_send_int16(int16_t val){
	char buff[7];
	itoa(val, buff, 10);
	usart3_send_string(buff);
}
