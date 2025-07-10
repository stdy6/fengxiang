#include "debug.h"
#include "CountSensor.h"

uint16_t CountSensor_Count1;				//ȫ�ֱ��������ڼ���
uint16_t CountSensor_Count2;		
uint8_t lastTriggeredSensor = 0;      // 0=��, 1=������1(PC8), 2=������2(PC9)

void CountSensor_Init1(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//����AFIO��ʱ�ӣ��ⲿ�жϱ��뿪��AFIO��ʱ��
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);						//��PC8���ų�ʼ��Ϊ��������
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource8);  //���ⲿ�жϵ�8����ӳ�䵽GPIOC����ѡ��PC8Ϊ�ⲿ�ж�����
	
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
	/*����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		
	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);						
	
	/*AFIOѡ���ж�����*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource10);
	
	/*EXTI��ʼ��*/
	EXTI_InitTypeDef EXTI_InitStructure;						
	EXTI_InitStructure.EXTI_Line = EXTI_Line10;				
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		
	EXTI_Init(&EXTI_InitStructure);								
	
	
	/*NVIC����*/
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
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) == 1) // ȷ������Ч����
        {
            if (lastTriggeredSensor == 0) 
            {
                // ��һ�δ�������¼״̬
                lastTriggeredSensor = 1;
            }
            else if (lastTriggeredSensor == 2)
            {  
                // �ȴ���������2���ٴ���������1����ʱ�����̣��ж�Ϊ����
                CountSensor_Count1 ++;
                lastTriggeredSensor = 0; // ����״̬
				Delay_Ms(500);
            }
            else
            {
                // ��Ч�������У�����״̬
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
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10) == 1) // ȷ������Ч����
        {
            if (lastTriggeredSensor == 0) 
            {
                // ��һ�δ�������¼״̬
                lastTriggeredSensor = 2;
            }
            else if (lastTriggeredSensor == 1)
            {
                // �ȴ���������1���ٴ���������2����ʱ�����̣��ж�Ϊ��ȥ
                CountSensor_Count2 ++;
                lastTriggeredSensor = 0; // ����״̬
				Delay_Ms(500);
            }
            else
            {
                // ��Ч�������У�����״̬
                lastTriggeredSensor = 2;
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line10);
    }
}
