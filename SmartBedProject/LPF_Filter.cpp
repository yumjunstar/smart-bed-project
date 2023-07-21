/* 
* LPF_Filter.cpp
*
* Created: 2023-06-24 오전 8:14:39
* Author: AOJUCBJD
*/


#include "LPF_Filter.h"

// default constructor
LPF_Filter::LPF_Filter(double Fcutoff, double DeltaT) : Fcut(Fcutoff), deltaT(DeltaT)
{
	//this->Fcut = Fcut;
	this->LPF_prev_value = -10000; // 값이 정해지지 않았을때 초기 값
	//this->deltaT = 0.5;
} //LPF_Filter

// default destructor
LPF_Filter::~LPF_Filter()
{
} //~LPF_Filter
double LPF_Filter::digitalLPF(double value)
{
	double tau = 1/(2*PI*Fcut);
	
	
	if (this->LPF_prev_value == -10000)
	{
		this->LPF_prev_value = value;
		return this->LPF_prev_value;
	}
	else
	{
		double filtered_value;
		filtered_value = (deltaT * value + tau * LPF_prev_value)/(tau + deltaT);
		LPF_prev_value = filtered_value;
		return filtered_value;
	}
}