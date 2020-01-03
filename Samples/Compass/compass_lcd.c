#ifndef F_CPU
#define F_CPU 14745600UL
#endif

#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include "i2c_master.c"
//#include "compass.c"
#include "lcd.c"
/*
// #define sbi(reg, bit) reg |= (1 << bit)
// #define cbi(reg, bit) reg &= ~(1 << bit)


int main()
{
	DDRD = 0x00;
	PORTD = 0x03;
	float p;
	float pi = 0;
	lcd_compass(pi);
	_delay_ms(1000);    
	i2c_init();

	init_HMC5883L();
    pi = getHeading(); //Initial compass reading when the code begins
    //usart_send_int16(pi);
    lcd_compass(pi);
    uint16_t offset;	
	// LCD_init();
	DDRG = 0XFF;
	PORTG  = 0XAA;
	_delay_ms(4000);
	PORTG = pi;
	uint16_t val = 0;
	while(1)
	{
		p = getHeading(); //Compass readings being polled
		offset = p - pi; //Calculate deviation from initial angle //variable to be printeds
		if(offset < -180) //To handle the cases for overflow. Angle going beyond 360.
			offset = (offset+360)%360;
			
		else if(offset > 180)
			offset = (offset-360)%360;
		lcd_compass(p);
		// val++;
		// _delay_ms(100);
		PORTG = offset;
	}
}
*/

int main(){
	DDRF = 0xff;
	DDRK = 0xff;
	while(1){
		unsigned int disp = 0;	
		lcd_compass(disp);
		_delay_ms(1000);
	}
	return 0;
}

