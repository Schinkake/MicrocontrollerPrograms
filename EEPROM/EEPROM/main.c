/*
 * EEPROM.c
 *
 * Created: 4/5/2019 12:20:47 PM
 * Author : Adams
 */ 
#include <avr/io.h>

void	EEPROM_write(uint8_t Address, uint8_t Data);
uint8_t	EEPROM_read(uint8_t Address);

int main(void) {
	
	uint8_t data;
	
	DDRD |= (1 << PIND6);
	
	EEPROM_write(0x00, 0x04);
	
	data = EEPROM_read(0x00);
	
	if(data == 3)
		PORTD |= (1 << PIND6);
	
	while(1) {
		
	}
}

void EEPROM_write(uint8_t Address, uint8_t Data) {
	
	while(EECR & (1 << EEPE));
	
	EEAR = Address;
	EEDR = Data;
	EECR |= (1 << EEMPE);
	EECR |= (1 << EEPE);
}

uint8_t EEPROM_read(uint8_t Address) {
	
	while(EECR & (1 << EEPE));
	
	EEAR = Address;
	EECR |= (1 << EERE);
	return EEDR;
}