//PIN e4 as echo
//PIN e2 as trigger
//#define clock_per_us	F_CPU/1000000UL		//1Mhz => 1us per clock cycle ... 8MHz = 1 us per 8 cycles
#ifndef F_CPU
#define F_CPU 14745600UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "max7219_spi.c"
#define kp 1
#define ki 0
#define kd 0

#define sbi(x,y)  x|=(1<<y)
#define cbi(x,y)  x&=~(1<<y)

volatile unsigned char working;
volatile unsigned char rising_edge;
volatile uint16_t timer_value;
float us;
long distance_cm;
volatile unsigned int tim = 0;
volatile unsigned long int pps = 0;

void Send_signal()
{
	TCCR4B &= ~(1 << CS40);
	_delay_ms(50);		//Restart HC-SR04
	working = 1;	//Be sure that it is ready
	PORTE &=~ (1 << PINE2);
	_delay_us(10);
	PORTE |= (1 << PINE2); //Send 10~12us second pulse
	_delay_us(10);
	PORTE &=~ (1 << PINE2);
}
void Initialize_external_interrupt()
{
	EIMSK &= ~(1 << INT4);
	EICRB |= (1 << ISC40);	//Any logical change on INT1
	EIMSK |= (1 << INT4);	//Enable INT1
}
void Initialize_timer()
{
	TCNT4H = 0x00;
	TCNT4L = 0x00;
	TCCR4B &= ~(1 << CS40);	 //No clock source
//	TIMSK |= (1 << TOIE4); //Timer overflow interrupt enable
}
/*ISR (TIMER1_OVF_vect)
{
		TCNT1H = 0x00;
		TCNT1L = 0x00;
		TCCR1B &= ~(1 << CS10);	 //No clock source
		working=0;
}*/
ISR (INT4_vect)
{
	if(working==1) //Check if echo is high, start timer
	{
		if(rising_edge==0)
		{	
			rising_edge=1;
			TCNT4H = 0x00;
			TCNT4L = 0x00;
			//TCCR4B |= (1 << CS42);	// timer with 1/256 prescaling
			TCCR4B |= (1 << CS40);      // timer with no prescaling
		}
		else //Check if echo turned low, calculate distance
		{				
			rising_edge = 0;
			us = (TCNT4/14.7456);	// number of microseconds for travelling
			distance_cm = (us)/58;		// conversion of microseconds to distance in cm
			TCNT4=0x0000;
			working = 0;
			_delay_ms(150);
		}
	}
}
void pwm(uint8_t a)
{
	if(a>255)
		a=255;
	else if(a<175)
		a=175;	
	OCR0A=a;
	//OC0A PB7uuuu guideway motor pwm
}
ISR(TIMER2_COMPA_vect)
{
	tim++;
	if(tim >= 12){
		pps = TCNT5;
		TCNT5=0;
		//PORTJ = 0xaa;
		//usart0_send_char(0xaa);
		tim=0;
	}
	//PORTB ^= (1 << PORTB0);
}
int main(void)
{
	long a=0;	
	init_spi();
	int setDistance = 27;
	int16_t integral=0, derivative=0, err=0, p=0, i=0, d=0, send_pwm=100, PID, prev_err;

	DDRK=0xFF;		//guideway dir
	DDRE |= (1 << PINE2);	// Trig
	DDRE &= ~(1 << PINE4);  // Echo	

	unsigned char rpm = 0;
	
	DDRB|=0xf0;
	//PORTB|=0x40;
	Initialize_external_interrupt();
	Initialize_timer();
	
	//guideway timers and pwm
	TCCR0A=0x83;
	TCCR0B=0x09;

	//encoder timers (for interrupt and counter)
	TCCR2A = 0x82;
	TCCR2B = 0x07;
	//OCR2A  = 31250;
	OCR2A = 144;
	TCCR5A = 0x00;
	TCCR5B = 0x06; //Timer 1 as a counter.
	TIMSK2 = (1 << OCIE2A);

	//pwm init encoder (noninverting fast pwm)
	TCCR3A=0x2f;
	TCCR3B=0x09;
	TCCR3C=0x00;
	OCR3A = 600;	//OC3A PE3 encoder motor pwm

	working=0;
	sei();

	while(1)
	{
		if(working == 0) //Be sure that conversation is finished
		{
			Send_signal();
		}
		rpm=(480*pps)/500;
		a = (10000*distance_cm) + rpm;	
		MAX7219_displayNumber(a);
		//err = setDistance - distance_cm;
		/*if(err > 0)
		{
			sbi(PORTK,PB7);
			cbi(PORTK,PB6);
		}
		else
		{
			cbi(PORTK,PB7);
			sbi(PORTK,PB6);
		}
		if(err < 2 && err > -2)
		{
			cbi(PORTK,PB7);
			cbi(PORTK,PB6);
		}		
		integral += err;
		derivative = err - prev_err;
		p = kp * err;
		i = ki * integral;
		d = kd * derivative;
		PID = p + i + d;
		send_pwm = send_pwm + PID;
		pwm(send_pwm);
		prev_err = err;*/
	}
	return 0;
}
