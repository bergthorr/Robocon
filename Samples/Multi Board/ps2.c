/*
	PS2 library
	The following bitmaps are set for 14.MHz
*/
#define LEFT		0b01111100
#define RIGHT		0b10011100
#define UP		0b11001100
#define DOWN		0b00111100
#define TRIANGLE	0b11001111
#define CROSS		0b00111111
#define CIRCLE		0b10011111
#define SQUARE		0b01111111
#define L_ONE		0b11110011 			//Flickering
#define L_TWO		0b11111000
#define R_ONE		0b11100111
#define R_TWO		0b11111001	//0b11110001
#define SELECT		0b11111000
#define START		0b11100100
#define L_THREE		0b11111000	//0b11110000
#define R_THREE		0b11110000 	//0b11100000 	//Flickering
#define RJ_UP		0x00
#define RJ_DOWN		0b11111100
#define RJ_LEFT		0x00
#define RJ_RIGHT	0xff
#define LJ_UP		0x00
#define LJ_DOWN		0b11111100
#define LJ_LEFT		0x00
#define LJ_RIGHT	0xfc

#define PSdata             1 //PC1
#define PScommand          2 //PC2
#define PSclock            7 //PC7
#define PSattention        6 //PC6

#define sbi(x,y)  x|=(1<<y)
#define cbi(x,y)  x&=~(1<<y)

unsigned char data[6];

int gameByte(short int command) {
	short int i ;
	_delay_us(1);
	short int data = 0x00;					// clear data variable to save setting low bits later.
	for(i = 0;i < 8;i++) {
		if(command & _BV(i))
			sbi(PORTC, PScommand);			// bit bang "command" out on PScommand wire.
		else
			cbi(PORTC, PScommand);
		cbi(PORTC, PSclock);				// CLOCK LOW
		_delay_us(1);					// wait for output to stabilize
		if((PINC & _BV(PSdata)))
			sbi(data, i);				// read PSdata pin and store
		else
			cbi(data, i);
		sbi(PORTC, PSclock);				// CLOCK HIGH
	}
	sbi(PORTC, PScommand);
	_delay_us(20);						// wait for ACK to pass.
	return(data);
}

void  int_PS2inanalougemode() {
	int cnt = 0;
	while(cnt < 25)
	{
	// put controller in config mode
	sbi(PORTC, PScommand);
	sbi(PORTC, PSclock);
	cbi(PORTC, PSattention);
	
	gameByte(0x01);
	gameByte(0x43);
	gameByte(0x00);
	gameByte(0x01);
	gameByte(0x00);
	
	sbi(PORTC, PScommand);
	_delay_ms(1);
	sbi(PORTC, PSattention);
	_delay_ms(10);
	
	// put controller in analoge mode
	sbi(PORTC, PScommand);
	sbi(PORTC, PSclock);
	cbi(PORTC, PSattention);
	
	gameByte(0x01);
	gameByte(0x44);
	gameByte(0x00);
	gameByte(0x01);
	gameByte(0x03);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x00);
	
	sbi(PORTC, PScommand);
	_delay_ms(1);
	sbi(PORTC, PSattention);
	_delay_ms(10);
	
	// exit config mode
	sbi(PORTC, PScommand);
	sbi(PORTC, PSclock);
	cbi(PORTC, PSattention);
	
	gameByte(0x01);
	gameByte(0x43);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x5A);
	gameByte(0x5A);
	gameByte(0x5A);
	gameByte(0x5A);
	gameByte(0x5A);
	
	sbi(PORTC, PScommand);
	_delay_ms(1);
	sbi(PORTC, PSattention);
	_delay_ms(10);
	
	// poll controller and check in analouge mode.
	sbi(PORTC, PScommand);
	sbi(PORTC, PSclock);
	cbi(PORTC, PSattention);
	
	gameByte(0x01);
	gameByte(0x42);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x00);
	gameByte(0x00);
	
	sbi(PORTC, PScommand);
	_delay_ms(1);
	sbi(PORTC, PSattention);
	_delay_ms(10);
	cnt++;
	}
}
void init_PS2() {

	sbi(DDRC, PC7);
	cbi(DDRC, PC1);
	sbi(PORTC, PC1);
	cbi(DDRC, PC3);
	sbi(PORTC, PC3);
	sbi(DDRC, PC2);
	sbi(DDRC, PC6);
	int_PS2inanalougemode();
}

short int PS2_commn() {
	short int temp;
	
	sbi(PORTC, PScommand);		// start communication with PS2 controller
	sbi(PORTC, PSclock);
	cbi(PORTC, PSattention);
	
	gameByte(0x01);			// bite 0. header.
	temp = gameByte(0x42);		// bite 1. header. (should possibly put test on this byte to detect unplugging of controller.)
	gameByte(0x00);			// bite 2. header.
	
	data[0] = gameByte(0x00);	// bite 3. first data bite.
	data[1] = gameByte(0x00);	// bite 4.
	data[2] = gameByte(0x00);	// bite 5.
	data[3] = gameByte(0x00);	// bite 6.
	data[4] = gameByte(0x00);	// bite 7.
	data[5] = gameByte(0x00);	// bite 8.
	
	_delay_us(1);			
	sbi(PORTC, PScommand);		// close communication with PSx controller
	_delay_us(1);
	sbi(PORTC, PSattention);	// all done.
	_delay_ms(30);
	return 1;
}

