#ifndef __MPU6050_H
#define __MPU6050_H

void MPU6050_WriteReg(uint16_t RegAddress, uint8_t Data);
uint8_t MPU6050_ReadReg(uint16_t RegAddress);
void MPU6050_Init(void);
void MPU6050_GetData(int16_t *AccX,int16_t *AccY,int16_t *AccZ,int16_t *GIROX,int16_t *GIROY,int16_t *GIROZ);


#endif
