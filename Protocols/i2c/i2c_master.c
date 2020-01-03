#include<avr/io.h>
#include<util/delay.h>
#include<stdlib.h>
#include <util/twi.h>
#include "i2c_master.h"	

/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

/* I2C clock in Hz */
#define SCL_CLOCK  100000L
#define Prescaler 1

/* I2C timer max delay */
#define I2C_TIMER_DELAY 0xFF


void i2c_init(void)
{
	TWBR = (uint8_t)(((F_CPU / SCL_CLOCK) / Prescaler) - 16 ) / 2;
}

uint8_t i2c_start(uint8_t address)
{
	uint8_t  i2c_timer = 0;
	// reset TWI control register
	TWCR = 0;
	// transmit START condition 
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	// wait for end of transmission
	i2c_timer = I2C_TIMER_DELAY;
	while( !(TWCR & (1<<TWINT)) && i2c_timer--);
	if(i2c_timer == 0)
		return 1;
		
	// check if the start condition was successfully transmitted
	if((TWSR & 0xF8) != TW_START)
		return 1;
		
	// load slave address into data register
	TWDR = address;
	// start transmission of address
	TWCR = (1<<TWINT) | (1<<TWEN);
	
	// wait for end of transmission
	i2c_timer = I2C_TIMER_DELAY;
	while( !(TWCR & (1<<TWINT)) && i2c_timer-- );
	if(i2c_timer == 0)
		return 1;
		
	// check if the device has acknowledged the READ / WRITE mode
	uint8_t twst = TW_STATUS & 0xF8;
	if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;
	
	return 0;
}

uint8_t i2c_write(uint8_t data)
{
	uint8_t  i2c_timer = 0;
	
	// load data into data register
	TWDR = data;
	// start transmission of data
	TWCR = (1<<TWINT) | (1<<TWEN);
	// wait for end of transmission
	i2c_timer = I2C_TIMER_DELAY;
	while( !(TWCR & (1<<TWINT)) && i2c_timer-- );
	if(i2c_timer == 0)
		return 1;
	
	if( (TWSR & 0xF8) != TW_MT_DATA_ACK ){ return 1; }
	
	return 0;
}

uint8_t i2c_read_ack(void)
{
	uint8_t  i2c_timer = 0;
	
	// start TWI module and acknowledge data after reception
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA); 
	// wait for end of transmission
	i2c_timer = I2C_TIMER_DELAY;
	while( !(TWCR & (1<<TWINT)) && i2c_timer--);
	if(i2c_timer == 0)
		return 0;
	// return received data from TWDR
	return TWDR;
}

uint8_t i2c_read_nack(void)
{
	uint8_t  i2c_timer = 0;
	
	// start receiving without acknowledging reception
	TWCR = (1<<TWINT) | (1<<TWEN);
	// wait for end of transmission
	i2c_timer = I2C_TIMER_DELAY;
	while( !(TWCR & (1<<TWINT)) && i2c_timer--);
	if(i2c_timer == 0)
		return 0;
	// return received data from TWDR
	return TWDR;
}

uint8_t i2c_transmit(uint8_t address, uint8_t* data, uint16_t length)
{
	uint16_t i;
	
	if (i2c_start(address | I2C_WRITE)) return 1;
	for (i = 0; i < length; i++)
	{
		if (i2c_write(data[i])) return 1;
	}
	
	i2c_stop();
	
	return 0;
}

uint8_t i2c_receive(uint8_t address, uint8_t* data, uint16_t length)
{
	uint16_t i;
	
	if (i2c_start(address | I2C_READ)) return 1;
	for ( i = 0; i < (length-1); i++)
	{
		data[i] = i2c_read_ack();
	}
	data[(length-1)] = i2c_read_nack();
	
	i2c_stop();
	
	return 0;
}

uint16_t i2c_writeReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length)
{
	uint16_t i, count=0;
	if(length > 0) {
		//request register
		if (i2c_start(devaddr | I2C_WRITE)) return count;
		i2c_write(regaddr);
		
		//write data
		for ( i = 0; i < length; i++)
		{	
			if (i2c_write(data[i])) return count;
			count++;
		}
		i2c_stop();
	}
	return count;
}

uint16_t i2c_readReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length)
{
	uint16_t i, count = 0;
	
	if(length > 0) {
		//request register
		if (i2c_start(devaddr | I2C_WRITE)) return count;
		i2c_write(regaddr);
		_delay_us(10);
	
		//read data
		if (i2c_start(devaddr | I2C_READ)) return count;
		for (i = 0; i < (length-1); i++)
		{	
			data[i] = i2c_read_ack();
			count++;
		}
		data[(length-1)] = i2c_read_nack();
		
		i2c_stop();
	}
	return ++count;

}

void i2c_stop(void)
{
	uint8_t  i2c_timer = 0;
	// transmit STOP condition
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	
	// wait until stop condition is executed and bus released
	i2c_timer = I2C_TIMER_DELAY;
	while((TWCR & (1<<TWSTO)) && i2c_timer--);
}
