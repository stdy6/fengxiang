#include "debug.h"
#include "hx711.h"
void HX711_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB , ENABLE);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_InitStructure.GPIO_Pin = HX711_SCK_GPIO_PIN;
		GPIO_Init(HX711_SCK_GPIO_PORT, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = HX711_DT_GPIO_PIN;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
     	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(HX711_DT_GPIO_PORT, &GPIO_InitStructure);	
}
	
unsigned long HX711_GetData(void)
{
		unsigned long Count;
		unsigned char i;
		HX711_SCK_L;
	    Delay_Us(1);
		Count=0;
		while(HX711_DT);
		for (i=0;i<24;i++)
	   {
			HX711_SCK_H;
			 Delay_Us(1);
		    Count=Count<<1;
			HX711_SCK_L;
			Delay_Us(1);
			if(HX711_DT) Count++;
		}
		HX711_SCK_H;
	    Delay_Us(1);
		Count=Count^0x800000;//最高位取反，其他位不变
		HX711_SCK_L;
	    Delay_Us(1);
		return(Count);
}

