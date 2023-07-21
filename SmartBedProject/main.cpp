/*
 * SmartBedProject.cpp
 *
 * Created: 2023-06-07 오후 11:59:24
 * Author : AOJUCBJD
 */ 


// I2C LCD와 Kalman_Filter 는 구현은 완료했다.
// 다만 칼만 필터는 아직 적용을 하지 못했고, I2C_LCD는 LCD 문제가 있어 작동하지 않아 프로젝트에 적용하지는 못했다.
// 추후에 문제가 해결됐으면 좋겠다.

#define F_CPU 16000000UL
#define velocity 340 // 공기 중에서 소리의 속도
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "ADC_Control.h"
#include "USART_Control.h"
#include "Gyro.h"
#include "DHT11.h"
#include "I2C_LCD.h"
#include "FND_Control.h"

#include "LPF_Filter.h"
#include "MAF_Filter.h"
#include "Kalman_Filter.cpp"
unsigned char ndata;

/*
기능 센서값 USART 전송
사용 센서


온도 Thermistor 또는 LM35 (증폭기 회로 구성하기) 내부 온도 측정
초음파 센서 (화장대와 거리)
가속도 센서 MPU 6050
//자이로 센서 MPU 6050 -> 고민중
조도 센서 CDS GR5539

습도 센서 (센서는 미정, 내부 습도 측정)
소음 센서
포텐션미터 (온/습도 조절)

이중 6개


이외
서보모터
LED
*/
void servo_rotate(uint8_t angle);
void PWM_init();
void sonic_init();
void external_switch_init();
void fadeLED();
void artificial_sun_init();


// 동작
void ActionMorning(int lux, int stdlux);
void ActionTempChange(double current_temp, double std_temp, double offset_temp);
void ActionStandAtMirror(int distance_mm, int turn_on_led_distance_mm);
volatile unsigned int start_time=0;
volatile unsigned int end_time=0;
volatile unsigned int range=0;
volatile int count5us=0;
volatile int dif_time=0;

volatile int alarm = 0;
volatile int fadeDirection = 0;
volatile int fadeProcessing = 0; // LED 가 켜지고 꺼지는 동작 중에는 LED Direction이 바뀌지 않도록	막는 역할
volatile int i = 0;
USART_Control USART; //USART 객체 생성




