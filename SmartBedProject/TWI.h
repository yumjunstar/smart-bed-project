/* 
* TWI.h
*
* Created: 2023-06-21 오후 4:08:59
* Author: AOJUCBJD
*/


#ifndef __TWI_H__
#define __TWI_H__

#define F_CPU 16000000UL
#define sbi(PORTX,bitX) PORTX|=(1<<bitX)
#define cbi(PORTX,bitX) PORTX&=~(1<<bitX)
#define tbi(PORTX,bitX) PORTX^=(1<<bitX)




#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<math.h>
class TWI
{
//variables
public:
protected:
private:

//functions
public:
	TWI();
	void twi_write(unsigned char address,unsigned char data);
	unsigned char twi_read(char addressr);
	~TWI();
protected:
private:
	TWI( const TWI &c );
	TWI& operator=( const TWI &c );

}; //TWI

#endif //__TWI_H__
