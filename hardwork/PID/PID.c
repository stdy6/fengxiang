#include "debug.h"         
#include "PID.h"

extern uint8_t TEMP;						
extern uint8_t SETTEMP;				
extern uint8_t Hot_flag;
//extern uint8_t Cool_flag;
//extern uint8_t KeyNum;	

struct PID pid;

void PID_Init(void)				
{
	pid.Set=SETTEMP;
	pid.Actual=TEMP;
	pid.err=0;
	pid.err_last=0;                                                                                                                                                                                                   

	pid.Kp=5;						//ʵ��Ϊ��1��
	pid.Ki=5;						//ʵ��Ϊ��1��
	pid.Kd=5;						//ʵ��Ϊ��1��
	//pid.Kout=2;						//ƫ��ֵ
	pid.voltage=0;
	pid.integral=0;					//�ۼ����
}

int PID_realize(void)
{
	int t;
	pid.Set      = 	SETTEMP;						//�趨ֵ 
	pid.Actual   =  TEMP;								//ʵ��ֵ
	pid.err      = pid.Set - pid.Actual;	//֮��
	pid.integral = pid.integral + pid.err;
	pid.voltage = pid.Kp*pid.err								//�趨ֵ��ʵ�ʵ�ƫ��
				+ pid.Ki*pid.integral/1000						//��ʷ�ۼ�ƫ��
				+ pid.Kd*(pid.err-pid.err_last);			//����ƫ�����ϴ�ƫ��Ƚ�				
	pid.err_last = pid.err;
	t=pid.voltage;
	if(t>99)		t=99;
	else if(t<=0)	t=3;

	return t;
}
