#ifndef _ADCX_H_
#define _ADCX_H_
#include "debug.h"

#define    ADCx                          ADC1
#define    ADC_CLK                       RCC_APB2Periph_ADC1

void ADCx_Init(void);
u16 ADC_GetValue(uint8_t ADC_Channel,uint8_t ADC_SampleTime);

#endif
