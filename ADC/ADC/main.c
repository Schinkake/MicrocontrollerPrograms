/*
 * ADC.c
 *
 * Created: 4/26/2019 10:26:12 AM
 * Author : Adams
 */ 
/*
#include <avr/io.h>
#include <avr/interrupt.h>

int messung;

ISR(ADC_vect) {
	messung = ADC;
}

int main(void) {
		
	DDRD |= (1 << PIND4);
	
	ADCSRA |= (1 << ADEN) | (1 << ADIE); 													// "ADC Enable" schaltet den ADC ein
		
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);						// Prescaler wird auf 128 eingestellt und man erhält bei 16MHz eine Frequenz von 125kHz

	sei();
	
    while(1) {
		
		ADCSRA |= (1 << ADSC);
		
		if(messung == 0)
			PORTD |= (1 << PIND4);
		else
			PORTD &= ~(1 << PIND4);
    }
}*/

#include <avr/io.h>

int main(void) {
	
	int messung;
	
	DDRD |= (1 << PIND4);
	
	ADCSRA |= (1 << ADEN); 									// "ADC Enable" schaltet den ADC ein
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);	// Prescaler wird auf 128 eingestellt und man erhält bei 16MHz eine Frequenz von 125kHz

	while(1) {
		
		ADCSRA |= (1 << ADSC);
		
		while(ADCSRA & (1 << ADSC));
		
		messung = ADC;
		
		if(messung == 0)
			PORTD |= (1 << PIND4);
		else
			PORTD &= ~(1 << PIND4);
	}
}