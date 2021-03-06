#include<avr/io.h>
#include<util/delay.h>

unsigned char data[6];
#define PSdata             6 //PB6
#define PScommand          5 //PB5
#define PSclock            7 //PB7
#define PSattention        4 //PB4

#define sbi(x,y)  x|=(1<<y)     //setbit
#define cbi(x,y)  x&=~(1<<y)	//clear bit

unsigned char gameByte(unsigned char command)
{
        unsigned char i ;
        _delay_us(1);
        unsigned char data = 0x00;                             // clear data variable to save setting low bits later.
	for(i=0;i<8;i++)
        {
		if(command & _BV(i))
			sbi(PORTB, PScommand);      	 	// bit bang "command" out on PScommand wire.
		else
			cbi(PORTB, PScommand);
		cbi(PORTB, PSclock);                             // CLOCK LOW
		_delay_us(1);                                    // wait for output to stabilise
		if((PINB & _BV(PSdata)))
			sbi(data, i);                               // read PSdata pin and store
		else
			cbi(data, i);
		sbi(PORTB, PSclock);                             // CLOCK HIGH
	}
        sbi(PORTB, PScommand);
        _delay_us(20);                                                   // wait for ACK to pass.
        return(data);
}
void init_PS2inanalougemode()
{
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
	gameByte(0x42);
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

	data0 = gameByte(0x00);		// bite 3. first data bite.
	data1 = gameByte(0x00);		// bite 4.
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
int main(void)
{
		DDRB = 0x0f;
		init_PS2();
		DDRA = 0xff;
		DDRD = 0xff;
		DDRC = 0xff;
		unsigned char PSorder0,PSorder1;
		while(1)
		{
			   PSorder0 = PS2_commn();
			   PORTC = PSorder0;
			
		}
		return 0;
}
