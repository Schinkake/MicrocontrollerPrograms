/*
 * Fast_PWM.c
 *
 * Created: 10/5/2018 10:19:32 AM
 * Author : Adams
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

int ud;

int main(void)
{
	DDRD |= (1 << DDD6);
	// PD6 is now an output

	OCR0A = 128;
	// set PWM for 50% duty cycle

	TCCR0A |= (1 << COM0A1);
	// set none-inverting mode

	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	// set fast PWM Mode

	TCCR0B |= (1 << CS01);
	// set prescaler to 8 and starts PWM

	while (1) {
		
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
