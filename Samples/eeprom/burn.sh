avr-gcc -g -Os -mmcu=$2 -c $3.c
avr-gcc -g -mmcu=$2 -o $3.elf $3.o
avr-objcopy -j .text -j .data -O ihex $3.elf $3.hex
sudo avrdude -c $1 -p $2 -U flash:w:$3.hex
