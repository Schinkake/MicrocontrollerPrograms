/*
 * USART_Data_Receiver.c
 *
 * Created: 5/3/2019 10:43:16 AM
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

/*****Baudrate Einstellen*****
	
// UBRRnH bzw. UBRRnL sind in 12 Bits unterteilt, die 4 MSB sind in UBRRnH die restlichen 0-7 Bits in UBBRnL
	
// Gewünschte Baudrate: 9600
// Berechnet [fosc/(16*Baudrate)]-1 = 103 = B'0000 0110 0111
// Reelle Baudrate: 9615.3846153846
// Absolute Error = 15.3846153846
// Relative Error[%] = ((Reelle Baudrate / Gewünschte Baudrate) - 1) * 100% = 0.1602564103%
	
// Set baud rate
UBRR0H &= ~((UBRR0_11 << 1) | (UBRR0_10 << 1) | (UBRR0_9 << 1) | (UBRR0_8 << 1));
UBRR0L |= (UBRR0_6 << 1) | (UBRR0_5 << 1) | (UBRR0_2 << 1) |(UBRR0_1 << 1) | (UBRR0_0 << 1);
UBRR0L &= ~((UBRR0_7 << 1) | (UBRR0_4 << 1) | (UBRR0_3 << 1));
*/
/******Frame-Format*******

// Default frame format is 8 data bits, no parity, 1 stop bit

// Gewünschtes Frame-Format: 8-N-1 (8 - Data Bits, No Parity, 1 Stopp Bit)
UCSR0C &= ~((UMSEL01 << 1) | (UMSEL00 << 1) | (UPM01 << 1) | (UPM00 << 0) | (USBS0 << 1)); // Asynchronous USART, No Parity, 1 Stop-Bit

// 8 - Data Bits
UCSR0B &= ~(UCSZ02 << 1);
UCSR0C |= (UCSZ01 << 1) | (UCSZ00 << 1);
*/