int main(void)
{

	sonic_init();
	DDRD |= 0x04;// PD2 부저 출력 활성화
	DDRA |= 0b00001111; // 0번 파란색 LED, 1번 빨간색 LED, 2번 초록색 LED, 3번 화장대 LED

	external_switch_init();
	PWM_init();//서보모터 Timer counter 2번
	artificial_sun_init();

	Gyro gy;
	ADC_Control pot, cds, lm35, ther;
	
	//pot.setChannel(ADC_Channel_VALUE::POTENTIONMETER);
	cds.setChannel(ADC_Channel_VALUE::CDS);
	lm35.setChannel(ADC_Channel_VALUE::LM35);
	ther.setChannel(ADC_Channel_VALUE::THERMISTOR);


	FND_Control fnd;
	DHT11 dht; // 습도 센서 객체 생성
	
	
	sei();

	int8_t dht11_result;
	
	
	double std_temperature = lm35.auto_get_sensor_value(); // 기준 온도
	int offset_temperature = 1; // LED 변화 최소 온도 차이
	int turn_on_led_distance_mm = 60; // 6cm
	int day_std_lux = 100;
	
	
	
	// LM35와 DHT11
	double temperature = 0;
	uint8_t humidity = 0; 
	
	// CDS
	double lux = 0;
	
	// 초음파
	int distance_mm = 0;
	int prev_distance_mm = 0;
	
	// MPU6050
	double a_x = 0, a_y = 0, a_z = 0;
	double g_x = 0, g_y = 0, g_z = 0;
	
    /* Replace with your application code */
	
	//fnd.TestFND();
	
	
	LPF_Filter lpf_ultrasonic_sensor, lpf_cds;
	LPF_Filter lpf_a_x(10.0), lpf_a_y(10.0), lpf_a_z(10.0), lpf_g_x(10.0), lpf_g_y(10.0), lpf_g_z(10.0);
	MAF_Filter maf_lm35, maf_dht11_hum;
	KalmanFilter kf;
    while (1) 
    {
		// LED 변화


		
		
		temperature = lm35.auto_get_sensor_value();
		lux = cds.auto_get_sensor_value();
		gy.auto_get_sensor_value(&a_x, &a_y, &a_z, &g_x, &g_y, &g_z); // gyro data
		dht11_result = dht.readDHT11(&humidity); // dht11 data
		
		
		distance_mm = round(((dif_time)*(8./F_CPU)*velocity/2.)*1000.);
		if (distance_mm < 0) 
		// timer 예외가 발생 할 때가 있음 해당 문제에 대한 예외 처리
		{
			distance_mm = prev_distance_mm;
		}
		else
		{
			prev_distance_mm = distance_mm;
		}

		
		double discomfort_index = ADC_Control::cal_discomfort_index(temperature, humidity); // 불쾌 지수 계산
		
		// 필터 통과
		// mafp = Moving Average Filter Passed
		
		double mafp_temperature = maf_lm35.Weight_MAF(temperature);
		double lpfp_lux = lpf_cds.digitalLPF(lux);
		double mafp_humidity = maf_dht11_hum.Weight_MAF(humidity);
		
		double lpfp_distance_mm = lpf_ultrasonic_sensor.digitalLPF(distance_mm);
		
		double lpfp_a_x = lpf_a_x.digitalLPF(a_x);
		double lpfp_a_y = lpf_a_x.digitalLPF(a_y);
		double lpfp_a_z = lpf_a_x.digitalLPF(a_z);
		
		double lpfp_g_x = lpf_a_x.digitalLPF(g_x);
		double lpfp_g_y = lpf_a_x.digitalLPF(g_y);
		double lpfp_g_z = lpf_a_x.digitalLPF(g_z);
		// 동작
		fadeLED();
		fnd.displayNumber((int)(discomfort_index)/10);
		ActionMorning(lpfp_lux, day_std_lux); // 동작 2개
		ActionTempChange(mafp_temperature, std_temperature, offset_temperature);
		ActionStandAtMirror(lpfp_distance_mm, turn_on_led_distance_mm);
		USART.USART0_TX_array_funct(distance_mm);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(lux);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(temperature);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(humidity);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(a_x);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(a_y);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(a_z);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(g_x);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(g_y);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(g_z);
		//USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct("  Filter passed  ");
		USART.USART0_TX_array_funct(lpfp_distance_mm);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(lpfp_lux);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(mafp_temperature);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(mafp_humidity);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(lpfp_a_x);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(lpfp_a_y);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(lpfp_a_z);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(lpfp_g_x);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(lpfp_g_y);
		USART.USART0_TX_array_funct(",");
		USART.USART0_TX_array_funct(lpfp_g_z);
		USART.USART0_TX_endl();
		

		
		/*
		USART.USART0_TX_array_funct("Fade Direction : ");
		USART.USART0_TX_array_funct(fadeDirection);
		
		USART.USART0_TX_array_funct(", Discomfort index : ");
		USART.USART0_TX_array_funct(discomfort_index);
		
		
		USART.USART0_TX_array_funct("Dist : ");
		USART.USART0_TX_array_funct(distance_mm);
		USART.USART0_TX_array_funct(" LPF Dist: ");
		USART.USART0_TX_array_funct(lpfp_distance_mm);
		
		
		USART.USART0_TX_array_funct(", Lux : ");
		USART.USART0_TX_array_funct(lux);
		USART.USART0_TX_array_funct(" LPF Lux : ");
		USART.USART0_TX_array_funct(lpfp_lux);
		
		USART.USART0_TX_array_funct(", Temp : ");
		USART.USART0_TX_array_funct(temperature);
		USART.USART0_TX_array_funct(" MAF Temp : ");
		USART.USART0_TX_array_funct(mafp_temperature);
		
		
		USART.USART0_TX_array_funct(", Gyro : ");
		USART.USART0_TX_array_funct(", a_x:");
		USART.USART0_TX_array_funct(a_x);
		USART.USART0_TX_array_funct(", a_y:");
		USART.USART0_TX_array_funct(a_y);
		USART.USART0_TX_array_funct(", a_z:");
		USART.USART0_TX_array_funct(a_z);
		USART.USART0_TX_array_funct(", g_x:");
		USART.USART0_TX_array_funct(g_x);
		USART.USART0_TX_array_funct(", g_y:");
		USART.USART0_TX_array_funct(g_y);
		USART.USART0_TX_array_funct(", g_z:");
		USART.USART0_TX_array_funct(g_z);
		USART.USART0_TX_array_funct(" ");
		
		
		USART.USART0_TX_array_funct(", Humidity : ");
		if (dht11_result == 0)
		{

			USART.USART0_TX_array_funct(humidity);
			USART.USART0_TX_array_funct(" MAF Humidity : ");
			USART.USART0_TX_array_funct(mafp_humidity);
		}
		else {
			// 읽기 실패
			USART.USART0_TX_array_funct("ERROR");
		}
		USART.USART0_TX_endl();
		//_delay_ms(500);
		*/
    }
}
void ActionStandAtMirror(int distance_mm, int turn_on_led_distance_mm)
{
	if (distance_mm <= turn_on_led_distance_mm)
	{
		PORTA |= 0x08; // 가까이 다가오면 LED 키기
	}
	else
	{
		PORTA &= ~0x08; // 멀리가면 LED 끄기
	}
}
void ActionTempChange(double current_temp, double std_temp, double offset_temp)
{
	double dif_temperature = current_temp - std_temp; // 현재 온도와 원래 온도 차이
	if (dif_temperature > offset_temp) // 기준 온도 보다 높다면
	{
		//USART.USART0_TX_array_funct("hot");
			
		PORTA |= 0x02; // 빨간색 LED 점등
		PORTA &= ~0x01; // 파란색 LED
		PORTA &= ~0x04; // 초록색 LED
		// 빨간색 1번만 키고 나머지 LED는 끔 그 외에 포트는 건들지 않음
	}
	else if (abs(dif_temperature) <= offset_temp) // 온도가 현재 온도랑 비슷하다.
	{
		// 반드시 등호를 추가 해야 한다. 추가 하지 않으면 dif_temperature가 offset 일때 파란색 led가 켜지게 된다.
		// 이는 기준 온도가 더 높을때도 파란색 led로 점등 될 수 있어서 주의해야 한다.
		PORTA |= 0x04; // 초록색 LED
		PORTA &= ~0x01; // 파란색 LED
		PORTA &= ~0x02; // 빨간색 LED
		// 초록색 2번만 키고 나머지 LED는 끔 그 외에 포트는 건들지 않음
		//USART.USART0_TX_array_funct("std");
	}
	else // 기준 온도 보다 더 낮다.
	{
		PORTA |= 0x01; // 파란색 LED
		PORTA &= ~0x02; // 빨간색 LED
		PORTA &= ~0x04; // 초록색 LED
			
		//USART.USART0_TX_array_funct("cool");
		// 파란색만 키고 나머지 LED는 끔 그 외에 포트는 건들지 않음
	}
}
void ActionMorning(int lux, int day_std_lux) // 문열기와 부저 울리기
{
	if (lux > day_std_lux)
	{
		servo_rotate(100);
		alarm = 1;
	}
	else
	{
		servo_rotate(0);
		alarm = 0;
	}
}
void artificial_sun_init()
{

	// PE3 (OC3A) 핀을 출력으로 설정
	DDRE |= (1 << PE3);

	// Fast PWM 모드 설정
	TCCR3A |= (1 << WGM31) | (1 << WGM30);
	TCCR3B |= (1 << WGM32);

	// 비반전 모드
	TCCR3A |= (1 << COM3A1);

	// 프리스케일러 64로 설정
	TCCR3B |= (1 << CS31) | (1 << CS30);
}
void external_switch_init()
{
	// 외부 인터럽트 사용 스위치
	EIMSK = (1<<INT3);
	EICRA = (1<<ISC31) | (0<<ISC30); // fallind edge시
}
void sonic_init()
{
	TCNT1 = 0;//65526;
	TCCR1B |= 0xC2; //6: 1 상승 엣지 [2:0]:010 Prescaler 8,노이즈 제거
	DDRB |= 0x40;// PB6 trigger핀 출력 활성화, PB5 LED 활성화
	TIMSK |= 0x25;
	//bit5: timer counter 1 input caputre,
	//bit2:overflow interrupt 1
	//bit0:overflow interrupt 0
	//TIMER0
	TCNT0 |= 246;//TCNT0 246 시작값
	TCCR0 = 0x02;//Prescaler 8
}
void fadeLED() { // 꺼졌다 켜졌다 하게 하는 함수
	volatile static int i = 0;
	fadeProcessing = 1;
	if (fadeDirection) {
		// 밝기를 천천히 늘림
		for (i; i < 1024; ++i) {
			//USART.USART0_TX_array_funct(i);
			_delay_ms(1);
			//USART.USART0_TX_array_funct(" ");
			OCR3A = i;
		}
	} else {
		// 밝기를 천천히 줄임
		for (i; i >= 0; --i) {
			//USART.USART0_TX_array_funct(i);
			//USART.USART0_TX_array_funct(" ");
			_delay_ms(1);
			OCR3A = i;
		}
	}
	fadeProcessing = 0;
}
ISR(INT3_vect) // 스위치가 눌렸을때 서서히 꺼지고 서서히 켜지기
{
	if (!fadeProcessing) // fade가 진행중이 아니라면
	{
		fadeDirection = !fadeDirection;
	}
}

