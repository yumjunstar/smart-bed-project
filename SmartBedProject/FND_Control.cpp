/*
* FND_Control.cpp
*
* Created: 2023-06-24 오전 1:04:46
* Author: AOJUCBJD
*/


#include "FND_Control.h"
const uint8_t FND_Control::patterns[10] = {
	0b00111111, // 0
	0b00000110, // 1
	0b01011011, // 2
	0b01001111, // 3
	0b01100110, // 4
	0b01101101, // 5
	0b01111101, // 6
	0b00000111, // 7
	0b01111111, // 8
	0b01101111  // 9
};

void FND_Control::TestFND()
{
	for (int i = 0; i < 10; ++i) //0~9까지 테스트
	{
		displayNumber(i);
		_delay_ms(2000);
	}
}

// default constructor
FND_Control::FND_Control()
{
	// PC0, PC1, PC2, PC3, PC4, PC5, PC6를 출력으로 설정
	DDRC |= (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5) | (1 << PC6);
	anode_type = 1;
} //FND_Control

// default destructor
FND_Control::~FND_Control()
{
} //~FND_Control

void FND_Control::displayNumber(uint8_t number)
{
	// 범위를 벗어난 숫자가 들어오면 아무 것도 표시하지 않음
	if (number > 9) {
		PORTC = 0x00;
		return;
	}
	
	// 해당하는 7-segment 패턴을 출력 핀에 쓰기
	if (anode_type) // anode type인 경우
	{
		PORTC = ~patterns[number];
	}
	else
	{
		PORTC = patterns[number];
	}
}
