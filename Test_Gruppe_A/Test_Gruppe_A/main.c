/*
 * Test_Gruppe_A.c
 *
 * Created: 10/25/2018 3:40:03 PM
 * Author : Adams
 */ 
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRD |= (1 << DDD6);
	// PD6 is now an output

	OCR0A = 128;
	// set PWM for 50% duty cycle

	TCCR0A |= (1 << COM0A0);
	// set none-inverting mode

	TCCR0A |= (1 << WGM01);
	// set fast PWM Mode

	TCCR0B |= (1 << CS01);
	// set prescaler to 8 and starts PWM

	while (1)
	{
		if(ud) {
			OCR0A = OCR0A + 1;
			if(OCR0A == 255)
			ud = 0;
		}
		else {
			OCR0A = OCR0A - 1;
			if(OCR0A == 0)
			ud = 1;
		}
		_delay_ms(1);
	}
}
