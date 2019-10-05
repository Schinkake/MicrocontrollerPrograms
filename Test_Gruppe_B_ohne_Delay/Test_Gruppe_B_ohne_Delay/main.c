/*
 * Test_Gruppe_B_ohne_Delay.c
 *
 * Created: 10/27/2018 12:52:46 AM
 * Author : Adams
 */ 
#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

int swap = 1;

ISR(TIMER1_COMPA_vect) {
	if(swap) {
		OCR0A = 5;
		swap = 0;
	}
	else {
		OCR0A = 243;
		swap = 1;
	}
}

int main(void)
{
	DDRD |= (1 << DDD6); // PD6 is an output

	TCCR0A |= (1 << COM0A1); // Clear OC0A on compare match when up-counting. Set OC0A on compare match when down-counting.

	TCCR0A |= (1 << WGM00); // Phase Correct PWM Mode with 0xFF (255) as TOP

	TCCR0B |= (1 << CS02) | (1 << CS00); // Prescaler equals 1024
	
	OCR0A = 0; // set duty cycle to 0%


	TCCR1B |= (1 << WGM12); // Set TCNT1 in CTC mode

	TCCR1B |= ((1 << CS10) | (1 << CS12)); // Prescaler equals 1024
	
	TIMSK1 |= (1 << OCIE1A); // Enable the COMPA CTC interrupt

	OCR1AL = (uint8_t)65535;//15625; // TOP value of TCNT1
	OCR1AH = (uint8_t)12590;
	sei();

	while (1)
	{
	}
}
