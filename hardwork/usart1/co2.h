#ifndef __USART3_H
#define __USART3_H             

extern uint8_t Usart1_RxPacket[6];				
extern uint8_t Usart1_RxFlag;

void USART1_Config(void);
void CO2GetData(uint16_t *data);

#endif


