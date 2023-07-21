#include "DHT11.h"


// DHT11 데이터 읽기 함수
int8_t DHT11::readDHT11(uint8_t* temperature, uint8_t* humidity) {

	//다른 인터럽트가 타이밍에 방해를 해서 dht11을 읽는 동안에는 잠시 인터럽트를 종료
	uint8_t bits[5] = {0};
	uint8_t cnt = 7;
	uint8_t idx = 0;

	// 시작 신호 전송
	DDRF |= (1 << DHT11_PIN);  // 출력 모드로 설정
	PORTF &= ~(1 << DHT11_PIN);  // 0으로 설정

	_delay_ms(20);  // 최소 18ms 이상 대기
	PORTF |= (1 << DHT11_PIN);  // 1로 설정
	_delay_us(1);
	DDRF &= ~(1 << DHT11_PIN);  // 입력 모드로 변경
	_delay_us(39);
	cli();
	// 센서로부터 응답 확인
	if ((PINF & (1 << DHT11_PIN))) {
		return -1;  // 응답 없음
	}
	_delay_us(80);
	if (!(PINF & (1 << DHT11_PIN))) {
		return -1;  // 응답 없음
	}
	_delay_us(80);
	// 데이터 수신
	for (uint8_t i = 0; i < 5; i++) {
		for (uint8_t j = 0; j < 8; j++) {
			while (!(PINF & (1 << DHT11_PIN)));  // 비트 시작을 기다림
			_delay_us(40);
			if (PINF & (1 << DHT11_PIN)) {
				bits[i] |= (1 << cnt);  // 1이면 비트에 값을 설정
			}
			while (PINF & (1 << DHT11_PIN));  // 비트의 끝을 기다림
			cnt--;
		}
		cnt = 7;
	}
	sei();
	// 체크섬 확인
	if (bits[4] == ((bits[0] + bits[1] + bits[2] + bits[3]) & 0xFF)) {
		*humidity = bits[0];
		*temperature = bits[2];
		return 0;  // 성공
		} else {
		return -1;  // 체크섬 오류
	}
}

int8_t DHT11::readDHT11(uint8_t* humidity)
{
	uint8_t temp;
	return readDHT11(&temp, humidity);
}

DHT11::DHT11()
 {
	DDRF |= (1 << DHT11_PIN);  // 출력으로 설정
	PORTF |= (1 << DHT11_PIN);  // 기본 상태로 설정
 }