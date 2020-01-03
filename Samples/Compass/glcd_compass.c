/*#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include "i2c_master.c"
#include "compass.c"
#include "glcd_lib.c"
#include "lcd_compass.c"

// #define sbi(reg, bit) reg |= (1 << bit)
// #define cbi(reg, bit) reg &= ~(1 << bit)
void glcd_compass(unsigned int i)
{

    unsigned char d;
	//unsigned int i;
	unsigned int x;
	//unsigned char msg[]="parshu la aarchi bhetlich pahije";
	int j;
	unsigned char p[4] ;
        for(j=2;j>=0;j--)
	{
	  p[j]=i%0x0A;
	  p[j]=p[j]+0x30;
	  i=i/0x0A;
	}
	p[3]='\0';
	glcd_write_string(p[0],4,0);
	_delay_ms(10);
    	
}

int main()
{
	DDRD = 0x00;
	PORTD = 0x03;
	float p;
	float pi;
    	i2c_init();
	port_pin_config();
	init_devices();    
	goto_xy(0,0);
    	glcd_write_char('1');
    	_delay_ms(1000);

	init_HMC5883L();
    	pi = getHeading(); //Initial compass reading when the code begins
    	//usart_send_int16(pi);
    	glcd_compass(pi);
    	uint16_t offset;	
	// LCD_init();
	DDRG = 0XFF;
	PORTG  = 0XAA;
	_delay_ms(1000);
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
		glcd_compass(offset);
		// val++;
		// _delay_ms(100);
		PORTG = offset;
	}
}*/

/*
 * ata_daddy_chidla.c

 *
 *  Created on: 05-Nov-2016
 *      Author: pinakwadikar(Daddy)
 *
 ** For MPU6050: write
 * START
 * 7 bit address of slave    +    (r/(w)')    (here     + 0)
 * 8 bit register address
 * acknowledge
 * 8 bit data in that register
 * acknowledge
 * stop

 * For MPU6050: read
 * START
 * 7 bit address of slave    +    (r/(w)')    (here     + 0)
 * 8 bit register address that is going to be  read
 * acknowledge
 * START (again)
 * 7 bit address of slave    +    (r/(w)')    (here     + 1)
 * acknowledge
 * data(From Slave)
 * ends with not acknowledge(From Master) SDA high while SCK full cycle
PC1 SDA
 * PC0 SCL*/
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include  <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "i2c_master.c"
#include "compass.c"
#include "glcd_lib.c"

#define sbi(x,y) x|=(1<<y)
#define cbi(x,y) x&=~(1<<y)

#define status (TW_STATUS & 0xF8)
#define address_w 0b11010000
#define address_r 0b11010001

void start_TWI()
{
    TWCR=((1<<TWEN)|(1<<TWINT)|(1<<TWSTA));
    while((TWCR & (1 << TWINT) ) == 0);
    while(status!=TW_START);
}
void address_write()
{
    TWDR=address_w;
    TWCR=((1<<TWEN)|(1<<TWINT));
    while((TWCR & (1 << TWINT) ) == 0);
    _delay_ms(10);
    while(status!=TW_MT_SLA_ACK);//SLA+W transmitted, ACK received
}
void address_read()
{
    TWDR=address_r;
    TWCR=((1<<TWEN)|(1<<TWINT));
    while((TWCR & (1 << TWINT) ) == 0);
    _delay_ms(10);
    while(status!=TW_MR_SLA_ACK);//SLA+R transmitted, ACK received
}
void data_send(unsigned char data)
{
    TWDR=data;
    TWCR=((1<<TWEN)|(1<<TWINT));
    while((TWCR & (1 << TWINT) ) == 0);
    _delay_ms(10);
    while(status!=TW_MT_DATA_ACK);//data transmitted, ACK received
}
void start_inter()
{
    TWCR=((1<<TWEN)|(1<<TWINT)|(1<<TWSTA));
    while((TWCR & (1 << TWINT) ) == 0);
    while(status!=TW_REP_START);//repeated start condition transmitted
}
unsigned char get_data()
{
    TWCR=((1<<TWEN)|(1<<TWINT));
    while((TWCR & (1 << TWINT) ) == 0);
    return(TWDR);
}
void stop_TWI()
{
    TWCR=((1<<TWEN)|(1<<TWINT)|(1<<TWSTO));
}
void data_write(unsigned char address,unsigned char data)
{
    start_TWI();
    address_write();
    data_send(address);//0x6B
    data_send(data);//0x02
    stop_TWI();
    _delay_ms(10);
}
unsigned char read_IMU(unsigned char address)
    {
        unsigned char data;
        start_TWI();
        address_write();
        data_send(address);//0x3E
        start_inter();
        address_read();
        data=get_data();
        stop_TWI();
        _delay_ms(10);
        return(data);
    }


void glcd_IMU(unsigned int i)
{

    int j;
    unsigned char p[4];

    _delay_ms(10);
                       //Configuring PORTJ as output
    for(j=2;j>=0;j--)
    {
      p[j]=i%10;
      p[j]=p[j]+'0';
      i=i/10;
    }  
    p[3]='\0';
    glcd_write_string(p,0,0);
    _delay_ms(10);
}

void glcd_compass(unsigned int i)
{

    unsigned char d;
	//unsigned int i;
	unsigned int x;
	//unsigned char msg[]="parshu la aarchi bhetlich pahije";
	int j;
	unsigned char p[4] ;
        for(j=2;j>=0;j--)
	{
	  p[j]=i%0x0A;
	  p[j]=p[j]+0x30;
	  i=i/0x0A;
	}
	p[3]='\0';
	glcd_write_string(p,4,0);
	_delay_ms(10);
    	
}

int main()
{
    unsigned char accl=0x00;
    unsigned char acch=0x00;
    DDRD=0x03;
    TWCR=(1<<TWEN);
    TWSR=0x00;
    TWBR=0x02;
    float pi;
    i2c_init();
    init_HMC5883L();
    pi = getHeading();
	port_pin_config();
	init_devices();    
	goto_xy(0,0);
    glcd_write_char('1');
    _delay_ms(1000);

    
    
    float p,offset,ass;
    int accumulator;
    while(1)
//read form 0x3E and 0x3D
    {
    //        data_write(0x6B,0x02);
    //data_write(0x6C,0x00);
    //data_write(0x1C,0x00);
        // accl=read_IMU(0x3E);
        // acch=read_IMU(0x3D); // Daddy Hagla
        // lcd_IMU(acch);
    /*int i;
        accumulator = 0;
        for(i=0;i<8;i++)
        {
            accumulator=accumulator+read_IMU(0x3D);
        }
        acch=(accumulator/8);
        glcd_IMU(acch);
        _delay_ms(100);
	*/
        init_HMC5883L();
        
        p = getHeading(); //Compass readings being polled
        offset = p - pi;
        if(p < pi)
            offset = pi - p;
        if(p > pi+180){
            ass = offset - 180;
            offset = 180 - ass;
        }
        glcd_compass(offset);
        i2c_stop();
	_delay_ms(5000);

    }

    return 0;
}
