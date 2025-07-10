#include "debug.h"
#include "PWM.h"
#include "Cooling.h"
void Cooling_Init(void){
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);		
		
	GPIO_InitTypeDef	GPIO_InitStructure;		
	GPIO_InitStructure.GPIO_Mode	=	GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin 	=	GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed 	=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
 
	PWM_Init();
}

void Cooling_SetSpeed(int8_t Speed){
	
	if(Speed>0)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_6);
		GPIO_ResetBits(GPIOA,GPIO_Pin_7);
		// PWM_SetCompare4(Speed);
		GPIO_SetBits(GPIOB,GPIO_Pin_9);
	}
}
