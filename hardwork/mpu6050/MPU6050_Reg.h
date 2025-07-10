#ifndef __MPU6050_Reg_H
#define __MPU6050_Reg_H

#define MPU6050_SMPLRT_DIV		0x19
#define MPU6050_CONFIG			0x1A
#define MPU6050_GYRO_CONFIG		0x1B
#define MPU6050_ACCEL_CONFIG	0x1C

/*以下是MPU6050的加速度方位的寄存器(测量结果是各轴加速度)*/
#define MPU6050_ACCEL_XOUT_H	0x3B
#define MPU6050_ACCEL_XOUT_L	0x3C
#define MPU6050_ACCEL_YOUT_H	0x3D
#define MPU6050_ACCEL_YOUT_L	0x3E
#define MPU6050_ACCEL_ZOUT_H	0x3F
#define MPU6050_ACCEL_ZOUT_L	0x40

/*以下是MPU6050的陀螺仪方位的寄存器(测量结果是各轴角速度)*/
#define MPU6050_GYRO_XOUT_H		0x43
#define MPU6050_GYRO_XOUT_L		0x44
#define MPU6050_GYRO_YOUT_H		0x45
#define MPU6050_GYRO_YOUT_L		0x46
#define MPU6050_GYRO_ZOUT_H		0x47
#define MPU6050_GYRO_ZOUT_L		0x48

/*以下是MPU6050的电源管理*/
#define MPU6050_PWR_MGMT_1		0x6B
#define MPU6050_PWR_MGMT_2		0x6C

/*以下是MPU6050的ID(序列号)*/
#define MPU6050_WHO_AM_I		0x75

#endif