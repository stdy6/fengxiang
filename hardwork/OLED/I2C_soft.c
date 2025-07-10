#include  "I2C_soft.h"

static void Init(void);
static void Start(void);
static void Stop(void);
static ack_t SendData(uint8_t);
static uint8_t ReceiveData(ack_t);

I2C_soft_t I2C_Soft =
{
	Init,
	Start,
	Stop,
	SendData,
	ReceiveData,
};

static void Init(void)
{
    //�����ṹ��
     GPIO_InitTypeDef GPIO_InitStructure = {0};

     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);    //ʹ��APB2 GPIO����ʱ��

     GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;         //����Ϊ�������ģʽ
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(I2C_PORT, &GPIO_InitStructure);

     I2C_SET_SCL();
     I2C_SET_SDA();
}

static void Start(void)
{
	I2C_SET_SDA();
	I2C_SET_SCL();
	I2C_SOFT_GLOBAL_DELAY();
	
	I2C_RESET_SDA();
	I2C_SOFT_GLOBAL_DELAY();
	
	I2C_RESET_SCL();
	I2C_SOFT_GLOBAL_DELAY();
}

static void Stop(void)
{
	I2C_RESET_SDA();
	I2C_SET_SCL();
	I2C_SOFT_GLOBAL_DELAY();

	I2C_SET_SDA();
}

static ack_t SendData(uint8_t _data)
{
	uint8_t i;
	ack_t  ack;
	
	for(i = 0;i < 8;i++)
	{
		I2C_RESET_SCL();
		I2C_SOFT_GLOBAL_DELAY();

		if((_data&(1<<7)) == (1<<7))I2C_SET_SDA();
		else I2C_RESET_SDA();
		I2C_SOFT_GLOBAL_DELAY();

		I2C_SET_SCL();
		I2C_SOFT_GLOBAL_DELAY();

		_data <<= 1;
	}
	
	I2C_RESET_SCL();
	I2C_SET_SDA();//�ͷ�SDA���ȴ��ӻ�Ӧ��
	I2C_SOFT_GLOBAL_DELAY();
	
	I2C_SET_SCL();
	I2C_SOFT_GLOBAL_DELAY();
	
	ack = (ack_t)I2C_RD_SDA();
	
	I2C_RESET_SCL();
	I2C_SOFT_GLOBAL_DELAY();
	
	//���شӻ���Ӧ���ź�
	return ack;
}

static uint8_t ReceiveData(ack_t ack)
{
	uint8_t _data = 0,i;

	for(i = 0;i < 8;i++)
	{
	    _data <<= 1;

		I2C_RESET_SCL();//SCL���㣬�ӻ�׼������
		I2C_SOFT_GLOBAL_DELAY();
		
		I2C_SET_SCL();//SCL�øߣ���ȡSDA��������
		I2C_SOFT_GLOBAL_DELAY();

		_data |= I2C_RD_SDA();
	}
	
	I2C_RESET_SCL();//SCL���㣬����׼��Ӧ���ź�
	I2C_SOFT_GLOBAL_DELAY();
	
	//��������Ӧ���ź�	
	if(ack == ACK)I2C_RESET_SDA();
	else I2C_SET_SDA();
	I2C_SOFT_GLOBAL_DELAY();
	
	I2C_SET_SCL();
	I2C_SOFT_GLOBAL_DELAY();
	
	//SCL�����㣬���ͷ�SDA����ֹ������������ʱ���ӻ���SDA�ͷ��źŵ���NACk�ź�
	I2C_RESET_SCL();
	I2C_SET_SDA();
	I2C_SOFT_GLOBAL_DELAY();

	return _data;
}
