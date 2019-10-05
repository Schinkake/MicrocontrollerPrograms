/*
 * DIC-Test2_ADC_B.cpp
 *
 * Created: 26.04.2019 09:58:14
 * Author : schelluk
 */ 

/* Angabe:
 * Input = ADC3
 * ADC-Clock = 125kHz
 * Aufl�sung = 10bit
 * AREF = intern
 */

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

float Messung = 0;
int data[6];
	
ISR(ADC_vect) {
	int ADC_wert = ADCL + (ADCH*256);
	Messung = (ADC_wert+1)*1.1/1024;
}

int main(void) {

	DDRD = 0xFF;
    ADMUX |= (1<<REFS1) | (1<<REFS0) | (1<<MUX1) | (1<<MUX0);
	ADCSRA |= (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	
	sei();

	
    while (1) {
		
		ADCSRA |= (1<<ADSC);
		_delay_ms(500);
		int(Messung/10)%10;	//Digit 1
		int(Messung*1)%10;	//Digit 2
		int(Messung*10)%10;	//Digit 3
		int(Messung*100)%10;	//Digit 4
		PORTD |= (1 << PIND6); // set 7th bit to HIGH
    }
}

