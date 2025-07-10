#ifndef __I2C_SOFT_H
#define __I2C_SOFT_H

#include "debug.h"

#define I2C_SOFT_GLOBAL_DELAY() Delay_Us(1)

typedef enum
{
	ACK	 = Bit_RESET,
	NACK = Bit_SET,
}ack_t;

typedef struct
{
	void (*Init)(void);
	void (*Start)(void);
	void (*Stop)(void);
	ack_t (*SendData)(uint8_t);
	uint8_t (*ReceiveData) (ack_t);
}I2C_soft_t;

#define I2C_PORT   GPIOB
#define I2C_SCL_PIN    GPIO_Pin_10
#define I2C_SDA_PIN    GPIO_Pin_11

#define I2C_SET_SCL()   GPIO_WriteBit(I2C_PORT, I2C_SCL_PIN, Bit_SET)
#define I2C_RESET_SCL() GPIO_WriteBit(I2C_PORT, I2C_SCL_PIN, Bit_RESET)
#define I2C_SET_SDA()   GPIO_WriteBit(I2C_PORT, I2C_SDA_PIN, Bit_SET)
#define I2C_RESET_SDA() GPIO_WriteBit(I2C_PORT, I2C_SDA_PIN, Bit_RESET)

#define I2C_RD_SDA()    GPIO_ReadOutputDataBit(I2C_PORT, I2C_SDA_PIN)

extern I2C_soft_t  I2C_Soft;

#endif
