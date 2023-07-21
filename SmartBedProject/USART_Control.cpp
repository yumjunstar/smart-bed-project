/* 
* USART_Control.cpp
*
* Created: 2023-06-08 오전 1:11:58
* Author: AOJUCBJD
*/


#include "USART_Control.h"
// default constructor
USART_Control::USART_Control()
{
	UCSR0B = 0b00011000; // 송수신 활성화, 인터럽트 활성화 //(1<<RXEN0)|(1<<TXEN0); 이렇게 써도 OK
	//비동기 모드, NO PARITY
	UCSR0C = 0b00000110; // 데이터 비트 수, 동기/비동기, Stop bit 수...
	// Baud rate
	UBRR0H = 0x00;
	UBRR0L = 103;

} //USART_Control

int USART_Control::pow_(int l, int h)
{
	int rr = 1;
	for (int i = 0; i<h;++i)
	{
		rr *= l;
	}
	return rr;
}
unsigned char USART_Control::USART0_RX_vect_funct(void) // USART0 에서 문자를 하나 읽는 함수
{
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
}
void USART_Control::USART0_TX_vect_funct(unsigned char data) // USART0 에서 문자를 하나 보내는 함수
{
	while (!(UCSR0A & (1<<UDRE0))); // UDRE =1 -> buffer empty -> ready to transmit
	UDR0 = data;
}
void USART_Control::USART0_TX_array_funct(char data[]) // USART0로 문자열을 보내는 함수
{
	for (size_t i = 0; data[i]!='\0'; ++i)
	{
		USART0_TX_vect_funct(data[i]);
	}
}
void USART_Control::USART0_TX_array_funct(double data, size_t precision) // USART0로 문자열을 보내는 함수
{
	char buffer_integer[100];	
	char* buffer_float = (char*)malloc (sizeof(char) * (precision + 1));
	int integer_part = (int)data; // CDS sell의 정수 부분
	int float_part = abs((int)((data - integer_part)*pow_(10, precision))); // 소수 부분


	itoa(integer_part, buffer_integer, 10);
	itoa(float_part, buffer_float, 10);
		
	this->USART0_TX_array_funct(buffer_integer);
	this->USART0_TX_vect_funct('.');
	this->USART0_TX_array_funct(buffer_float);
	free(buffer_float);
}
void USART_Control::USART0_TX_array_funct(int data) // USART0로 문자열을 보내는 함수
{
	char buffer_integer[100];
	itoa(data, buffer_integer, 10);
	this->USART0_TX_array_funct(buffer_integer);
	
}

void USART_Control::USART0_TX_endl()
{
	this->USART0_TX_vect_funct('\n');
	this->USART0_TX_vect_funct('\r');
}

// default destructor
USART_Control::~USART_Control()
{
} //~USART_Control
