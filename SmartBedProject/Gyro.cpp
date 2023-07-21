/* 
* Gyro.cpp
*
* Created: 2023-06-21 오후 5:20:29
* Author: AOJUCBJD
*/


#include "Gyro.h"

// default constructor
Gyro::Gyro()
{
	this->dt = 0.000;
	temp = 0;
	a_x_l = a_x_h = a_y_l = a_y_h = a_z_l = a_z_h = 0;
	g_x_l = g_x_h = g_y_l = g_y_h = g_z_l = g_z_h = 0;
	bas_a_x = bas_a_y = bas_a_z = 0.0;
	bas_g_x = bas_g_y = bas_g_z = 0.0;
	a_x = a_y = a_z = 0.0;
	g_x = g_y = g_z = 0.0;
	las_angle_gx = las_angle_gy = las_angle_gz = 0.0;
	angle_ax = angle_ay = angle_az = 0.0;
	angle_gx = angle_gy = angle_gz = 0.0;
	roll = pitch = yaw = 0.0;
	alpha = 0.0;
	USART.USART0_TX_array_funct("Turn off Gyro Sleep state");
	USART.USART0_TX_endl();
	
	//MPU6050 init
	TWI_Instance.twi_write(0x6B, 0x00); 	//sleep 끔
	USART.USART0_TX_array_funct("DLPF 10Hz");
	USART.USART0_TX_endl();
	_delay_ms(1);
	//TWI_Instance.twi_write(0x1A, 0x05); 	//DLPF 10Hz digital low pass filter
	USART.USART0_TX_array_funct("Setting success calibration start");
	USART.USART0_TX_endl();
	this->calibrate();
	USART.USART0_TX_array_funct("Init calibration success. Ready to use");
	
	sei();
} //Gyro

double Gyro::get_a_x()
{

	a_x = (a_x_h<<8) | a_x_l;
	double calibrated = a_x - bas_a_x;
	a_x = (calibrated/AFS_SEL)*9.81; // m/s^2 단위로 변경
	return a_x;
}

double Gyro::get_a_y()
{
	a_y = (a_y_h<<8) | a_y_l;
	double calibrated = a_y - bas_a_y;
	a_y = (calibrated/AFS_SEL)*9.81; // m/s^2 단위로 변경
	return a_y;
}

double Gyro::get_a_z()
{
	a_z = (a_z_h<<8) | a_z_l;
	double calibrated = a_z - bas_a_z;
	a_z = (calibrated/AFS_SEL)*9.81; // m/s^2 단위로 변경
	return a_z;
}

double Gyro::get_g_x()
{
	g_x = (g_x_h<<8) | g_x_l;
	g_x = (g_x - bas_g_x)/FS_SEL;
	return deg2rad(g_x);
}

double Gyro::get_g_y()
{
	g_y = (g_y_h<<8) | g_y_l;
	g_y = (g_y - bas_g_y)/FS_SEL;
	return deg2rad(g_y);
}

double Gyro::get_g_z()
{
	g_z = (g_z_h<<8) | g_z_l;
	g_z = (g_z - bas_g_z)/FS_SEL;
	return deg2rad(g_z);
}

void Gyro::auto_get_sensor_value(double* a_x, double* a_y, double* a_z, double* g_x, double* g_y, double* g_z)
{
	this->get_raw_data();
	*a_x = get_a_x();
	*a_y = get_a_y();
	*a_z = get_a_z();
	
	*g_x = get_g_x();
	*g_y = get_g_y();
	*g_z = get_g_z();
}

void Gyro::calibrate()	//초기값 읽기
{
	int cal = 10;

	for(int i=0; i<cal; i++)	//평균
	{
		get_raw_data();
		
		temp = (a_x_h<<8) | a_x_l;
		a_x += temp;
		temp = (a_y_h<<8) | a_y_l;
		a_y += temp;
		temp = (a_z_h<<8) | a_z_l;
		a_z += temp;
		temp = (g_x_h<<8) | g_x_l;
		g_x += temp;
		temp = (g_y_h<<8) | g_y_l;
		g_y += temp;
		temp = (g_z_h<<8) | g_z_l;
		g_z += temp;

		_delay_ms(100);
	}
	
	a_x /= cal;
	a_y /= cal;
	a_z /= cal;
	g_x /= cal;
	g_y /= cal;
	g_z /= cal;

	bas_a_x = a_x;	//초기 값으로 저장
	bas_a_y = a_y;
	bas_a_z = a_z;
	bas_g_x = g_x;
	bas_g_y = g_y;
	bas_g_z = g_z;

}

double Gyro::deg2rad(double deg)
{
	double rad = 0;
	rad = deg * PI / 180.0;
	return rad;
}

void Gyro::get_raw_data()
{
	a_x_h = TWI_Instance.twi_read(0x3B);		//x축 가속도
	_delay_us(10);
	a_x_l = TWI_Instance.twi_read(0x3C);
	_delay_us(10);
	a_y_h = TWI_Instance.twi_read(0x3D);		//y축 가속도
	_delay_us(10);
	a_y_l = TWI_Instance.twi_read(0x3E);
	_delay_us(10);
	a_z_h = TWI_Instance.twi_read(0x3F);		//z축 가속도
	_delay_us(10);
	a_z_l = TWI_Instance.twi_read(0x40);
	_delay_us(10);
	g_x_h = TWI_Instance.twi_read(0x43);		//x축 각속도
	_delay_us(10);
	g_x_l = TWI_Instance.twi_read(0x44);
	_delay_us(10);
	g_y_h = TWI_Instance.twi_read(0x45);		//y축 각속도
	_delay_us(10);
	g_y_l = TWI_Instance.twi_read(0x46);
	_delay_us(10);
	g_z_h = TWI_Instance.twi_read(0x47);		//z축 각속도
	_delay_us(10);
	g_z_l = TWI_Instance.twi_read(0x48);
	_delay_us(10);
	
	
}

// default destructor
Gyro::~Gyro()
{
} //~Gyro
