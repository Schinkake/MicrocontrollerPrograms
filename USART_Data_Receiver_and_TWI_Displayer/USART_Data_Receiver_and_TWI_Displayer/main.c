/*
 * USART_Data_Receiver_and_TWI_Displayer.c
 *
 * Created: 5/3/2019 9:09:10 PM
 * Author : Adams
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE ((F_CPU / (USART_BAUDRATE * 16UL)) - 1)

uint8_t status_byte;

ISR(TWI_vect) {
	status_byte = TWSR;
	status_byte &= 0b11111000;
}

void USART_Init(void) {

	UBRR0L = BAUD_PRESCALE; // Load lower 8-bits into the low byte of the UBRR register
	UBRR0H = (BAUD_PRESCALE >> 8); // Load upper 8-bits into the high byte of the UBRR register

	UCSR0B = (1 << TXEN0) | (1 << RXEN0); // Enable receiver and transmitter and receive complete interrupt
	sei();
}

void USART_SendByte(uint8_t Data_Byte) {
	while(!(UCSR0A & (1 << UDRE0))); // Wait until last byte has been transmitted
	UDR0 = Data_Byte; // Transmit data
}

uint8_t USART_ReceiveByte() {
	while(!(UCSR0A & (1 << RXC0))); // Wait until a byte has been received and return received data
	return UDR0;
}

void sendPacket(int data_length, uint8_t address_byte, uint8_t instruction_byte, uint8_t data[]);
void recievePacket(int data_length, uint8_t address_byte, uint8_t clock_data[], uint8_t data[]);

void TWI_Init(uint8_t bitRate_byte) {
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
	
	int i;
	uint8_t Data[5];
	uint8_t seg_codes[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
	
	USART_Init();
	
	TWI_Init(0b01001000); // Enabling all needed interrupts, the Two Wire Interface and setting the Bit-Rate Register
	
	while(1) {
		
		for(i = 1; i < 5; i++) {
			Data[i] = seg_codes[USART_ReceiveByte()];
		}

		Data[0] = 0b01000111;
		sendPacket(5, 0x70, 0x00, Data); // Sending the data of the seconds and minutes to the display chip, 0x70 = Display Address
		_delay_ms(5);
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