/*
 * Halim_test.c
 *
 * Created: 6/7/2019 12:20:21 PM
 * Author : Adams
 */ 

#include <avr/io.h>

int messung;

int main()
{
	DDRD |= 0xFF;
	ADMUX &= ~((1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0));
	ADMUX |= ((1<<REFS0)|(1<<REFS1));
	ADCSRA |= 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0;
	ADCSRA |= 1<<ADEN;
	


	while(1)
	{
		ADCSRA |= (1<<ADSC);
		
		while(ADCSRA &(1<<ADSC));
		
		messung = ADC;
		
		
		if (messung >= 0)
		{
			PORTD |= (1<<2);
		}
		else
		{
			PORTD &= ~(1<<2);
		}
		if (messung >= 244)
		{
			PORTD |= (1<<3);
		}
		else
		{
			PORTD &= ~(1<<3);
		}
		if (messung >= 511)
		{
			PORTD |= (1<<4);
		}
		else
		{
			PORTD &= ~(1<<4);
		}
		if (messung >= 767)
		{
			PORTD |= (1<<5);
		}
		else
		{
			PORTD &= ~(1<<5);
		}
		if (messung == 1023)
		{
			PORTD |= (1<<6);
		}
		else
		{
			PORTD &= ~(1<<6);
		}
	}
}