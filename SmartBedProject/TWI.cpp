/* 
* TWI.cpp
*
* Created: 2023-06-21 오후 4:08:58
* Author: AOJUCBJD
*/



#include "TWI.h"

// default constructor
TWI::TWI()
{
	
	//TWI(I2C)
	TWCR = (1<<TWEN);
	TWBR = 12;	 		//400khz
} //TWI

void TWI::twi_write(unsigned char address,unsigned char data)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//START

	while(!(TWCR & (1<<TWINT))); //TWINT flag 기다림
	while((TWSR&0xF8) != 0x08);  //START 상태(08) 기다림

	TWDR = 0b11010000;			 //AD(1101000)+W(0)
	TWCR = (1<<TWINT)|(1<<TWEN); //전송

	while(!(TWCR & (1<<TWINT)));
	while((TWSR&0xF8) != 0x18);  //SLA+W ACK 상태(18) 기다림

	TWDR = address; 			 //register address
	TWCR = (1<<TWINT)|(1<<TWEN); //전송

	while(!(TWCR & (1<<TWINT)));
	while((TWSR&0xF8) != 0x28);  //Data ACK 상태(28) 기다림

	TWDR = data; 				 //data
	TWCR = (1<<TWINT)|(1<<TWEN); //전송

	while(!(TWCR & (1<<TWINT)));
	while((TWSR&0xF8) != 0x28);

	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN); //STOP
}

unsigned char TWI::twi_read(char address)
{
	unsigned char data;

	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//START

	while(!(TWCR & (1<<TWINT))); //TWINT flag 기다림
	while((TWSR&0xF8) != 0x08);  //START 상태(08) 기다림

	TWDR = 0b11010000;			 //AD(1101000)+W(0)
	TWCR = (1<<TWINT)|(1<<TWEN); //전송

	while(!(TWCR & (1<<TWINT)));
	while((TWSR&0xF8) != 0x18);  //SLA+W ACK 상태(18) 기다림

	TWDR = address; 			 //register address
	TWCR = (1<<TWINT)|(1<<TWEN); //전송

	while(!(TWCR & (1<<TWINT)));
	while((TWSR&0xF8) != 0x28);  //Data ACK 상태(28) 기다림

	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//Repeat START

	while(!(TWCR & (1<<TWINT)));
	while((TWSR&0xF8) != 0x10);  //Repeat START 상태(08) 기다림

	TWDR = 0b11010001;			 //AD(1101000)+R(1)
	TWCR = (1<<TWINT)|(1<<TWEN); //전송

	while(!(TWCR & (1<<TWINT)));
	while((TWSR&0xF8) != 0x40);  //SLA+R ACK 상태(40) 기다림

	TWCR = (1<<TWINT)|(1<<TWEN); //전송

	while(!(TWCR & (1<<TWINT)));
	while((TWSR&0xF8) != 0x58);  //ACK 상태(58) 기다림

	data = TWDR;

	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN); //STOP

	return data;
}

// default destructor
TWI::~TWI()
{
} //~TWI
