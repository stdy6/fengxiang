#include "adcx.h"
#include "debug.h"

void ADCx_Init(void)
{
	//开启ADC时钟
	RCC_APB2PeriphClockCmd(ADC_CLK, ENABLE);
	
	//ADC频率进行6分频
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	//配置ADC结构体
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//独立模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//数据右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//软件触发
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//单次转换
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//非扫描模式
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//总通道数
	ADC_Init(ADCx, &ADC_InitStructure);	//初始化ADC1
	
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
