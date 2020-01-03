#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdio.h>

#ifndef AVR_BASE_H_
#define AVR_BASE_H_

#define sbi(reg, bit) reg |= (1 << bit)
#define cbi(reg, bit) reg &= ~(1 << bit)

#endif /* AVR_BASE_H_ */
