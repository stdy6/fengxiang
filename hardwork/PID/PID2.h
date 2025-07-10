#ifndef	_PID2_H
#define _PID2_H

struct PID2{
	signed int Set;				//�趨ֵ
	signed int Actual;			//ʵ��ֵ
	signed int err;				//��ǰ���
	signed int err_last;		//��һ�����
	
	signed int Kp,Ki,Kd,Kout;
	signed int voltage;			//����ֵ
	signed int integral;		//������ֵ
};

void PID2_Init(void)	;
int PID2_realize(void)	;


#endif
