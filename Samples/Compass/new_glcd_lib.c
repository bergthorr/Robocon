
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "font.h"
#include <avr/interrupt.h>

#define E_DELAY 		 100
#define DATAPORT 		PORTK
#define CONTROLPORT		PORTF
#define CONTROLADDR		DDRF
#define DATADDR 		DDRK
#define DATAPIN 		PINK
#define chip_selec_1 (1<<PF0)	//chip select for controller 1
#define chip_selec_2 (1<<PF1)	//chip select for controller 2
#define RS  		 (1<<PF2)	//register select
#define RW  		 (1<<PF3)	//read/write
#define EN  		 (1<<PF4)	//enable
#define RST  		 (1<<PF5)	//reset

unsigned char cy = 0,cx=0;
char 		data[3];
unsigned char j=0,i=0;

void port_pin_config(void)
{			
	DATAPORT 	= 0xFF;	
	DATADDR  	= 0xFF;	//initialize as output
	CONTROLPORT = 0x00;
	CONTROLADDR = 0xFF;	//initialize as output
}

void trigger_pulse()
{
	CONTROLPORT |= EN;	//EN high
   _delay_us(E_DELAY);
   CONTROLPORT &= ~EN;	//EN low
   _delay_us(E_DELAY);
}
//------------------------------------------------------------------------------------------------------------
void glcd_on()
{
    CONTROLPORT &= ~RS;        			//RS low --> command
  	CONTROLPORT &= ~RW;        			//RW low --> write
  	
	DATAPORT = 0x3F;        			//ON command


	CONTROLPORT  |=  chip_selec_1;      //select chip 1
	CONTROLPORT  &= ~chip_selec_2;
	
	CONTROLPORT &= ~RST;				//logic low on reset
	CONTROLPORT |= RST;					//logic high on reset

	trigger_pulse();

	CONTROLPORT  &= ~chip_selec_1;      //select chip 2
	CONTROLPORT  |=  chip_selec_2;
	
	CONTROLPORT &= ~RST;				//logic low on reset
	CONTROLPORT |= RST;					//logic high on reset
	trigger_pulse();
}           
//------------------------------------------------------------------------------------------------------------
void glcd_off()
{
	CONTROLPORT &= ~RS;					  //RS low --> command
	CONTROLPORT &= ~RW;					  //RW low --> write
	DATAPORT = 0x3E; 					  //OFF command

	CONTROLPORT  |=  chip_selec_1;        //select chip 1
	CONTROLPORT  &= ~chip_selec_2;
	trigger_pulse();

	CONTROLPORT  &= ~chip_selec_1;        //selcet chip 2
	CONTROLPORT  |=  chip_selec_2;
	
	trigger_pulse();	
}     
//-------------------------------------------------------------------------------------------------------------
void set_start_line(unsigned char line)
{
  CONTROLPORT &= ~RS;					//RS low --> command
  CONTROLPORT &= ~RW;					//RW low --> write
   
  DATAPORT = 0xC0 | line;     			//Set Start Line command

  CONTROLPORT  |=  chip_selec_1;        //select chip 1
  CONTROLPORT  &= ~chip_selec_2;
  trigger_pulse();

  CONTROLPORT  &= ~chip_selec_1;        //select chip 2
  CONTROLPORT  |=  chip_selec_2;
  
  trigger_pulse();
}

//--------------------------------------------------------------------------------------------------------------
void goto_col(unsigned int x)
{
   unsigned char pattern;
   CONTROLPORT &= ~RS;					//RS low --> command
   CONTROLPORT &= ~RW;					//RW low --> write
   cy = x;
   if(x<64)             				//left section
   {
      CONTROLPORT |= chip_selec_1;		//deselect chip 1
      CONTROLPORT &= ~chip_selec_2;		//select chip 2

      pattern = x;      				//put column address on data port
   }
   else                 				//right section
   {
	   CONTROLPORT &= ~chip_selec_1;	//deselct chip 1
	   CONTROLPORT |= chip_selec_2;		//select chip 2

	   pattern = x-64;   				//put column address on data port
   }    
   pattern = (pattern | 0x40);  		//Command format
   DATAPORT = pattern; 
   trigger_pulse();     
}   
//-------------------------------------------------------------------------------------------------------------
void goto_row(unsigned int y)
{    
   unsigned char pattern;
   CONTROLPORT &= ~RS;	 //RS low --> command
   CONTROLPORT &= ~RW;	 //RW low --> write
   pattern = (y | 0xB8); //put row address on data port set command
   cx = y;
   DATAPORT = pattern;   
   trigger_pulse();             
}
//--------------------------------------------------------------------------------------------------------------
void goto_xy(unsigned int x,unsigned int y)
{    
    goto_col(x);	//column
    goto_row(y);	//row
}  
//--------------------------------------------------------------------------------------------------------------
void glcd_write(unsigned char b)
{
	CONTROLPORT |= RS;	//RS high --> data
	CONTROLPORT &= ~RW;	//RW low --> write
	DATAPORT = b;    	//put data on data port
   _delay_us(100);
   trigger_pulse();
}
//--------------------------------------------------------------------------------------------------------------
void glcd_clrln(unsigned char line)
{
    int i;
    //goto_xy(0,line);      //At start of line of left side
    //goto_xy(64,line);     //At start of line of right side 

    CONTROLPORT |= chip_selec_1;

    for(i=0;i<128;i++)
    {
    	goto_xy(i,line);
    	glcd_write(0);
    }
}
//---------------------------------------------------------------------------------------------------------------
void glcd_clear()
{
   int i;
   for(i=0;i<64;i++)
	   glcd_clrln(i);
}

void glcd_write_char(char charToWrite)	//character to display
{
	int i;
	if(cy<64)
	{	
	    CONTROLPORT |= chip_selec_1;	//select chip 1
	    CONTROLPORT &= ~chip_selec_2;	//deselect chip 2
	}
	else
	{
		CONTROLPORT &= ~chip_selec_1;	//deselct chip 1
	   	CONTROLPORT |= chip_selec_2;	//select chip 2
	}

	//cy=0;
	//cx=0;	
	for(i=0;i<32;i=i+2)
	{
	      goto_xy(cy,cx);	//cy=column,cx=row
		  glcd_write(pgm_read_byte(&(font[(32*(charToWrite-48)+i)])));	//address of character is provided in program read byte to read the bytes for character
		  cy++;
	 }
	 cx++;
	 cy=cy-16;
	 for(i=1;i<32;i=i+2)
	{
	      goto_xy(cy,cx);	//cy=column,cx=row
		  glcd_write(pgm_read_byte(&(font[(32*(charToWrite-48)+i)])));	//address of character is provided in program read byte to read the bytes for character
		  cy++;
	 }
}
//-------------------------------------------------------------------------------------------------------------------
void glcd_write_string(char *stringToWrite,unsigned char page,unsigned char col)
{
   cx=page;
   cy=col;
   while(*stringToWrite){
  	glcd_write_char(*stringToWrite++);
   	cx=page;
  }
}
//-------------------------------------------------------------------------------------------------------------------

void init_devices()
{
	glcd_on();				//initialize glcd
	glcd_clear();			//clear screen
	set_start_line(0);
}

