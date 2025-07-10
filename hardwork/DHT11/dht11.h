#ifndef __DHT11_H
#define __DHT11_H

#define DHT11_PORT      GPIOB
#define DHT11_PIN       GPIO_Pin_14

#define DHT11_SET(n)    GPIO_WriteBit(DHT11_PORT, DHT11_PIN, n)
#define DHT11_READ()    GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN)

typedef struct
{
    float temperature;
    float humidity;
}DHT11_data;

void DHT11_Init();
unsigned char DHT11_Read();

extern DHT11_data DHT11;

#endif
