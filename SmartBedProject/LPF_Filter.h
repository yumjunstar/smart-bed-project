/* 
* LPF_Filter.h
*
* Created: 2023-06-24 오전 8:14:39
* Author: AOJUCBJD
*/


#ifndef __LPF_FILTER_H__
#define __LPF_FILTER_H__

#define PI 3.141592653589793238462643383
class LPF_Filter
{
//variables
public:
protected:
private:
	double Fcut; // cut off frequency
	double LPF_prev_value; // previous value
	double deltaT; // delta time
//functions
public:
	LPF_Filter(double Fcutoff = 0.2, double DeltaT = 0.5);
	~LPF_Filter();
	double digitalLPF(double value);
protected:
private:

}; //LPF_Filter

#endif //__LPF_FILTER_H__
