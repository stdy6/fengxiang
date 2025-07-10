#include "debug.h"                 
#include "math.h"
#include"MPU6050_Reg.h"
#define MPU6050_Address 0xD0
#define SUCCESS 1

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	I2C_GenerateSTART(I2C1,ENABLE);											//I2C 开始的起始条件
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) !=SUCCESS);		//判断EV5是否完成
	
	I2C_Send7bitAddress(I2C1,MPU6050_Address,I2C_Direction_Transmitter);			//指明从机地址
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);//判断EV6是否完成
	
	I2C_SendData(I2C1,RegAddress);												//指明向DR寄存器发送数据的对方寄存器的地址
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING) != SUCCESS);//判断EV8-2是否完成
	
	I2C_SendData(I2C1,Data);												//向位于DR寄存器里的对方寄存器发送数据
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);//判断EV8-2是否完成

	I2C_GenerateSTOP(I2C1,ENABLE);							//生成终止信号
}

/*函数功能：从MPU6050的寄存器中读出数据(通常时读出其测量数据)*/
uint8_t MPU6050_ReadReg(uint16_t RegAddress)
{
	uint8_t Data;
	
	I2C_GenerateSTART(I2C1,ENABLE);											//I2C 开始的起始条件
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) !=SUCCESS);		//判断EV5是否完成
	
	I2C_Send7bitAddress(I2C1,MPU6050_Address,I2C_Direction_Transmitter);			//指明从机地址
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);
	
	I2C_SendData(I2C1,RegAddress);										//指明DR寄存器里存储想要的数据的那个寄存器的地址										
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);

	I2C_GenerateSTART(I2C1,ENABLE);		//I2C 开始的起始条件
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	
	I2C_Send7bitAddress(I2C1,MPU6050_Address,I2C_Direction_Receiver);			//指明从机地址
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS);//判断EV6是否完成(但注意这里的主机对应的时Receiver而不是Transmitter！)

	I2C_AcknowledgeConfig(I2C1,DISABLE);	//需要提前停止响应并设置停止标志，不然的话会多收到一个字节！(这是固定的规则！)
	I2C_GenerateSTOP(I2C1,ENABLE);
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
	
	Data=I2C_ReceiveData(I2C1);			//用局部变量Data接住I2C线上读到的数据
	
	I2C_AcknowledgeConfig(I2C1,ENABLE); //最后应答要恢复默认值
	return Data;
}

void MPU6050_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6| GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	I2C_InitTypeDef I2C_InitStruct;
	I2C_InitStruct.I2C_Ack=I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed=50000;
	I2C_InitStruct.I2C_DutyCycle=I2C_DutyCycle_2;
	I2C_InitStruct.I2C_Mode=I2C_Mode_I2C;
	I2C_InitStruct.I2C_OwnAddress1=0x00;
	I2C_Init(I2C1,&I2C_InitStruct);
	
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01);
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x09);
	MPU6050_WriteReg(MPU6050_CONFIG,0x06);
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG,0x18);
}

/*这个函数用指针把数值传递给地址，再读取地址从而实现函数的多返回值*/

void MPU6050_GetData(int16_t *AccX,int16_t *AccY,int16_t *AccZ,int16_t *GYROX,int16_t *GYROY,int16_t *GYROZ)
{
	uint16_t DataH,DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL;

	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL;

	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GYROX = (DataH << 8) | DataL;

	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GYROY = (DataH << 8) | DataL;

	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GYROZ = (DataH << 8) | DataL;

}
