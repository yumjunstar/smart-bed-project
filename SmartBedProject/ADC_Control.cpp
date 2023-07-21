/* 
* ADC_Control.cpp
*
* Created: 2023-06-08 오전 12:29:06
* Author: AOJUCBJD
*/


#include "ADC_Control.h"

// default constructor
ADC_Control::ADC_Control()
{
	this->channel = 0x00;
	ADMUX = ADC_Control::channel;
	ADCSRA = 0b10000111;

} //ADC_Control

double ADC_Control::ADC_V_get(void)
{
	ADMUX = this->channel;
	ADCSRA |= (1<<ADSC); //ADC START CONVERTION => ADSC
	while (!(ADCSRA & (1<<ADIF))); //ADC Flag Check ADC Interrupt flag가 1이 되는지 확인하는 것
	
	int adc = ADC;
	double Vadc_ = adc*ADC_Control::Vadc/1023.; //adc : Vadc = 1024 : 5[V]
	
	return Vadc_;
}
int ADC_Control::getChannel()
{
	return this->channel;
}


void ADC_Control::setChannel(int channel)
{
	this->channel = channel;
}
double ADC_Control::cal_cds(double Vout)
{

   // GL5539 CDS cell 의 gamma R100 기준 10~15K Ohm -> 12.5 K Ohm으로 평균
   const double Gamma = 0.5051499783;
   const double R9 = 4700;
   const double AVCC = ADC_Control::Vadc;
   
   double Rcds = (R9 * AVCC)/Vout - R9;
   
   double Lux = pow(10, 1-(log10(Rcds)-log10(40000))/Gamma);
   
   return Lux;

}
double ADC_Control::cal_LM35(double Vout)
{
	// Datasheet에 10mV당 1도 올라간다고 써져있다.
	// 현재 보드는 4배 증폭을 하는 중 이므로
	// 40mv당 1도가 증가한다.
	// 즉 0.04V당 1도
	//int R12 = 30;
	//int R13 = 10;
	//int gain = (R12+R13)/R13;
	//double VpC_Default = 0.01;
	//double VpC = VpC_Default*gain; // 0.04
	double VpC = 0.04;

	double T = Vout / VpC;
	return T;
}
double ADC_Control::cal_thermistor(double Vout)
{
	//서미스터 계산
	double T0 = 25+ 273.15;
	double R0 = 1000;
	double beta = 3650;

	
	double Rth = (Vadc/Vout)*4700 - 4700;
	double T = 1/((1/T0  + (1/beta) * log (Rth/R0))); //[K]
	T = T - 273.15;
	return T;
}

double ADC_Control::cal_discomfort_index(double temperature, double humidity)
{
	double result = 1.8*temperature-0.55*(1-humidity/100)*(1.8*temperature-26)+32;
	return result;
}

double ADC_Control::auto_get_sensor_value(void)
{
	double Vout = this->ADC_V_get();
	switch (this->channel)
	{
		case ADC_Channel_VALUE::POTENTIONMETER:
			return Vout;
			break;
		case ADC_Channel_VALUE::CDS:
			return cal_cds(Vout);
			break;
		case ADC_Channel_VALUE::LM35:
			return cal_LM35(Vout);
			break;
		case ADC_Channel_VALUE::THERMISTOR:
			return cal_thermistor(Vout);
			break;
		default:
		 assert(false); //없는 센서 값으로 channel 이 설정 되어있는 경우
		 break;
	}
}
// default destructor
ADC_Control::~ADC_Control()
{
} //~ADC_Control
