/*
 * TWI_Data_Transmission_from_Clock_Chip.c
 *
 * Created: 2/13/2019 8:31:10 PM
 * Author : Adams
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

uint8_t status_byte;

ISR(TWI_vect) {
	status_byte = TWSR;
	status_byte &= 0b11111000;
}

void sendPacket(int data_length, uint8_t address_byte, uint8_t instruction_byte, uint8_t data[]);
void recievePacket(int data_length, uint8_t address_byte, uint8_t clock_data[], uint8_t data[]);

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

void recieveDataByte() {
	TWCR |= (1 << TWINT) | (1 << TWEA);
	TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
}

int main(void) {
	
	uint8_t clock_data[2];
	uint8_t data[5];
	uint8_t seg_codes[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
	
	initTWI(0b01001000); // Enabling all needed interrupts, the Two Wire Interface and setting the Bit-Rate Register
	
	data[0] = 0b10000000; // Control Reg.: Function Mode should stay 32768Hz. Stop counting flag must be set
	data[1] = 0b00000000; // milli-seconds
	data[2] = 0b00000000; // seconds - Clock time
	data[3] = 0b00000000; // minutes
	data[4] = 0b00000000; // hours
	sendPacket(4, 0xA0, 0x00, data); // Sending the time data to the clock chip, 0xA0 = Clock Address,
	
	data[0] = 0b00000000; // Control Reg.: Stop counting flag must be reset
	sendPacket(1, 0xA0, 0x00, data);
	
	while(1) {
		 
		sendPacket(0, 0xA0, 0x02, data); // Preparing to start reading from the seconds register, 0xA0 = Clock Address, 
								   // instruction_byte = 2 sets position, so you can display milliseconds + seconds or seconds + minutes etc.
		recievePacket(1, 0xA0, clock_data, data); // Reading the values of the seconds and minutes

		data[0] = 0b01000111;
		data[1] = seg_codes[clock_data[1] >> 4];
		data[2] = seg_codes[clock_data[1] & 0b00001111];
		data[3] = seg_codes[clock_data[0] >> 4];
		data[4] = seg_codes[clock_data[0] & 0b00001111];
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

void recievePacket(int data_length, uint8_t address_byte, uint8_t clock_data[], uint8_t data[]) {
	
	int i = 0;
	
	TWCR |= (1 << TWSTA) | (1 << TWINT);
	
	while(!(i > data_length)) {
		
		while(!(TWCR & (1 << TWINT)));
		
		switch(status_byte) {
			case 0x08:
				sendDataByte(address_byte + 1);
				break;
			case 0x10:
				sendDataByte(address_byte + 1);
				break;
			case 0x38:
				TWCR |= (1 << TWINT) | (1 << TWSTA); // Wait until the Bus is free and then send a START
				TWCR &= ~(1 << TWSTO);
				break;
			case 0x40:
				recieveDataByte(); // Send instruction to recieve 1 data byte and return an ACK
				break;
			case 0x48:
				stopCondition();
				break;
			case 0x50:
				if(i < data_length) {
					clock_data[i] = TWDR;
					recieveDataByte();
					i++;
				}
				else {
					clock_data[i] = TWDR;
					TWCR |= (1 << TWINT); // Not ACK will be returned to get into status 0x58
					TWCR &= ~((1 << TWSTA) | (1 << TWSTO) | (1 << TWEA));
					i++;
				}
				break;
			case 0x58:
				stopCondition();
				break;
			default:
				break;
		}
		_delay_us(90);
	}
}

/*
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int i = 0;
int data_dir = 0;
int data_length;
uint8_t status_byte;
uint8_t address_byte;
uint8_t instruction_byte = 0;
uint8_t clock_data[2];
uint8_t data[5];
uint8_t seg_codes[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

void sendDataByte(int reg_value) {
	TWDR = reg_value;
	TWCR |= (1 << TWINT);
	TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
}

void recieveDataByte() {
	clock_data[i] = TWDR;
	TWCR |= (1 << TWINT) | (1 << TWEA);
	TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
}

void stopCondition() {
	TWCR |= (1 << TWINT) | (1 << TWSTO);
	TWCR &= ~(1 << TWSTA);
	i = 0;
}

ISR(TWI_vect) {
	
	status_byte = TWSR;
	status_byte &= 0b11111000;
	
	if(data_dir == 0) {
		
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
				}
				break;
			case 0x30:
				stopCondition();
				break;
			case 0x38:
				i = 0;
				TWCR |= (1 << TWINT) | (1 << TWSTA); // Wait until the Bus is free and then send a START
				TWCR &= ~(1 << TWSTO);
				break;
			default:
				break;
		}
	}
	else if(data_dir == 1) {
		
		switch(status_byte) {
			case 0x08:
				sendDataByte(address_byte + 1);
				break;
			case 0x10:
				sendDataByte(address_byte + 1);
				break;
			case 0x38:
				TWCR |= (1 << TWINT) | (1 << TWSTA); // Wait until the Bus is free and then send a START
				TWCR &= ~(1 << TWSTO);
				break;
			case 0x40:
				recieveDataByte(); // Send instruction to recieve 1 data byte and return an ACK
				break;
			case 0x48:
				stopCondition();
				break;
			case 0x50:
				if(i < data_length) {
					recieveDataByte();
					i++;
				}
				else {
					clock_data[i] = TWDR;
					TWCR |= (1 << TWINT); // Not ACK will be returned to get into status 0x58
					TWCR &= ~((1 << TWSTA) | (1 << TWSTO) | (1 << TWEA));
				}
				break;
			case 0x58:
				stopCondition();
				break;
			default:
				break;
		}
	}
}

int main(void) {
	
	TWBR = 0b01001000;
	TWCR |= (1 << TWIE) | (1 << TWEN);
	sei();
	
	data_length = 4; // Sending the time data to the clock chip
	address_byte = 0xA0; // Clock Address
	data[0] = 0b10000000; // Control Reg.: Function Mode should stay 32768Hz. Stop counting flag must be set
	data[1] = 0b00000000; // milli-seconds
	data[2] = 0b00000000; // seconds - Clock time
	data[3] = 0b00000000; // minutes
	data[4] = 0b00000000; // hours
	TWCR |= (1 << TWSTA) | (1 << TWINT);
	_delay_ms(10);

	data_length = 1;
	data[0] = 0b00000000; // Control Reg.: Function Mode should stay 32768Hz. Stop counting flag must be reset
	TWCR |= (1 << TWSTA) | (1 << TWINT);
	_delay_ms(10);
	
	while(1) {

		data_length = 0; // Preparing to start reading from the seconds register
		instruction_byte = 2; // sets position, so you can display milliseconds + seconds or seconds + minutes etc.
		address_byte = 0xA0; // Clock Address
		TWCR |= (1 << TWSTA) | (1 << TWINT);
		
		_delay_ms(10);
		
		data_dir = 1; // Reading the values of the seconds and minutes
		data_length = 1;
		TWCR |= (1 << TWSTA) | (1 << TWINT);
		_delay_ms(10);
		
		data_dir = 0; // Sending the data of the seconds and minutes to the display chip
		data_length = 5;	
		instruction_byte = 0;
		address_byte = 0x70; // Display Address
		data[0] = 0b01000111;
		data[1] = seg_codes[clock_data[1] >> 4];
		data[2] = seg_codes[clock_data[1] & 0b00001111];
		data[3] = seg_codes[clock_data[0] >> 4];
		data[4] = seg_codes[clock_data[0] & 0b00001111];
		TWCR |= (1 << TWSTA) | (1 << TWINT);
		_delay_ms(10);
	}
}
*/