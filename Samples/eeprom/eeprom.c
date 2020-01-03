#ifndef F_CPU
#define F_CPU 14745600UL 
#endif

#include<avr/io.h>
#include<util/delay.h>
#include<avr/eeprom.h>
#include"max7219_spi.c"
struct ideal{
	uint16_t pwm;
	float angle;
};
struct runtime{
	uint16_t pwm;
	float angle;	
};
void main()
{
	uint16_t pwm = 500,readpwm;
	float angle,readangle;
	struct ideal I[7];
	//eeprom_write_word((uint16_t *)0,500);
	//eeprom_write_word((uint16_t *)2,750);
	//init_spi();
	//readpwm = eeprom_read_word((uint16_t*)0);
	//MAX7219_displayNumber(readpwm);
	while(1);
}
