#include "debug.h"         
#include "PWM.h"     

void PWM_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
	 RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE);	
	 RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode	=	GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin 	=	GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed 	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100-1;					//周期ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 96-1;				//预分频器PSC
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 50;		//CCR的值
	TIM_OC3Init(TIM4,&TIM_OCInitStructure);
	TIM_OC4Init(TIM4,&TIM_OCInitStructure);
	TIM_OC3PreloadConfig( TIM4, TIM_OCPreload_Disable); // 使能ccr1预装载，如果需要PWM启用的情况下修改比较参数，那么通常使能预装载会比较安全
	TIM_OC4PreloadConfig( TIM4, TIM_OCPreload_Disable); // 使能ccr1预装载，如果需要PWM启用的情况下修改比较参数，那么通常使能预装载会比较安全
	TIM_Cmd(TIM4,ENABLE);  //启动定时器

}

void PWM_SetCompare3(uint16_t speed){
	  TIM_SetCompare3(TIM4,speed);
	  Delay_Ms(50);
}


void PWM_SetCompare4(uint16_t speed){
	
		TIM_SetCompare4(TIM4,speed);
		  Delay_Ms(50);
}
