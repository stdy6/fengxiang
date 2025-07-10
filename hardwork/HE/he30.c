#include "he30.h"
#include "debug.h"
#include "dht11.h"

int he=50;
void HE30_Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void HE30_ON()
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
    Delay_Ms(100);
}

void HE30_OFF()
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
    Delay_Ms(100);
}

void HC30R()
{
        if(DHT11.humidity<he)
        {
           HE30_ON();
        }
        else{
           HE30_OFF();
        }
    }
