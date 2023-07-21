/* 
* MAF_Filter.h
*
* Created: 2023-06-23 오전 6:07:49
* Author: willy
*/


#ifndef __MAF_FILTER_H__
#define __MAF_FILTER_H__

#define MAF_SIZE 10
class MAF_Filter
{
//variables
public:
protected:
private:
	volatile int Weight_avg_count_flag;
	volatile double Weight_data_list[MAF_SIZE];
	volatile int Weight_MAF_count;
	

//functions
public:
	MAF_Filter();
	~MAF_Filter();
	double Weight_MAF(double Weight_data);
protected:
private:
	MAF_Filter( const MAF_Filter &c );
	MAF_Filter& operator=( const MAF_Filter &c );

}; //MAF_Filter

#endif //__MAF_Filter_H__
