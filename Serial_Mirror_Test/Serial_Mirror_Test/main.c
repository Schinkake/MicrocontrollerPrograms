/*
 * Serial_Mirror_Test.c
 *
 * Created: 9/6/2019 2:35:46 PM
 * Author : Adams
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE ((F_CPU / (USART_BAUDRATE * 16UL)) - 1)

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

int main(void) {
	
	int i;
	uint8_t Data[6]; 
	
	USART_Init();

/*	for(i = 0; i < 6; i++) {
		Data[i] = USART_ReceiveByte();
	}
*/
    while(1) {
		USART_SendByte(USART_ReceiveByte());
	}
}