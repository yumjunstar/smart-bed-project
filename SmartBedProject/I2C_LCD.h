/* 
* I2C_LCD.h
*
* Created: 2023-06-23 오후 9:23:28
* Author: AOJUCBJD
*/


#ifndef __I2C_LCD_H__
#define __I2C_LCD_H__

#include "TWI.h"
#include "USART_Control.h"
#include <util/delay.h>


class I2C_LCD {
	public:
	I2C_LCD(uint8_t lcd_address);
	void init();
	void clear();
	void setCursor(uint8_t col, uint8_t row);
	void writeChar(char character);
	void writeString(const char* str);
	
	private:
	uint8_t _lcd_address;
	TWI _twi;
	USART_Control USART;
	
	void send(uint8_t data, uint8_t mode);
	void write4bits(uint8_t bits, uint8_t mode);

protected:
private:
	I2C_LCD( const I2C_LCD &c );
	I2C_LCD& operator=( const I2C_LCD &c );

}; //I2C_LCD

#endif //__I2C_LCD_H__
