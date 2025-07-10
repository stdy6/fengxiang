#include "debug.h"    
#include "co2.h"	
uint8_t Usart1_RxPacket[6];				//定义接收数据包数组
uint8_t Usart1_RxFlag;					//定义接收数据包标志位

void USART1_Config(void){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能USART3，GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能GPIOA时钟

	//USART1_TX   GPIOB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOB.10
   
    //USART3_RX	  GPIOB.11初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOB.11  

  //Usart3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级4
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = 9600;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口3
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART1, ENABLE);                    //使能串口3 

}

void USART1_IRQHandler(void)__attribute__((interrupt("WCH-Interrupt-fast")));
void USART1_IRQHandler(void)                	
{
		uint8_t Res;
		static uint8_t RxState = 0;		
		static uint8_t pRxPacket = 0;	

		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
		{
			Res =USART_ReceiveData(USART1);	
			switch (RxState)
			{
				case 0: 
					if (Res == 0x2C)
					{
						Usart1_RxPacket[pRxPacket] = Res;	
						pRxPacket++;
						RxState = 1;

					}
					else
					{
						pRxPacket = 0;
						RxState = 0;
					}
					break;
				case 1: 
						Usart1_RxPacket[pRxPacket] = Res;	
						pRxPacket++;
						if(pRxPacket >= 6)
						{
							pRxPacket = 0;
							RxState = 2;
						}
					break;
				case 2:
					if (Usart1_RxPacket[5] == (uint8_t)(Usart1_RxPacket[0] + Usart1_RxPacket[1]
						+ Usart1_RxPacket[2] + Usart1_RxPacket[3] + Usart1_RxPacket[4]))
					{
						
						RxState = 0;
						pRxPacket = 0;
						Usart1_RxFlag = 1;		
					}
					else
					{
						pRxPacket = 0;
						RxState = 0;
					}
					break;
			}
			
			USART_ClearITPendingBit(USART1, USART_IT_RXNE);	
		} 
} 


void CO2GetData(uint16_t *data)
{
	if (Usart1_RxFlag == 1)
	{
		Usart1_RxFlag = 0;
		*data = Usart1_RxPacket[1] * 256 + Usart1_RxPacket[2];
	}
}

