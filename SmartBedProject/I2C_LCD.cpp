#include "I2C_LCD.h"

I2C_LCD::I2C_LCD(uint8_t lcd_address) : _lcd_address(lcd_address << 1), _twi() {
	USART.USART0_TX_array_funct("LCD initializing");
	USART.USART0_TX_endl();
	init();
	USART.USART0_TX_array_funct("LCD initializing success ready to use");
	USART.USART0_TX_endl();
}

void I2C_LCD::init() {
	_delay_ms(50);
	// Initialization sequence
	write4bits(0x03 << 4, 0);
	_delay_ms(5);
	write4bits(0x03 << 4, 0);
	_delay_ms(5);
	write4bits(0x03 << 4, 0);
	_delay_us(150);
	write4bits(0x02 << 4, 0);
	send(0x28, 0); // Function set: 4-bit, 2-line, 5x8 dots
	send(0x08, 0); // Display off
	clear();
	send(0x06, 0); // Entry mode set: increment cursor by 1, no shift
	send(0x0C, 0); // Display on, cursor off, blink off
}

void I2C_LCD::clear() {
	send(0x01, 0);
	_delay_ms(2);
}

void I2C_LCD::setCursor(uint8_t col, uint8_t row) {
	static const uint8_t row_offsets[] = {0x00, 0x40};
	send(0x80 | (col + row_offsets[row]), 0);
}

void I2C_LCD::writeChar(char character) {
	send(character, 1);
}

void I2C_LCD::writeString(const char* str) {
	while (*str) {
		writeChar(*str++);
	}
}

void I2C_LCD::send(uint8_t data, uint8_t mode) {
	uint8_t high_nibble = data & 0xF0;
	uint8_t low_nibble = (data << 4) & 0xF0;
	write4bits(high_nibble | mode, 1);
	write4bits(low_nibble | mode, 1);
}

void I2C_LCD::write4bits(uint8_t bits, uint8_t mode) {
	_twi.twi_write(_lcd_address, bits | mode);
}
