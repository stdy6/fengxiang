#ifndef	_PID_H
#define _PID_H

struct PID{
	signed int Set;				//设定值
	signed int Actual;			//实际值
	signed int err;				//当前误差
	signed int err_last;		//上一次误差
	
	signed int Kp,Ki,Kd,Kout;
	signed int voltage;			//计算值
	signed int integral;		//误差积分值
};

void PID_Init(void)	;
int PID_realize(void)	;


#endif
