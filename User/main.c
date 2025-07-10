#include "debug.h"
#include "dht11.h"
#include "onenet.h"
#include "esp8266.h"
#include "usart.h"
#include "CountSensor.h"
#include "MqttKit.h"
#include "cJSON.h"
#include "Motor.h"
#include "Cooling.h"
#include "DS18B20.h"
#include "PID.h"
#include "PID2.h"
#include "hx711.h"
#include "PWM.h"
#include "he30.h"
#include "co2.h"
#include "LDR.h"
#include "adcx.h"
#include "MPU6050.h"
#include "Kalman.h"
#include "usart3.h"
#include "Timer.h"
#include "fan.h"

#include <stdio.h>
#include <string.h>

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"
void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
unsigned char *dataPtr = NULL;

float TEMP;					
int SETTEMP=20;					
uint8_t flag100ms;
uint8_t flag500ms;
uint8_t	Hot_flag=0;
uint8_t	Cool_flag=0;
extern struct PID pid;
extern struct PID2 pid2;
int8_t	Speed;
int8_t	Speed2;

int value;
float weight;
int32_t reset;
u8 buff[30];

float Weights=200.0;  
int32_t Weights_100=8514990;  
int ang=0;
u8 cnt=10;
uint16_t co2;
u8 buff[30];
int time;
u16 light;
extern int he;
void angle();
void allint();
void temp();

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SystemCoreClockUpdate();
	Delay_Init();

    allint();
	ESP8266_Init();					              //初始化ESP8266
	while(OneNet_DevLink())		                  //接入OneNET
    Delay_Ms (500);
	OneNET_Subscribe();                           // 处理ESP8266连接和数据发送
    
	while(1)
{
	value = HX711_GetData();
	weight=(float)(value-reset)*Weights/(float)(Weights_100-reset);
    HMI_send_number("x1.val",weight*100);          //重量

	// Kalman_Calculate();
	// HMI_send_number("x1.val",(int)pitch);
	angle();                                 //倾角

	light = LDR_LuxData();
    HMI_send_number("n2.val",light);          //光照
	
	CO2GetData(&co2);
    HMI_send_number("n3.val",co2);            //co2
	
    // CO2_CONTROLLER();
    if(co2>800)
        GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_SET);
    else
        GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_RESET);

    while(DHT11_Read()){}
    HMI_send_number("n0.val",(int)DHT11.humidity);   //湿度

	HC30R();                                      
	HMI_send_number("n4.val",he);                     //加湿器

		int t=CountSensor_Get1()-CountSensor_Get2();
		HMI_send_number("n1.val",t);                 //数量

		TEMP=DS18B20_GetTemp();
		HMI_send_number("x0.val",TEMP*100);              
		HMI_send_number("n5.val",SETTEMP);           //温度
		temp();                                     //加热制冷

        cnt++;
		if(cnt>=10){
		cnt=0;
        OneNet_SendData();									
		ESP8266_Clear();
		 }
 }
 }

void allint(){
    Usart2_Init(115200);							//串口2，驱动ESP8266用	
    Usart3_Init();
    DHT11_Init();
	Motor_Init();
	Cooling_Init();
	DS18B20_Init();
	PID_Init();
	PID2_Init();
    PWM_Init();
	CountSensor_Init1();
    CountSensor_Init2();
	HE30_Init();
	HX711_Init();
	reset = HX711_GetData();
    LDR_Init();
	USART1_Config();
	MPU6050_Init();	
	Kalman_Init();
	FAN_Init();
	TIM3_Int_Init(3999,7199);
	TIM2_Int_Init(399,7199);
}

void angle(){
    if(fabs(-90-pitch)<20&&fabs(15-roll)<20){
         ang=0;
        HMI_send_string("t7.txt", "NORMAL");
	}
    else
	{
		ang=1;
        HMI_send_string("t7.txt", "WARNING");    //倾角警告
	}
}

void temp(){
    				if((TEMP-SETTEMP)>1||(TEMP-SETTEMP)<-1)
				{
					if(SETTEMP<=TEMP){
						pid.integral=0;
					    Speed2=PID2_realize();
					    Cooling_SetSpeed(Speed2);
					    Motor_SetSpeed(0);
				        HMI_send_string("t11.txt", "COOLING");
                     }
					if(SETTEMP>=TEMP){
						pid2.integral=0;
						Speed=PID_realize();
				        Motor_SetSpeed(Speed);
					    Cooling_SetSpeed(0);
			            HMI_send_string("t11.txt", "HEATING");
           }                                                                                   
					}
				else{
					    pid.integral=0;
					    pid2.integral=0;
					    Cooling_SetSpeed(0);
					    Motor_SetSpeed(0);
			            HMI_send_string("t11.txt", "WAITING");
                    }
				
}

