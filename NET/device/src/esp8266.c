#include "esp8266.h"
#include "debug.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>


#define ESP8266_WIFI_INFO		"AT+CWJAP=\"123456789\",\"lzy204517\"\r\n"           //WIFI����������
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"
unsigned char esp8266_buf[512];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;

void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)			
	{
		
		esp8266_cnt = 0;							
			
		return REV_OK;								
	}
		
	esp8266_cntPre = esp8266_cnt;				
	
	return REV_WAIT;							
    	
}

_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	 unsigned char timeOut = 200;
    
	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
     
	 while(timeOut--)
	{
		
		if(ESP8266_WaitRecive() == REV_OK)					//����յ�����
		
		{
			// UsartPrintf(USART_DEBUG, "RECV: %s\r\n", esp8266_buf);
			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
				//UsartPrintf(USART_DEBUG, "123\r\n");
				ESP8266_Clear();									//��ջ���
				
				return 0;
			}
		}
		
		Delay_Ms(10);
	}
	return 1;

}


void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
	{
		Usart_SendString(USART2, data, len);		//�����豸������������
	}

}

//	�������ܣ�	��ȡƽ̨���ص�����
 unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	do
	{
			
		if(ESP8266_WaitRecive() == REV_OK)								//����������
		{
		
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//������IPD��ͷ
		    //  printf("RX Buffer: %s\r\n", (char*)esp8266_buf);
			if(ptrIPD == NULL)											
			{
				//   UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
				
					return NULL;
				
			}
		}
		
		Delay_Ms(5);										 			//��ʱ�ȴ�
	} while(timeOut--);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

void ESP8266_Init(void)
{
	ESP8266_Clear();
	while(ESP8266_SendCmd("AT\r\n","OK"))
		Delay_Ms(500);

	while(ESP8266_SendCmd("AT+CWMODE=1\r\n","OK"))
		Delay_Ms(500);
	
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
		Delay_Ms(500);
	
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
		Delay_Ms(500);

	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		Delay_Ms(500);

	
}



void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast"))) ;
void USART2_IRQHandler(void)
{
    // ��������ֱ����������
    while(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(USART2); // ��ȡ���ݣ��Զ����RXNE��־��
        
        // �洢���ݣ������������
        if(esp8266_cnt < sizeof(esp8266_buf) - 1) {
            esp8266_buf[esp8266_cnt++] = data;
        }
        
        // ��ѡ��������������޿����׶Σ�
        // USART_SendData(USART_DEBUG, data); // ֱ�ӻ����ַ�
    }
    
    // ��鲢��������־
    if(USART_GetFlagStatus(USART2, USART_FLAG_ORE)) {
        USART_ClearFlag(USART2, USART_FLAG_ORE); // ����������
    }
}
