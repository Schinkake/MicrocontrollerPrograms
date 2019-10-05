/*
 * TWI_Data_Transmission_Clock.c
 *
 * Created: 12/17/2018 1:35:32 PM
 * Author : Adams
 */ 
#define F_CPU 16000000
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

int i = 0, j = 0, arr[4] = {0};
int status_value, instructionByte = 0;
int seg_codes[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
	
void sendDataByte(int reg_value) {
	TWDR = reg_value;
	TWCR |= (1 << TWINT);
	TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
}

int counter(int cnt) {
	switch(cnt) {
		case 0:
			if(arr[cnt] > 9) {
				arr[cnt] = 0;
				return arr[cnt]++;
			}
			else {
				return arr[cnt]++;
			}
			break;
		case 1:
			if(arr[cnt] > 9) {
				arr[cnt] = 0;
				return arr[cnt]++;
			}
			else {
				return arr[cnt]++;
			}
			break;
		case 2:
			if(arr[cnt] > 9) {
				arr[cnt] = 0;
				return arr[cnt]++;
			}
			else {
				return arr[cnt]++;
			}
			break;
		case 3:
			if(arr[cnt] > 9) {
				arr[cnt] = 0;
				return arr[cnt]++;
			}
			else {
				return arr[cnt]++;
			}
			break;
		default:
			return -1;
	}
}

ISR(TWI_vect) {
	
	status_value = TWSR;
	status_value &= 0b11111000;
	
	switch(status_value) {
		case 0x08:
			sendDataByte(0x70);
			break;
		case 0x10:
			sendDataByte(0x70);
			break;
		case 0x18:
			if(instructionByte == 0) {
				sendDataByte(instructionByte);
				instructionByte = 4;
			}
			else if(instructionByte > 0) {
				sendDataByte(instructionByte);
				instructionByte--;
			}
			break;
/*		case 0x20:
			instructionByte--;
			sendDataByte(instructionByte);
			if(instructionByte > 4)
				instructionByte = 0;
			else
				instructionByte++;
			break; */
		case 0x28:
			switch(i) {
				case 0:
					sendDataByte(0b01000111);
					_delay_ms(400);
					TWCR |= (1 << TWINT) | (1 << TWSTA);
					TWCR &= ~(1 << TWSTO);
					i = 1;
					break;
				case 1:
					if(j < 4) {
						sendDataByte(seg_codes[counter(j)]);
						_delay_ms(400);
						TWCR |= (1 << TWINT) | (1 << TWSTA);
						TWCR &= ~(1 << TWSTO);
						j++;
					}
					else if(j == 4) {
						i = 0;
						j = 0;
					}
					break;
			}

			break;
/*		case 0x30:
			if(i == 1) {
				sendDataByte(0b01000111);
				i++;
			}
			else if(i == 2) {
				sendDataByte(seg_codes[counter(--i)]);
				i++;
			}
			else if(i == 3) {
				i = 0;
				_delay_ms(400);
				TWCR |= (1 << TWINT) | (1 << TWSTA);
				TWCR &= ~(1 << TWSTO);
			}
			break; */
		case 0x38:
			i = 0;
			TWCR |= (1 << TWINT) | (1 << TWSTA);
			TWCR &= ~(1 << TWSTO);
			break;
		default:
			return;
	}
}

int main(void) {
	
	TWBR = 0b01001000;
	TWSR = 0b11111000;
	TWCR |= (1 << TWIE);
	sei();
	
	TWCR |= (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	
    while (1) {	    
    }
}
/*

#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include <avr/interrupt.h>

int status, adresse, instr_byte, control_byte, zaehler, digit;

ISR(TWI_vect)
{
	status = TWSR;
	status &= 0b11111000;

	switch (status)
	{case (0x8):
		{
			TWDR = adresse;
			TWCR |= (1 << TWINT);
			TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
		}	break;
		case (0x18):
		{
			TWDR = instr_byte;
			TWCR |= (1 << TWINT);
			TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
		}	break;
		case (0x20):
		{
			TWDR = instr_byte;
			TWCR |= (1 << TWINT);
			TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
		}	break;
		case (0x28):
		{
			if (zaehler == 5)
			{
				TWDR = control_byte;
				TWCR |= (1 << TWINT);
				TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
				zaehler--;
				break;
			}
			if (zaehler == 0)
			{
				PORTB ^= (1 << PB5);
				_delay_ms(3000);
				PORTB ^= (1 << PB5);
				TWCR |= (1 << TWSTO) | (1 << TWINT);	// sendet Stop Bedingung
				TWCR &= ~(1 << TWSTA);
				break;
			}
			TWDR = digit;
			TWCR |= (1 << TWINT);
			TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
			zaehler--;
		}	break;
	}
	
}


int main(void)
{
	DDRB |= (1 << PB5);
	TWBR = 0b01001000;		// ergibt mit Prescale = 1 eine 100kHz Takt;
	TWSR = 0b11111000;		// Reset Wert, Bit 0 und Bit 1 stellen Prescaler auf Wert 1;
	TWCR |= (1 << TWIE);	// Enabled TW-Bus interrupt
	sei();
	
	PORTB &= ~(1 << PB5);
	adresse = 0x70;
	zaehler = 6;
	digit = 0b01111111;
	instr_byte = 0;
	control_byte = 0b00010111;
	
	TWCR |= (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);	// sendet Startbedingung
	while (1)
	{
		_delay_ms(5000);
		while(1)
		{
			PORTB ^= (1 << PB5);
			_delay_ms(100);
		}
	}
	return 0;
}
*/