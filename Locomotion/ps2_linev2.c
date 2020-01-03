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
#define kp 1.2
#define ki 0
#define kd 1

#define fwd 0b00000101
#define rev 0b00001010
#define cw 0b00000110
#define acw 0b00001001
#define stop 0

int j_count=0;

volatile uint8_t data;

#define PSdata             6 //PB6
#define PScommand          5 //PB5
#define PSclock            7 //PB7
#define PSattention        4 //PB4

#define sbi(x,y)  x|=(1<<y)     //setbit
#define cbi(x,y)  x&=~(1<<y)//clear bit

void rotate(uint8_t dir)
{
	PORTA=dir;
//	OCR1AH = 0;
//	OCR1BH = 0;
	OCR1A= 200;
	OCR1B= 200;
}

unsigned char gameByte(unsigned char command)
{
        unsigned char i ;
        _delay_us(1);
        unsigned char data = 0x00;                             // clear data variable to save setting low bits later.

		for(i=0;i<8;i++)
        {
		 if(command & _BV(i))
		   {
			 sbi(PORTB, PScommand);       // bit bang "command" out on PScommand wire.
		   }
		 else
		   {
			 cbi(PORTB, PScommand);
		   }


		 cbi(PORTB, PSclock);                             // CLOCK LOW
		 _delay_us(1);                                    // wait for output to stabilise


		 if((PINB & _BV(PSdata)))
		  {
			 sbi(data, i);                               // read PSdata pin and store
		  }
		 else
		  {
		     cbi(data, i);
		  }

		sbi(PORTB, PSclock);                             // CLOCK HIGH
        }

        sbi(PORTB, PScommand);
        _delay_us(20);                                                   // wait for ACK to pass.
        return(data);
}
void init_PS2inanalougemode()
{
	unsigned char chk_ana = 0, cnt = 0;
	// put controller in config mode
	sbi(PORTB, PScommand);
	sbi(PORTB, PSclock);
	cbi(PORTB, PSattention);

	gameByte(0x01);
	gameByte(0x43);
	gameByte(0x00);
	gameByte(0x01);
	gameByte(0x00);

	sbi(PORTB, PScommand);
	_delay_ms(1);
	sbi(PORTB, PSattention);
	_delay_ms(10);

	// put controller in analoge mode
	sbi(PORTB, PScommand);
	sbi(PORTB, PSclock);
	cbi(PORTB, PSattention);

	gameByte(0x01);
	gameByte(0x44);
	gameByte(0x00);
	gameByte(0x01);
	gameByte(0x03);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x00);

	sbi(PORTB, PScommand);
	_delay_ms(1);
	sbi(PORTB, PSattention);
	_delay_ms(10);

	// exit config mode
	sbi(PORTB, PScommand);
	sbi(PORTB, PSclock);
	cbi(PORTB, PSattention);

	gameByte(0x01);
	gameByte(0x43);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x5A);
	gameByte(0x5A);
	gameByte(0x5A);
	gameByte(0x5A);
	gameByte(0x5A);

	sbi(PORTB, PScommand);
	_delay_ms(1);
	sbi(PORTB, PSattention);
	_delay_ms(10);

	// poll controller and check in analouge mode.
	sbi(PORTB, PScommand);
	sbi(PORTB, PSclock);
	cbi(PORTB, PSattention);

	gameByte(0x01);
	chk_ana = gameByte(0x42);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x00);

	sbi(PORTB, PScommand);
	_delay_ms(1);
	sbi(PORTB, PSattention);
	_delay_ms(10);
}
void init_PS2()
{  
	sbi(DDRB, PB7);
	cbi(DDRB, PB6);
	sbi(PORTB, PB6);
	sbi(DDRB, PB5);
	sbi(DDRB, PB4);
	init_PS2inanalougemode();
}
unsigned char PS2_commn()
{
	unsigned char temp, data0, data1, data2, data3, data4, data5;

	sbi(PORTB, PScommand);		// start communication with PSx controller
	sbi(PORTB, PSclock);
	cbi(PORTB, PSattention);

	gameByte(0x01);			// bite 0. header.
	temp = gameByte(0x42);		// bite 1. header. (should possibly put test on this byte to detect unplugging of controller.)
	gameByte(0x00);			// bite 2. header.

	data0 = gameByte(0x00);	// bite 3. first data bite.
	data1 = gameByte(0x00);	// bite 4.
	data2 = gameByte(0x00);		// bite 5.
	data3 = gameByte(0x00);		// bite 6.
	data4 = gameByte(0x00);		// bite 7.
	data5 = gameByte(0x00);		// bite 8.

	_delay_us(1);
	sbi(PORTB, PScommand);                      // close communication with PSx controller
	_delay_us(1);
	sbi(PORTB, PSattention);                        // all done.
   	_delay_ms(30);
   	return data0;
}
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
	
	right = 200 - err;
	left = 200 + err;
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
	DDRB = 0x0f;
	init_PS2();
	DDRC = 0xff;
	DDRA = 0xFF;
	signed int curr=35, prev;					 
	DDRD |= (1<<PD6);				 //PD6-trigger pin for lsa08
	
	PORTD|=(1<<PD6);	
	UARTinit(BRX);
	init_interrupt();
    
	sei();					 //MOTOR DIRECTION PORT
	INITPWM();
	DDRD |= ((1<<PD4) | (1<<PD5));	 //PD4-OC1B ,PD5-OC1A
	unsigned char PSorder0,PSorder1;
	uint8_t ps2_flag = 0;
	while(1)
	{
		   PSorder0 = PS2_commn();
		   if (PSorder0 == 0b11100100)
		   {
		   	ps2_flag=1;
		   }
		   else if(PSorder0 == 0b11111000)
		   {
		   	ps2_flag=0;
		   }
		   if(ps2_flag)
		   {       PORTC = PSorder0;
	
		   	   if(PSorder0 == 0b11001100)
			   {	rotate(fwd);
			   	PORTC = 0x01;
			   }		
			   else if(PSorder0 == 0b01111100)
			   {
			   	rotate(acw);
			   	PORTC = 0x02;
			   }
			   else if(PSorder0 == 0b00111100)
			   {
			   	rotate(rev);
			   	PORTC = 0x03;
			   }
			   else if(PSorder0 == 0b10011100)
			   {
			   	rotate(cw);
			   	PORTC = 0x04;
			   }
			    else
	  	   	   {
	   	 		rotate(stop);
	   	           }
		   }
		   else if(ps2_flag==0)
	   	   {
	   			prev=curr;
	   			PORTC=0xaa;
				PORTD &= ~(1<<PD6);  //UART TRIGGER
				curr = receive();
				PORTD |= (1<<PD6);
				if(curr==255)
				{
					rotate(stop);
				}
				else
				{
					PWM(curr,prev);
				}
				_delay_ms(50);
	           }
	}
}
