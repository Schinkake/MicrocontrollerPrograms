/*
 * ADC_I2C.c
 *
 * Created: 4/26/2019 11:26:00 AM
 * Author : Adams
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int messung;
uint8_t status_byte;

ISR(ADC_vect) {
	messung = ADC;
}

ISR(TWI_vect) {
	status_byte = TWSR;
	status_byte &= 0b11111000;
}

void sendPacket(int data_length, uint8_t address_byte, uint8_t instruction_byte, uint8_t data[]);

void initTWI(uint8_t bitRate_byte) {
	TWBR = bitRate_byte;
	TWCR |= (1 << TWIE) | (1 << TWEN);
	sei();
}

void stopCondition() {
	TWCR |= (1 << TWINT) | (1 << TWSTO);
	TWCR &= ~(1 << TWSTA);
}

void sendDataByte(int reg_value) {
	TWDR = reg_value;
	TWCR |= (1 << TWINT);
	TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
}

int main(void) {

	int i, temp;
	uint8_t data[5];
	uint8_t seg_codes[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

	ADMUX |= (1 << REFS1) | (1 << REFS0) | (1 << MUX3);		// Set the internal reference and mux
	
	ADCSRA |= (1 << ADEN) | (1 << ADIE); 					// "ADC Enable" schaltet den ADC ein und der Interrupt für wenn eine Conversion fertig ist ein
	
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);	// Prescaler wird auf 128 eingestellt und man erhält bei 16MHz eine Frequenz von 125kHz

	initTWI(0b01001000); // Enabling all needed interrupts, the Two Wire Interface and setting the Bit-Rate Register

	sei();
	
	while(1) {
		
		ADCSRA |= (1 << ADSC);
		
		_delay_ms(200);
		
		data[0] = 0b01000111;

		temp = (int)((messung - 324.31) * 100)/ 1.22; // Umrechnung in Decimal z.B: 2269
		for(i = 4; temp != 0; i--) { // In der for-Schleife teilt er diese Zahl in ihre einzelnen Stellen auf z.B: 2 2 6 9 und speichert
									 // sie in 7-Seg Form in dem Daten-Array
			data[i] = seg_codes[temp % 10];
			temp = temp / 10;
		}
		sendPacket(5, 0x70, 0x00, data); // Sending the data of the seconds and minutes to the display chip, 0x70 = Display Address
	}
}

void sendPacket(int data_length, uint8_t address_byte, uint8_t instruction_byte, uint8_t data[]) {
	
	int i = 0;
	
	TWCR |= (1 << TWSTA) | (1 << TWINT);
	
	while(!(i > data_length)) {
		
		while(!(TWCR & (1 << TWINT)));
		
		switch(status_byte) {
			case 0x08:
			sendDataByte(address_byte);
			break;
			case 0x10:
			sendDataByte(address_byte);
			break;
			case 0x18:
			sendDataByte(instruction_byte);
			break;
			case 0x20:
			stopCondition();
			break;
			case 0x28:
			if(i < data_length) {
				sendDataByte(data[i]);
				i++;
			}
			else {
				stopCondition();
				i++;
			}
			break;
			case 0x30:
			stopCondition();
			break;
			case 0x38:
			TWCR |= (1 << TWINT) | (1 << TWSTA); // Wait until the Bus is free and then send a START
			TWCR &= ~(1 << TWSTO);
			break;
			default:
			break;
		}
		_delay_us(90);
	}
}