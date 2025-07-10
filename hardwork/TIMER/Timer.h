#ifndef _TIMER_H
#define _TIMER_H

void TIM3_Int_Init(uint16_t arr,uint16_t psc);
void TIM2_Int_Init(uint16_t arr,uint16_t psc);
void TIM3_IRQHandler(void);
void TIM2_IRQHandler(void);


#endif
