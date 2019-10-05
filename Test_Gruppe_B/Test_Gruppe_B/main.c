/*
 * Test_Gruppe_B.c
 *
 * Created: 10/25/2018 2:59:17 PM
 * Author : Adams
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRD |= (1 << DDD6);
	// PD6 is now an output

	OCR0A = 0;
	// set PWM for 50% duty cycle

	TCCR0A |= (1 << COM0A1);
	// set none-inverting mode

	TCCR0A |= (1 << WGM00);
	// set fast PWM Mode

	TCCR0B |= (1 << CS02) | (1 << CS00);
	// set prescaler to 8 and starts PWM

	while (1)
	{
		OCR0A = 5;
		_delay_ms(500);
		OCR0A = 243;
		_delay_ms(500);
	}
}
