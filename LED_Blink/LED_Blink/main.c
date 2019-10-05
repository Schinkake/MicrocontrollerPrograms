/*
 * LED_Blink.c
 *
 * Created: 10/27/2018 2:27:11 AM
 * Author : Adams
 */ 

#define F_CPU 16000000L // Specify oscillator frequency
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB |= (1 << PINB5); // configure pin 7 of PORTB as output (digital pin 13 on the Arduino Mega2560)
	
	while(1)
	{
		PORTB |= (1 << PINB5); // set 7th bit to HIGH
		_delay_ms(50);
		PORTB &= ~(1 << PINB5); // set 7th bit to LOW
		_delay_ms(50);
	}
}

/*
int main(void) {
	
	DDRB |= (1 << PINB0) | (1 << PINB1) | (1 << PINB2) | (1 << PINB3);
	PORTB = 0x00;
	
	while(1) {
	    PORTB = 0b00001100;
	    _delay_ms(2);
	    PORTB = 0b00000110;
	    _delay_ms(2);
	    PORTB = 0b00000011;
	    _delay_ms(2);
	    PORTB = 0b00001001;
	    _delay_ms(2);
	}
}
*/