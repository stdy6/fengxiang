#ifndef	_PID_H
#define _PID_H

struct PID{
	signed int Set;				//�趨ֵ
	signed int Actual;			//ʵ��ֵ
	signed int err;				//��ǰ���
	signed int err_last;		//��һ�����
	
	signed int Kp,Ki,Kd,Kout;
	signed int voltage;			//����ֵ
	signed int integral;		//������ֵ
};

void PID_Init(void)	;
int PID_realize(void)	;


#endif
