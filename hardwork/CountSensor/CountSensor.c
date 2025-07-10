#include "debug.h"
#include "CountSensor.h"

uint16_t CountSensor_Count1;				//全局变量，用于计数
uint16_t CountSensor_Count2;		
uint8_t lastTriggeredSensor = 0;      // 0=无, 1=传感器1(PC8), 2=传感器2(PC9)

void CountSensor_Init1(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//开启AFIO的时钟，外部中断必须开启AFIO的时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);						//将PC8引脚初始化为上拉输入
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource8);  //将外部中断的8号线映射到GPIOC，即选择PC8为外部中断引脚
	
	EXTI_InitTypeDef EXTI_InitStructure;						
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;					
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		
	EXTI_Init(&EXTI_InitStructure);								
	
	NVIC_InitTypeDef NVIC_InitStructure;						
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			
	NVIC_Init(&NVIC_InitStructure);								
}

void CountSensor_Init2(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);						
	
	/*AFIO选择中断引脚*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource10);
	
	/*EXTI初始化*/
	EXTI_InitTypeDef EXTI_InitStructure;						
	EXTI_InitStructure.EXTI_Line = EXTI_Line10;				
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		
	EXTI_Init(&EXTI_InitStructure);								
	
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			
	NVIC_Init(&NVIC_InitStructure);							
}

uint16_t CountSensor_Get1(void)
{
	return CountSensor_Count1;
}

uint16_t CountSensor_Get2(void)
{
	return CountSensor_Count2;
}

void EXTI9_5_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI9_5_IRQHandler(void)
{
	 if (EXTI_GetITStatus(EXTI_Line8) == SET)
    {
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) == 1) // 确认是有效触发
        {
            if (lastTriggeredSensor == 0) 
            {
                // 第一次触发，记录状态
                lastTriggeredSensor = 1;
            }
            else if (lastTriggeredSensor == 2)
            {  
                // 先触发传感器2，再触发传感器1，且时间间隔短，判断为进入
                CountSensor_Count1 ++;
                lastTriggeredSensor = 0; // 重置状态
				Delay_Ms(500);
            }
            else
            {
                // 无效触发序列，重置状态
                lastTriggeredSensor = 1;
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
}
void EXTI15_10_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI15_10_IRQHandler(void){
    if (EXTI_GetITStatus(EXTI_Line10) == SET)
    {
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10) == 1) // 确认是有效触发
        {
            if (lastTriggeredSensor == 0) 
            {
                // 第一次触发，记录状态
                lastTriggeredSensor = 2;
            }
            else if (lastTriggeredSensor == 1)
            {
                // 先触发传感器1，再触发传感器2，且时间间隔短，判断为出去
                CountSensor_Count2 ++;
                lastTriggeredSensor = 0; // 重置状态
				Delay_Ms(500);
            }
            else
            {
                // 无效触发序列，重置状态
                lastTriggeredSensor = 2;
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line10);
    }
}
