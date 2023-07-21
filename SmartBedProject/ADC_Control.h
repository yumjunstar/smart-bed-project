/* 
* ADC_Control.h
*
* Created: 2023-06-08 오전 12:29:06
* Author: AOJUCBJD
*/


#ifndef __ADC_Control_H__
#define __ADC_Control_H__

#define F_CPU 16000000UL
#include <math.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <assert.h>

namespace ADC_Channel_VALUE{
	enum ADC_Channel {POTENTIONMETER, CDS, LM35, THERMISTOR, ADMP};
}

class ADC_Control
{
//variables
public:

protected:

private:
	const static int Vadc = 5;
	int channel;
//functions
public:
	ADC_Control();
	double ADC_V_get(void);
	
	void setChannel(int channel);
	int getChannel(void);
	
	static double cal_cds(double Vout);
	static double cal_LM35(double Vout);
	static double cal_thermistor(double Vout);
	static double cal_discomfort_index(double temperature, double humidity);
	double auto_get_sensor_value(void);
	~ADC_Control();
protected:
private:
	ADC_Control( const ADC_Control &c );
	ADC_Control& operator=( const ADC_Control &c );

}; //ADC_Control

#endif //__ADC_Control_H__
