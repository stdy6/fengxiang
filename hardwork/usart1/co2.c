#include "debug.h"    
#include "co2.h"	
uint8_t Usart1_RxPacket[6];				//����������ݰ�����
uint8_t Usart1_RxFlag;					//����������ݰ���־λ

void USART1_Config(void){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//ʹ��USART3��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��GPIOAʱ��

	//USART1_TX   GPIOB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOB.10
   
    //USART3_RX	  GPIOB.11��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOB.11  

  //Usart3 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�4
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = 9600;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������3
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���3 

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

