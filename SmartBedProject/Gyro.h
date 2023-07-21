/* 
* Gyro.h
*
* Created: 2023-06-21 오후 5:20:30
* Author: AOJUCBJD
*/


#ifndef __GYRO_H__
#define __GYRO_H__
#define PI 3.141592653589793238462643383
// RAW 값의 최대 값을 나타냄

// 해당 값이 됐을때 1 deg/s
#define FS_SEL 131
// 해당 값이 됐을때 1g
#define AFS_SEL 16384
#include "TWI.h"
#include "USART_Control.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
class Gyro
{
//variables
public:
protected:
private:
	volatile double dt;
	volatile int temp;
	volatile unsigned char a_x_l,a_x_h,a_y_l,a_y_h,a_z_l,a_z_h;
	volatile unsigned char g_x_l,g_x_h,g_y_l,g_y_h,g_z_l,g_z_h;
	volatile double bas_a_x,bas_a_y,bas_a_z;
	volatile double bas_g_x,bas_g_y,bas_g_z;
	volatile double a_x,a_y,a_z;
	volatile double g_x,g_y,g_z;
	volatile double las_angle_gx,las_angle_gy,las_angle_gz;
	volatile double angle_ax,angle_ay,angle_az;
	volatile double angle_gx,angle_gy,angle_gz;
	volatile double roll,pitch,yaw;
	volatile double alpha;

	USART_Control USART;
	TWI TWI_Instance;
//functions
public:
	Gyro();
	void get_raw_data();
	double get_a_x();
	double get_a_y();
	double get_a_z();

	double get_g_x();
	double get_g_y();
	double get_g_z();
	
	void auto_get_sensor_value(double* a_x, double* a_y, double* a_z, double* g_x, double* g_y, double* g_z);
	~Gyro();
protected:
private:
	void calibrate();
	double deg2rad(double deg);
	Gyro( const Gyro &c );
	Gyro& operator=( const Gyro &c );

}; //Gyro

#endif //__GYRO_H__
