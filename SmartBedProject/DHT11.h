/* 
* DHT11.h
*
* Created: 2023-06-21 오후 10:35:37
* Author: AOJUCBJD
*/


#ifndef __DHT11_H__
#define __DHT11_H__
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

#define DHT11_PIN 7  // DHT11 센서 데이터 핀
#include "USART_Control.h"
class DHT11
{
//variables
public:
protected:
private:
	USART_Control USART;
//functions
public:
	int8_t readDHT11(uint8_t* temperature, uint8_t* humidity);
	int8_t readDHT11(uint8_t* humidity);
	DHT11();
	~DHT11();
protected:
private:
	DHT11( const DHT11 &c );
	DHT11& operator=( const DHT11 &c );

}; //DHT11

#endif //__DHT11_H__
