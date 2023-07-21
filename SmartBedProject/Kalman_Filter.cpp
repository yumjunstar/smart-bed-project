class KalmanFilter {
	private:
	float Q_angle; // 프로세스 잡음 분산
	float Q_bias; // 자이로 바이어스에 대한 프로세스 잡음 분산
	float R_measure; // 측정 잡음 분산
	float angle;  // 칼만 필터에 의해 계산된 각도
	float bias;  // 칼만 필터에 의해 계산된 자이로 바이어스
	float rate;  // 바이어스를 계산한 후의 보정된 각속도

	float P[2][2];  // 오차 공분산 행렬

	public:
	KalmanFilter() {
		/* 이렇게 변수들을 설정합니다, 이 값들은 사용자에 의해 조절될 수 있습니다 */
		Q_angle = 0.001;
		Q_bias = 0.003;
		R_measure = 0.03;

		angle = 0;  // 각도 초기화
		bias = 0;  // 바이어스 초기화

		P[0][0] = 0;  // 바이어스가 0이라고 가정하고, 시작 각도를 알고 있다면(각도 설정을 사용), 오차 공분산 행렬은 다음과 같이 설정됩니다
		P[0][1] = 0;
		P[1][0] = 0;
		P[1][1] = 0;
	}

	// 각도는 도 단위이어야 하며, 속도는 도/초 단위이어야 하고, 시간 간격은 초 단위이어야 합니다
	float getAngle(float newAngle, float newRate, float dt) {
		float S;
		float K[2];  // 칼만 이득 - 이는 2x1 벡터입니다
		float y;  // 각도 차이
		float P00_temp, P01_temp;

		// 이산 칼만 필터 시간 업데이트 방정식 - 시간 업데이트 ("예측")
		// xhat 업데이트 - 상태를 앞으로 추정
		rate = newRate - bias;       // 각속도 측정
		angle += dt * rate;

		// 오차 공분산 추정 업데이트 - 오차 공분산을 앞으로 추정
		P00_temp = P[0][0];
		P01_temp = P[0][1];
		P[0][0] += dt * (dt * P[1][1] - P01_temp - P[0][1] + Q_angle);
		P[0][1] -= dt * P[1][1];
		P[1][0] -= dt * P[1][1];
		P[1][1] += Q_bias * dt;

		// 이산 칼만 필터 측정 업데이트 방정식 - 측정 업데이트 ("보정")
		// 칼만 이득 계산 - 칼만 이득 계산
		S = P[0][0] + R_measure;  // 오차 추정
		K[0] = P[0][0] / S;
		K[1] = P[1][0] / S;

		// 각도와 바이어스 계산 - 측정 zk(newAngle)와 함께 추정치 업데이트
		y = newAngle - angle;  // 각도 차이
		angle += K[0] * y;
		bias += K[1] * y;

		// 오차 공분산 계산 - 오차 공분산 업데이트
		P[0][0] -= K[0] * P00_temp;
		P[0][1] -= K[0] * P01_temp;
		P[1][0] -= K[1] * P00_temp;
		P[1][1] -= K[1] * P01_temp;

		return angle;
		}

		// 각도 설정, 이는 시작 각도로 설정되어야 합니다
		void setAngle(float newAngle) { angle = newAngle; }
    
		// 측정된 부분과 칼만 필터에 의해 계산된 부분 간의 차이
		float getRate() { return rate; }
};

