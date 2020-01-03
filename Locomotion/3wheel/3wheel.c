#ifndef F_CPU
#define F_CPU 8000000UL // or whatever may be your frequency
#endif
#include<avr/io.h>
#include<util/delay.h>

short int data[2];
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
#define m3c 0 //PD0
#define m3a 1 //PD1


int gameByte(short int command)
{
        short int i ;
        _delay_us(1);
        short int data = 0x00;                             // clear data variable to save setting low bits later.

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
void  init_PS2inanalougemode()
{

	   unsigned char chk_ana = 0, cnt = 0;
//here i have used 50 cnts to alow the controller to set in the analog mode
	   while(cnt<25)
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
		   chk_ana = gameByte(0x42);            // the 2nd byte to be returned from the controller should = 0x73 for "red" analouge controller.
//no need for this any more

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

	  // keep increasing counter to be dispalyed untill PSx controller confirms it's in analouge mode.
		   cnt++;
//increment till 50 counts
 //if (cnt > 254){ cnt=0;}
   }


}
void init_PS2()
{
    DDRC=0XFF;     // PSx controller I/O pin setup:
	   sbi(DDRB, PB7);                 // clock. output. (blue)

	   cbi(DDRB, PB6);                 // data. input. (brown)
	   sbi(PORTB, PB6);               //    enable pullup resistor
        // this was 4 first but as it is required for pwm it is made 5.and its connection is anyways not require at uc end
	   //cbi(DDRB, PB3);                 // acknolage. input. (green)
	   //sbi(PORTB, PB3);               //    enable pullup resistor

	   sbi(DDRB, PB5);                 // command. output. (orange)

	   sbi(DDRB, PB4);                 // attention. output. (yellow)

	   init_PS2inanalougemode();
}
short int PS2_commn()
{
	short int temp, data0, data1, data2, data3, data4, data5;

	sbi(PORTB, PScommand);                          // start communication with PSx controller
	sbi(PORTB, PSclock);
	cbi(PORTB, PSattention);

	gameByte(0x01);                                       // bite 0. header.
	temp = gameByte(0x42);                          // bite 1. header. (should possibly put test on this byte to detect unplugging of controller.)
	gameByte(0x00);                                       // bite 2. header.

	data0 = gameByte(0x00);                         // bite 3. first data bite.
	data1 = gameByte(0x00);                         // bite 4.
	data2 = gameByte(0x00);                         // bite 5.
	data3 = gameByte(0x00);                         // bite 6.
	data4 = gameByte(0x00);                         // bite 7.
	data5 = gameByte(0x00);                         // bite 8.

	_delay_us(1);
	sbi(PORTB, PScommand);                      // close communication with PSx controller
	_delay_us(1);
	sbi(PORTB, PSattention);                        // all done.
	data[0] = data2;
	data[1] = data3;
   _delay_ms(30);
   return data0;
}
int main(void)
{
		init_PS2();
		DDRA = 0xff;
		DDRD = 0xff;
		DDRC = 0xff;
		PORTD = 0xff;
		sbi(PORTD,2);
		sbi(PORTD,3);
		short int PSorder;
		PSorder = 0x00;
		while(1)
		{
			   PSorder = PS2_commn();
			   PORTC = PSorder;
			   if(PSorder == 0b11001100)//motor 1 in clock wise, motor 2 in anticlockwise
			   {	
			   	sbi(PORTA,m1c);
			   	cbi(PORTA,m1a);
			   	cbi(PORTA,m2c);
			   	sbi(PORTA,m2a);
			   	cbi(PORTD,m3c);
			   	cbi(PORTD,m3a);	
			   }
			   else if(PSorder == 0b00111100)//backward
			   {	
			   	cbi(PORTA,m1c);
			   	sbi(PORTA,m1a);
			   	sbi(PORTA,m2c);
			   	cbi(PORTA,m2a);
			   	cbi(PORTD,m3c);
			   	cbi(PORTD,m3a);
			   }
			   else if(PSorder == 0b01111100)  //right dpad botton
			   {	
			   	sbi(PORTA,m1c);
			   	cbi(PORTA,m1a);
			   	sbi(PORTA,m2c);
			   	cbi(PORTA,m2a);
			   	sbi(PORTD,m3c);
			   	cbi(PORTD,m3a);
			   }
			   else if(PSorder == 0b10011100)
			   {	
			   	cbi(PORTA,m1c);
			   	sbi(PORTA,m1a);
			   	cbi(PORTA,m2c);
			   	sbi(PORTA,m2a);
			   	cbi(PORTD,m3c);
			   	sbi(PORTD,m3a);
			   }
			   else
			   {
			   	cbi(PORTA,m1c);
			   	cbi(PORTA,m1a);
			   	cbi(PORTA,m2a);
			   	cbi(PORTA,m2c);
			   	cbi(PORTD,m3c);
			   	cbi(PORTD,m3a);
			   }
		}
		return 0;
}
