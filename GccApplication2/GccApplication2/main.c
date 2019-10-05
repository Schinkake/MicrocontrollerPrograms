/*
 * GccApplication2.c
 *
 * Created: 3/7/2019 9:44:23 PM
 * Author : Adams
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>


int main()
{
	uint8_t brightness;
			
	// make sure to make OC0 pin (pin PB3 for atmega32) as output pin
	DDRD |= (1<<PIND6);
	
	// initialize timer0 in PWM mode
	TCCR0A |= (1<<WGM01)|(1<<WGM00)|(1<<COM0A1);
	TCCR0B |= (1<<CS00);

	// run forever
	while(1)
	{
		// increasing brightness
		for (brightness = 0; brightness < 255; ++brightness)
		{
			// set the brightness as duty cycle
			OCR0A = brightness;
			
			// delay so as to make the user "see" the change in brightness
			_delay_ms(5);
		}
		
		// decreasing brightness
		for (brightness = 255; brightness > 0; --brightness)
		{
			// set the brightness as duty cycle
			OCR0A = brightness;
			
			// delay so as to make the user "see" the change in brightness
			_delay_ms(5);
		}
		
		// repeat this forever
	}
	return 0;
}