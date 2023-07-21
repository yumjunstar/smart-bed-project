/*
* MAF_Filter.cpp
*
* Created: 2023-06-23 오전 6:07:49
* Author: AOJUCBJD
*/


#include "MAF_Filter.h"

// default constructor
MAF_Filter::MAF_Filter()
{
	for (int i = 0; i < MAF_SIZE; ++i)
	{
		Weight_data_list[i] = 0.0;
	}
	Weight_avg_count_flag = 1;
	Weight_MAF_count = 0;
} //MAF_Filter

// default destructor
MAF_Filter::~MAF_Filter()
{
} //~MAF_Filter
// 무게센서 MAF 필터
double MAF_Filter::Weight_MAF(double Weight_data){
	// 평균필터 값 집어넣기
	Weight_data_list[Weight_MAF_count++] = Weight_data;
	if (Weight_MAF_count >= MAF_SIZE)
	{
		Weight_MAF_count = 0;
		Weight_avg_count_flag = 0;
	}
	// 평균필터 합 구하기
	double MAF_Sum = 0;
	for(int i = 0; i < MAF_SIZE; i++){
		MAF_Sum += Weight_data_list[i];
	}
	if(Weight_avg_count_flag)
	{
		return MAF_Sum / Weight_MAF_count;
	}
	return MAF_Sum / MAF_SIZE;
}

