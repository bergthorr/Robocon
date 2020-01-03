#ifndef F_CPU
#define F_CPU 1000000UL // or whatever may be your frequency
#endif
#include<avr/io.h>
#include<util/delay.h>

unsigned char data[6];
#define PSdata             6 //PB6
#define PScommand          5 //PB5
#define PSclock            7 //PB7
#define PSattention        4 //PB4

#define sbi(x,y)  x|=(1<<y)     //setbit
#define cbi(x,y)  x&=~(1<<y)//clear bit

#define m1c 0 //PA0	     //Clockwise when viwed from centre
#define m1a 1 //PA1	     //Anticlockwise when viwed from centre
#define m2c 2 //PA2
#define m2a 3 //PA3
#define m3c 0 //PB0
#define m3a 1 //PB1


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
	//while(chk_ana!=0x73)
	//{
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
		//PORTC = cnt++;
   //}


}
void init_PS2()
{
	DDRC=0XFF;     
	sbi(DDRB, PB7);
	cbi(DDRB, PB6);
	sbi(PORTB, PB6);
	sbi(DDRB, PB5);
	sbi(DDRB, PB4);
	init_PS2inanalougemode();
}
void PS2_commn()
{
	unsigned char temp, data0, data1, data2, data3, data4, data5;

	sbi(PORTB, PScommand);		// start communication with PSx controller
	sbi(PORTB, PSclock);
	cbi(PORTB, PSattention);

	gameByte(0x01);			// bite 0. header.
	temp = gameByte(0x42);		// bite 1. header. (should possibly put test on this byte to detect unplugging of controller.)
	gameByte(0x00);			// bite 2. header.

	data[0] = gameByte(0x00);	// bite 3. first data bite.
	data[1] = gameByte(0x00);	// bite 4.
	data2 = gameByte(0x00);		// bite 5.
	data3 = gameByte(0x00);		// bite 6.
	data4 = gameByte(0x00);		// bite 7.
	data5 = gameByte(0x00);		// bite 8.

	_delay_us(1);
	sbi(PORTB, PScommand);                      // close communication with PSx controller
	_delay_us(1);
	sbi(PORTB, PSattention);                        // all done.
   	_delay_ms(30);
   	//return data[1];
}
int main(void)
{
		DDRB = 0x0f;
		init_PS2();
		DDRA = 0xff;
		DDRD = 0xff;
		DDRC = 0xff;
		sbi(PORTB,2);
		sbi(PORTB,3);
		unsigned char PSorder0,PSorder1;
		while(1)
		{
			   PS2_commn();
			   //PORTC = data[0];
			   //PORTD = data[1]; 
			   if(data[0] == 0b11101110)//motor 1 in clock wise, motor 2 in anticlockwise
			   {
			   	PORTD = 0x01;	
			   	sbi(PORTA,m1c);
			   	cbi(PORTA,m1a);
			   	sbi(PORTA,m2c);
			   	cbi(PORTA,m2a);
			   	cbi(PORTB,m3c);
			   	cbi(PORTB,m3a);	
			   }
			   else if(data[0] == 0b10111110)//backward
			   {	PORTD = 0x02;
			   	cbi(PORTA,m1c);
			   	sbi(PORTA,m1a);
			   	cbi(PORTA,m2c);
			   	sbi(PORTA,m2a);
			   	cbi(PORTB,m3c);
			   	cbi(PORTB,m3a);
			   }
			   else if(data[1] == 0b11111011)//L1 anti
			   {	PORTD = 0x03;
			   	sbi(PORTA,m1c);
			   	cbi(PORTA,m1a);
			   	cbi(PORTA,m2c);
			   	sbi(PORTA,m2a);
			   	sbi(PORTB,m3c);
			   	cbi(PORTB,m3a);
			   }
			   else if(data[1] == 0b11110111)// R1 clock
			   {	PORTD = 0x04;
			   	cbi(PORTA,m1c);
			   	sbi(PORTA,m1a);
			   	sbi(PORTA,m2c);
			   	cbi(PORTA,m2a);
			   	cbi(PORTB,m3c);
			   	sbi(PORTB,m3a);
			   }
			   else
			   {
			   	cbi(PORTA,m1c);
			   	cbi(PORTA,m1a);
			   	cbi(PORTA,m2a);
			   	cbi(PORTA,m2c);
			   	cbi(PORTB,m3c);
			   	cbi(PORTB,m3a);
			   }
		}
		return 0;
}
