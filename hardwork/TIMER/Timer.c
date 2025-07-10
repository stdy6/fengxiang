#include "debug.h"              
#include "Timer.h"
#include "onenet.h"
#include "esp8266.h"
#include "Kalman.h"
#include "fan.h"
extern u8 cnt;
extern u8 hard;
extern unsigned char *dataPtr ;
extern int data;

void TIM3_Int_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);		//ʹ��TIM3ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Period		=	arr;	//��ʼ����ʱ��3
	TIM_TimeBaseInitStructure.TIM_Prescaler		=	psc;
	TIM_TimeBaseInitStructure.TIM_CounterMode	=	TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision	=	TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);				//����ʱ��3�����ж�
	
	NVIC_InitStructure.NVIC_IRQChannel 						=	TIM3_IRQn;	//��ʼ��NVIC
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	=	0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			=	0x03;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					=	ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3,ENABLE);									//ʹ�ܶ�ʱ��3
}

void TIM3_IRQHandler(void)__attribute__((interrupt("WCH-Interrupt-fast")));
void TIM3_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{    
		dataPtr = ESP8266_GetIPD(0);  
		if(dataPtr!= NULL)
	    OneNet_RevPro(dataPtr);
		// data=1;
}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}

void TIM2_Int_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);		//ʹ��TIM2ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Period		=	arr;	//��ʼ����ʱ��2
	TIM_TimeBaseInitStructure.TIM_Prescaler		=	psc;
	TIM_TimeBaseInitStructure.TIM_CounterMode	=	TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision	=	TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);				//����ʱ��2�����ж�
	
	NVIC_InitStructure.NVIC_IRQChannel 						=	TIM2_IRQn;	//��ʼ��NVIC
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	=	0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			=	0x03;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					=	ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2,ENABLE);								
}

void TIM2_IRQHandler(void)__attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
	{  
     	Kalman_Calculate();
}
	    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
}

