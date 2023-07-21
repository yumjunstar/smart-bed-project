/* 
* FND_Control.h
*
* Created: 2023-06-24 오전 1:04:46
* Author: AOJUCBJD
*/


#ifndef __FND_CONTROL_H__
#define __FND_CONTROL_H__
#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
class FND_Control
{
//variables
public:
protected:
private:
	// 숫자에 해당하는 7-segment 패턴을 설정 (0-9)
	static const uint8_t patterns[10];
	int anode_type;
//functions
public:
	void displayNumber(uint8_t number);
	
	// anode_type 이 맞으면 1 아니면 0
	void setFNDType(int anode_type);
	void TestFND();
	FND_Control();
	~FND_Control();
protected:
private:
	FND_Control( const FND_Control &c );
	FND_Control& operator=( const FND_Control &c );

}; //FND_Control

#endif //__FND_CONTROL_H__
