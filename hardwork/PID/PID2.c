#include "debug.h"          
#include "PID2.h"

extern uint8_t TEMP;					
extern uint8_t SETTEMP;					
extern uint8_t Cool_flag;
struct PID2 pid2;

void PID2_Init(void)				
{
	pid2.Set= SETTEMP-1;
	pid2.Actual= TEMP;
	pid2.err=0;
	pid2.err_last=0;                                                                                                                                                                                                   

	pid2.Kp=30;						//ʵ��Ϊ��1��
	pid2.Ki=10;						//ʵ��Ϊ��1��
	pid2.Kd=10;						//ʵ��Ϊ��1��
	//pid.Kout=2;						//ƫ��ֵ
	pid2.voltage=0;
	pid2.integral=0;					//�ۼ����
}

int PID2_realize(void)
{
	int t;
	pid2.Set      = TEMP;						//�趨ֵ 
	pid2.Actual   = SETTEMP-1;								//ʵ��ֵ
	pid2.err      = pid2.Set - pid2.Actual;	//֮��
	pid2.integral = pid2.integral + pid2.err;

	pid2.voltage = pid2.Kp*pid2.err								//�趨ֵ��ʵ�ʵ�ƫ��
				+ pid2.Ki*pid2.integral/1000						//��ʷ�ۼ�ƫ��
				+ pid2.Kd*(pid2.err-pid2.err_last);			//����ƫ�����ϴ�ƫ��Ƚ�			
	pid2.err_last = pid2.err;
	t=pid2.voltage;
	
	if(t>99)		t=99;
	else if(t<=0)	t=3;
	Cool_flag=0;
	return t;
}
