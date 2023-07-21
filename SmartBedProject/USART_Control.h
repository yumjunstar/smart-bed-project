/* 
* USART_Control.h
*
* Created: 2023-06-08 오전 1:11:58
* Author: AOJUCBJD
*/


#ifndef __USART_CONTROL_H__
#define __USART_CONTROL_H__
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
class USART_Control
{
//variables
public:
protected:
private:
	static int pow_(int l, int h);
//functions
public:
	USART_Control();
	unsigned char USART0_RX_vect_funct(void); // USART0 에서 문자를 하나 읽는 함수
	void USART0_TX_vect_funct(unsigned char data); // USART0로 문자열을 보내는 함수
	void USART0_TX_array_funct(char *data);
	void USART0_TX_array_funct(double data, size_t precision = 3);
	void USART0_TX_array_funct(int data);
	void USART0_TX_endl();

	~USART_Control();
protected:
private:
	USART_Control( const USART_Control &c );
	USART_Control& operator=( const USART_Control &c );

}; //USART_Control

#endif //__USART_CONTROL_H__
