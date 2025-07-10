#ifndef __LDR_H
#define	__LDR_H
#include "adcx.h"
#include "math.h"
#include "debug.h"

#define LDR_READ_TIMES	10 

#define		LDR_GPIO_CLK							RCC_APB2Periph_GPIOC
#define 	LDR_GPIO_PORT							GPIOC
#define 	LDR_GPIO_PIN							GPIO_Pin_0
#define   ADC_CHANNEL               ADC_Channel_10

void LDR_Init(void);
uint16_t LDR_Average_Data(void);
uint16_t LDR_LuxData();

#endif 

