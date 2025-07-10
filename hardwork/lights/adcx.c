#include "adcx.h"
#include "debug.h"

void ADCx_Init(void)
{
	//����ADCʱ��
	RCC_APB2PeriphClockCmd(ADC_CLK, ENABLE);
	
	//ADCƵ�ʽ���6��Ƶ
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	//����ADC�ṹ��
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//����ģʽ
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//�����Ҷ���
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//�������
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//����ת��
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//��ɨ��ģʽ
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//��ͨ����
	ADC_Init(ADCx, &ADC_InitStructure);	//��ʼ��ADC1
	
	ADC_Cmd(ADCx, ENABLE);
	
	ADC_ResetCalibration(ADCx);
	while(ADC_GetResetCalibrationStatus(ADCx) == SET);
	ADC_StartCalibration(ADCx);
	while(ADC_GetCalibrationStatus(ADCx) == SET);	
}

u16 ADC_GetValue(uint8_t ADC_Channel,uint8_t ADC_SampleTime)
{
	ADC_RegularChannelConfig(ADCx, ADC_Channel, 1, ADC_SampleTime);
	ADC_SoftwareStartConvCmd(ADCx, ENABLE); 
	while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET); 
	return ADC_GetConversionValue(ADCx);
}
