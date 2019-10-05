/*
 * TWI_Data_Transmission.c
 *
 * Created: 11/23/2018 10:25:22 AM
 * Author : Adams
 */

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int i = 0;
int status_value;
int adress_byte;
int instruction_byte;
int control_byte;
int data_length;
int data[4];
int seg_codes[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
	
void sendDataByte(int reg_value) {
	TWDR = reg_value;
	TWCR |= (1 << TWINT);
	TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
}

ISR(TWI_vect) {
	
	status_value = TWSR;
	status_value &= 0b11111000;
	
	switch(status_value) {
		case 0x08:
			sendDataByte(adress_byte);
			break;
		case 0x10:
			i = 0;
			sendDataByte(adress_byte);
			break;
		case 0x18:
			sendDataByte(instruction_byte);
			break;
		case 0x20:
			sendDataByte(instruction_byte);
			break;
		case 0x28:
			if(i == 0) {
				sendDataByte(control_byte);
				i++;
			}
			else if(i < data_length + 1) {
				sendDataByte(seg_codes[data[i-1]]);
				i++;
			}
			else {
				i = 0;
				TWCR |= (1 << TWINT) | (1 << TWSTO);
				TWCR &= ~(1 << TWSTA);
			}
			break;
		case 0x30:
			if(i == 1) {
				sendDataByte(control_byte);
				i++;
			}
			else if(i < 6) {
				sendDataByte(seg_codes[data[i]]);
				i++;
			}
			else {
				i = 0;
				TWCR |= (1 << TWINT) | (1 << TWSTA);
				TWCR &= ~(1 << TWSTO);
			}
			break;
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
	
	adress_byte = 0x70;
	data_length = 4;
	instruction_byte = 0;
	control_byte = 0b01000111;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;
	
	TWBR = 0b01001000;
	TWCR |= (1 << TWIE) | (1 << TWEN);
	sei();
	
    while (1) {
		TWCR |= (1 << TWSTA) | (1 << TWINT);
		_delay_ms(100);
		if(data[3] == 9) {
			data[3] = 0;
			if(data[2] == 9) {
				data[2] = 0;
				if(data[1] == 9) {
					data[1] = 0;
					if(data[0] == 9) {
						data[0] = 0;
					}
					else
						data[0]++;
				}
				else 
					data[1]++;
			}
			else 
				data[2]++;
		}
		else
			data[3]++;
    }
}