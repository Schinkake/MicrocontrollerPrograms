/*
 * PWM_Poti_Control.c
 *
 * Created: 9/9/2019 1:26:39 PM
 * Author : Adams
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

int main(void) {

	DDRB |= (1 << PINB1); // PIN 9 on Arduino

	TCCR1A |= (1 << COM1A1); // set none-inverting mode

	TCCR1A |= (1 << WGM11) | (1 << WGM10); // set fast PWM Mode

	TCCR1B |= (1 << CS11); // set prescaler to 8 and starts PWM
	
	ADCSRA |= (1 << ADEN); 									// "ADC Enable" schaltet den ADC ein
	
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);	// Prescaler wird auf 128 eingestellt und man erhält bei 16MHz eine Frequenz von 125kHz

	while(1) {
		
		ADCSRA |= (1 << ADSC);
		
		while(ADCSRA & (1 << ADSC));
		
		OCR1A = ADC;
	}
}