ISR(USART0_RX_vect) //For USART
{
	ndata = UDR0;
}
ISR (TIMER0_OVF_vect){//5us간격
	//volatile static int count5us=0;

	if (alarm && (count5us % 50  == 0))
	{
		PORTD ^= (1<<2); //PD2
	}
	count5us++;
	if (count5us<=12000){//60ms 간격

		if (count5us<=2){//10us 동안
			//PB6
			PORTB|=0x40;//on
			
		}
		else{
			PORTB&= ~0x40;//off 전송 완료
		}
	}
	else{
		count5us=0;
	}

	TCNT0 = 246;
}
ISR (TIMER1_OVF_vect){//60ms간격으로 초음파 센서의 echo를 받는데 사용
	TCNT1 = 0;//65526;
}
ISR (TIMER1_CAPT_vect){
	if (TCCR1B&(1<<6)){//6번째 비트가 1이라면(상승 엣지라면)
		start_time=ICR1;
		TCCR1B&=~(1<<6);//6번째 비트 0으로 하강 엣지
		}else{//하강 엣지라면
		end_time=ICR1;
		dif_time=end_time-start_time;

		TCCR1B|=1<<6;//6번째 비트 1으로 다시 상승 엣지
	}
}

// Function to initialize PWM on Timer2
void PWM_init() {
	// Set fast PWM mode with non-inverted output
	TCCR2 |= (1 << WGM21) | (1 << WGM20) | (1 << COM21);
	
	// Set prescaler to 64
	TCCR2 |= (1 << CS22);
	
	// Set the OCR2 register for desired PWM duty cycle
	OCR2 = 127;  // 50% duty cycle (Center position)
	
	// Set OC2 pin (PB7) as output
	DDRB |= (1 << 7);
}

// Function to control the servo position
void servo_rotate(uint8_t angle) {
	//총 10ms 
	// 이중 0도이면 1ms 즉 듀티비 10% 즉 25.5,
	// 180도 이면 2ms 즉 듀티비 20% 즉 51
	// 5~ 10% duty 비
	// 0 일때 5% 180도 일때 10%
	// 30% 가 0이고
	// 50% 180
	OCR2 = ((angle * 51) / 90) + 77;  // Map the angle to PWM duty cycle (77-179)
}