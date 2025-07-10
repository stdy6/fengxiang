#include "dht11.h"
#include "debug.h"

DHT11_data DHT11 = {0};

void DHT11_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   

    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);

    DHT11_SET(1);   
    Delay_Ms(1500); 
}

unsigned char DHT11_Read()
{

    uint8_t i, j;
    uint8_t data[5] = {0x00};

    DHT11_SET(0);
    Delay_Ms(20);
    DHT11_SET(1); 

    Delay_Us(30);
    if(DHT11_READ() == Bit_SET)return 1;
    while(DHT11_READ() != Bit_SET);
    Delay_Us(90);

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 8; j++)
        {
            data[i] <<= 1;
            while(DHT11_READ() == Bit_SET);
            while(DHT11_READ() != Bit_SET);
            Delay_Us(40);
            if (DHT11_READ() == Bit_SET)data[i] |= 0x01;
        }
    }

    if((data[0] + data[1] + data[2] + data[3]) != data[4])return 2;//校验

    DHT11.humidity = data[0] + data[1]/10.0f;

    if(data[3] & 0x80)DHT11.temperature = -(data[2] + (data[3] & 0x7f)/10.0f);//负温度处理
    else DHT11.temperature = data[2] + data[3]/10.0f;

    while(!DHT11_READ());
    return 0;
}
