/*
PWM LEFT 	OC1B	PD4
PWM RIGHT 	OC1A	PD5
lSA08 Tx	Rx		PD0
J_PULSE		INT0	PD2			
UART En				PD6
MOTOR Dir pins		PORTA - 0,1,2,3

*/

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define baudrate 9600
#define BRX ((F_CPU/(16UL*baudrate))-1)	//baudrate prescaler

#define setvalue 35
#define kp 2
#define ki 0
#define kd 2

#define fwd 0b00000101
#define rev 0b00001010
#define rotate_cw 0b00000110
#define rotate_acw 0b00001001
#define stop 0

int j_count=0;

volatile uint8_t data;

void set_direction(uint8_t dir)
{
	PORTA = dir;
}

void init_interrupt()
{
	MCUCR |= (1<<ISC00) | (1<<ISC01);	//Rising Edge triggered
	GICR |= (1<<INT0);
}

void UARTinit(unsigned int ubrr)		//UART INIT
{
	UBRRH = (unsigned char)(ubrr >> 8);   //baud rate 
	UBRRL = (unsigned char)ubrr;
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0) ; //asyn mode, 8 bit data size
	UCSRB = (1 << TXEN)  | (1 << RXEN)  ;
}

uint8_t receive()
{
	while((UCSRA & (1<<RXC) )== 0);
	return UDR;
}

void transmit(uint8_t datasend)
{
	while((UCSRA & (1<<UDRE)) == 0);
	UDR = datasend;
}

void INITPWM()// 1-0 ANTICLOCKWISE 
{	
	//16 bit timer1
	TCCR1A |= (1<<COM1A1) | (1<<COM1B1)| (1 << WGM10) |(1<<COM1A0) | (1<<COM1B0);
	TCCR1B |= (1<<CS10) | (1<<WGM12); //no prescaling
}

void PWM(signed int curr, signed int prev)
{	
	signed int err,deviation;
	unsigned int left,right;
	int P=0,I=0,D=0;
	
	deviation = setvalue - curr;
	P = deviation * kp;
	I = (I + deviation) * ki;
	D = (deviation - (setvalue - prev)) * kd;
	
	err = P + I + D;
	
	right = 225 - err;
	left = 225 + err;
	set_direction(fwd);
	OCR1AH = 0;
	OCR1BH = 0;
	
	if(right > 255)
	right = 255;
	if(left > 255)
	left=255;

	if(right<80)
	right=80;
	if(left<80)
	left=80;

	OCR1AL = left;			// LEFT MOTOR		PD5
	OCR1BL = right;			// RIGHT MOTOR		PD4
	
}	

ISR (INT0_vect)
{
	set_direction(stop);
	j_count*= 2;
	_delay_ms(2000);
}

int main(void)
{ 
	signed int curr=35, prev;

	DDRB = 0XFF;					 //led
	DDRD |= (1<<PD6);				 //PD6-trigger pin for lsa08
	DDRA = 0xFF;					 //MOTOR DIRECTION PORT
	DDRD |= ((1<<PD4) | (1<<PD5));	 //PD4-OC1B ,PD5-OC1A
	
	PORTD|=(1<<PD6);
		
	UARTinit(BRX);
	INITPWM();
	init_interrupt();
    
	sei();
	while(1)
	{ 

		prev=curr;
		PORTD &= ~(1<<PD6);  //UART TRIGGER
		curr = receive();
		PORTC |= (1<<PD6);
		PORTB = j_count;
		if(curr==255)
		{
			if(j_count>1)
			{
			set_direction(rev);
			_delay_ms(500);
			}
//			PWM(prev,prev);	
		}
		else
		{
			PWM(curr,prev);
		}
		_delay_ms(50);
		
    }
}