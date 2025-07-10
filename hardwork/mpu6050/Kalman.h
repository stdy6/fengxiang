#ifndef KALMAN_H
#define KALMAN_H
#include <math.h>

void Kalman_Init(void);
void Kalman_Calculate(void);
extern float pitch,roll;

typedef struct {
    float Q_angle;
    float Q_bias; 
    float R_measure;
    float angle;
    float bias; 
    float rate; 

    float P[2][2];
} KalmanFilter;

extern int16_t AccX, AccY, AccZ, GYROX, GYROY, GYROZ;
extern float gyroXrate,gyroYrate,accPitch,accRoll;

void Kalman_Init(void);
void Kalman_Calculate(void);
void KalmanFilter_Init(KalmanFilter *kf);
float KalmanFilter_Update(KalmanFilter *kf, float newAngle, float newRate, float dt);

#endif 
