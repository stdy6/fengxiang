#ifndef _USART3_H_
#define _USART3_H_
#include "debug.h"

void Usart3_Init(void);
void Usart3Printf( USART_TypeDef *USARTx,char *fmt,...);
void HMI_send_number(char* name, int num);
void HMI_send_float(char* name, float num);
void HMI_send_string(char* name, char* showdata);

#endif
