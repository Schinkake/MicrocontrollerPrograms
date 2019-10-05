/*
 * Stepper_Motor.c
 *
 * Created: 4/28/2019 2:20:00 PM
 * Author : Adams
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
	
	DDRD |= (1 << PIND6) | (1 << PIND5) | (1 << PIND4) | (1 << PIND3);
	
    while (1) {
		
		PORTD |= (1 << PIND6);
		PORTD |= (1 << PIND5);
		PORTD &= ~(1 << PIND4);
		PORTD &= ~(1 << PIND3);
		_delay_ms(2);
		PORTD &= ~(1 << PIND6);
		PORTD |= (1 << PIND5);
		PORTD |= (1 << PIND4);
		PORTD &= ~(1 << PIND3);
		_delay_ms(2);
		PORTD &= ~(1 << PIND6);
		PORTD &= ~(1 << PIND5);
		PORTD |= (1 << PIND4);
		PORTD |= (1 << PIND3);
		_delay_ms(2);
		PORTD |= (1 << PIND6);
		PORTD &= ~(1 << PIND5);
		PORTD &= ~(1 << PIND4);
		PORTD |= (1 << PIND3);
		_delay_ms(2);
    }
}