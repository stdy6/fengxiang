#include "DS18B20.h"
#include "debug.h"

void DS18B20_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  
    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
    DS18B20_SET;    
}

static unsigned char DS18B20_RST()
{
    uint8_t var;    //��ʱ����
    DS18B20_SET;    //�ͷ�����
    Delay_Us(5);    //��һ��
    DS18B20_RESET;  //��������
    Delay_Ms(2);    //��������1ms�����͸�λ����
    DS18B20_SET;    //�ͷ�����
    Delay_Us(70);   //�ȴ�15-60us
    var = !DS18B20_READ;  //���DS18B20
    Delay_Us(500);  //����480us
    return var;
}

static unsigned char DS18B20_ReadByte()
{
    uint8_t _data = 0x00, var;
    for (var = 0; var < 8; var++)
    {
        _data >>= 1;
        DS18B20_RESET;
        Delay_Us(5);//��������1us
        DS18B20_SET;//�ͷ�����
        Delay_Us(10);//15us�ڿ�ʼ����
        if(DS18B20_READ)_data |= 0x80;  //��λ��ǰ
        Delay_Us(60);//10+60>15+45us
    }
    return _data;
}

static void DS18B20_WriteByte(uint8_t _data)
{
    uint8_t var;
    for(var = 0;var < 8;var++)
    {
        if(_data & 0x01)
        {
            DS18B20_RESET;
            Delay_Us(5);//15us���ͷ�����->��1
            DS18B20_SET;
            Delay_Us(70);//��15-60us�Ĵ����ڲ���
        }
        else
        {
            DS18B20_RESET;
            Delay_Us(70);//����60us->��0
            DS18B20_SET;
            Delay_Us(5);//��15-60us�Ĵ����ڲ���
        }
        _data >>= 1;
    }
}

float DS18B20_GetTemp()
{
    uint16_t temp = 0x00;

    if(DS18B20_RST())
    {
        DS18B20_WriteByte(DS18B20_CMD_Skip_ROM);
        DS18B20_WriteByte(DS18B20_CMD_Covert_Temp);
        Delay_Ms(1000);
        DS18B20_RST();
        DS18B20_WriteByte(DS18B20_CMD_Skip_ROM);
        DS18B20_WriteByte(DS18B20_CMD_Read_Scratchpad);
        temp |= DS18B20_ReadByte();
        temp |= DS18B20_ReadByte() << 8;
        if(temp & 0x8000)return -((u16)(~temp + 1) * 0.0625);
        else return (temp * 0.0625);
    }
    else return 999;
}




