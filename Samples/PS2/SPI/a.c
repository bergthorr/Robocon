#ifndef F_CPU
#define F_CPU 16000000L
#endif


#include <util/delay.h>
#include <avr/io.h>
#include <math.h>

unsigned char spi(unsigned char data)
{
       //Start transmission
       SPDR = data;
       //Wait for transmission complete
       while (!(SPSR & (1<<SPIF)));
       return SPDR;
}

int main(void)
{
       int data0,data1,data2,data3,data4,data5,a,b,c,d=0;
      
       int x,y,z;
      
       // Port A initialization
       PORTA=0x00;
       DDRA=0xFE; //A0-ip 1234567-op 0b11111110

       // Port B initialization
       PORTB=0x00;
       DDRB=0xB0;
           // Port C initialization
       PORTC=0x00;
       DDRC=0xFF;

       // Port D initialization
       PORTD=0x00;
       DDRD=0x30;
      
       //init timer 1
       TCCR1A=0xA1;
       TCCR1B=0x0A;
       ICR1H=0x00;
       ICR1L=0x00;
       OCR1AH=0x00;
       OCR1AL=0x00;
       OCR1BH=0x00;
    //configuring spi in mode3
       SPCR=0x7E;
       SPSR=0x01;

       TWCR=0x00;


      PORTA|=(1<<PORTA3);
      while(d!= 0x73) // locking the controller on analog mode
      {
            
             PORTB|=(1<<PORTB5)|(1<<PORTB7); //set
             PORTB&=~(1<<PORTB4); //clr
            
            
             spi(0x01); //entering config mode
             spi(0x43);
             spi(0x00);
               spi(0x01);
             spi(0x00);
            
             PORTB|=(1<<PORTB5);
             _delay_ms(1);
             PORTB|=(1<<PORTB4);

             _delay_ms(10);

            
             PORTB|=(1<<PORTB5)|(1<<PORTB7); //setting
             
             PORTB&=~(1<<PORTB4); // clearing

             spi(0x01); //seting analog mode
             spi(0x44);
             spi(0x00);
             spi(0x01);
             spi(0x03);
             spi(0x00);
             spi(0x00);
             spi(0x00);
             spi(0x00);

             PORTB|=(1<<PORTB5);
             _delay_ms(1);
             PORTB|=(1<<PORTB4);

             _delay_ms(10);

            
             PORTB|=(1<<PORTB5)|(1<<PORTB7);
             PORTB&=~(1<<PORTB4);
                spi(0x01);/*was using this to poll.The code will work fine even without this*/
             spi(0x43);
             spi(0x00);
             spi(0x00);
             spi(0x5A);
             spi(0x5A);
             spi(0x5A);
             spi(0x5A);
             spi(0x5A);

             PORTB|=(1<<PORTB5);
             _delay_ms(1);
             PORTB|=(1<<PORTB4);
             _delay_ms(10);


            
            
             PORTB|=(1<<PORTB5)|(1<<PORTB7);
             PORTB&=~(1<<PORTB4);
             spi(0x01);
             d=spi(0x42); //making sure we're in the analog mode the value of d will be 0x73
             spi(0x00);   // if d is not equal to 0x73 this loop will repeat 
             spi(0x00);
             spi(0x00);
             spi(0x00);
             spi(0x00);
             spi(0x00);
             spi(0x00);
      PORTB|=(1<<PORTB5);
             _delay_ms(1);
             PORTB|=(1<<PORTB4);
             _delay_ms(10);
            
      }
              while(d==0x73) 
       {
         while (1)
     { 
     PORTC = 0xaa;
       PORTB|=(1<<PORTB5) | (1<<PORTB7);
       PORTB&=~(1<<PORTB4);
        a=spi(0x01);                               
        b=spi(0x42);                               
        c=spi(0x00);
                                             

        data0 = spi(0x00); //buttons set 1 8                     
        data1 = spi(0x00); //button set 2  8        
        data2 = spi(0x00); //  rx                          
        data3 = spi(0x00); //  ry                    
        data4 = spi(0x00); //  lx                     
        data5 = spi(0x00); //  ly                     

	PORTC = data0;
        _delay_us(1);
        PORTB|=(1<<PORTB5);                    
        _delay_us(1);
        PORTB|=(1<<PORTB4);
}

}
}
