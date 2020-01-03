#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
#include "i2c_master.h"

#define HMC5883L_WRITE 0x3C
#define HMC5883L_READ 0x3D 

void init_HMC5883L(void){

	i2c_start(HMC5883L_WRITE);
	i2c_write(0x00); // set pointer to CRA
	i2c_write(0x70); // write 0x70 to CRA
	i2c_stop();
	
	i2c_start(HMC5883L_WRITE);
	i2c_write(0x01); // set pointer to CRB
	i2c_write(0xA0);
	i2c_stop();
	
	i2c_start(HMC5883L_WRITE);
	i2c_write(0x02); // set pointer to measurement mode
	i2c_write(0x00); // continous measurement
	i2c_stop();
}

float getHeading(void){

	int16_t raw_x = 0;
	int16_t raw_y = 0;
	int16_t raw_z = 0;
	float headingDegrees = 0.00;

	i2c_start(HMC5883L_WRITE);
	i2c_write(0x03); // set pointer to X axis MSB
	i2c_stop();


	i2c_start(HMC5883L_READ);

	raw_x = ((uint8_t)i2c_read_ack())<<8;
	raw_x |= i2c_read_ack();

	raw_z = ((uint8_t)i2c_read_ack())<<8;
	raw_z |= i2c_read_ack();

	raw_y = ((uint8_t)i2c_read_ack())<<8;
	raw_y |= i2c_read_nack();

	i2c_stop();

	headingDegrees = (atan2((double)raw_y,(double)raw_x) * 57.29578);		//gives values between -180 to 180

	return headingDegrees;
}
