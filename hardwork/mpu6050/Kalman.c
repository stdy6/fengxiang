#include "debug.h"
#include "MPU6050.h"
#include <math.h>
#include "Kalman.h"

KalmanFilter kf_pitch; 														// ·­¹ö½Ç
KalmanFilter kf_roll;													    // ¸©Ñö½Ç
int16_t AccX, AccY, AccZ, GYROX, GYROY, GYROZ;
float gyroXrate,gyroYrate,accPitch,accRoll;
float pitch,roll;

void KalmanFilter_Init(KalmanFilter *kf) {
    kf->Q_angle = 0.001f;
    kf->Q_bias = 0.003f;
    kf->R_measure = 0.03f;

    kf->angle = 0.0f;
    kf->bias = 0.0f; 

    kf->P[0][0] = 0.0f;
    kf->P[0][1] = 0.0f;
    kf->P[1][0] = 0.0f;
    kf->P[1][1] = 0.0f;
}

float KalmanFilter_Update(KalmanFilter *kf, float newAngle, float newRate, float dt) 
{
    kf->rate = newRate - kf->bias;
    kf->angle += dt * kf->rate;

    kf->P[0][0] += dt * (dt*kf->P[1][1] - kf->P[0][1] - kf->P[1][0] + kf->Q_angle);
    kf->P[0][1] -= dt * kf->P[1][1];
    kf->P[1][0] -= dt * kf->P[1][1];
    kf->P[1][1] += kf->Q_bias * dt;

    float S = kf->P[0][0] + kf->R_measure;
    float K[2];
    K[0] = kf->P[0][0] / S;
    K[1] = kf->P[1][0] / S;

    float y = newAngle - kf->angle;
    kf->angle += K[0] * y;
    kf->bias += K[1] * y;

    float P00_temp = kf->P[0][0];
    float P01_temp = kf->P[0][1];

    kf->P[0][0] -= K[0] * P00_temp;
    kf->P[0][1] -= K[0] * P01_temp;
    kf->P[1][0] -= K[1] * P00_temp;
    kf->P[1][1] -= K[1] * P01_temp;

    return kf->angle;
}

void Kalman_Init()
{
	KalmanFilter_Init(&kf_pitch);
    KalmanFilter_Init(&kf_roll);
}

void Kalman_Calculate()
{
		MPU6050_GetData(&AccX, &AccY, &AccZ, &GYROX, &GYROY, &GYROZ);
		gyroXrate = GYROX / 131.0;
		gyroYrate = GYROY / 131.0;
		accPitch = atan2f(AccY, AccZ) * 180 / 3.14159265358979323846;
		accRoll = atan2f(AccX, AccZ) * 180 / 3.14159265358979323846;
		pitch = KalmanFilter_Update(&kf_pitch, accPitch, gyroYrate, 0.01);
		roll = KalmanFilter_Update(&kf_roll, accRoll, gyroXrate, 0.01);
}
