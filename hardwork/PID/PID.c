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

	pid.Kp=5;						//实际为除1倍
	pid.Ki=5;						//实际为除1倍
	pid.Kd=5;						//实际为除1倍
	//pid.Kout=2;						//偏移值
	pid.voltage=0;
	pid.integral=0;					//累计误差
}

int PID_realize(void)
{
	int t;
	pid.Set      = 	SETTEMP;						//设定值 
	pid.Actual   =  TEMP;								//实际值
	pid.err      = pid.Set - pid.Actual;	//之差
	pid.integral = pid.integral + pid.err;
	pid.voltage = pid.Kp*pid.err								//设定值与实际的偏差
				+ pid.Ki*pid.integral/1000						//历史累计偏差
				+ pid.Kd*(pid.err-pid.err_last);			//本次偏差与上次偏差比较				
	pid.err_last = pid.err;
	t=pid.voltage;
	if(t>99)		t=99;
	else if(t<=0)	t=3;

	return t;
}
