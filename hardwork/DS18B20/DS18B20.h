#ifndef __DS18B20_H
#define __DS18B20_H

#define DS18B20_PORT GPIOB
#define DS18B20_PIN  GPIO_Pin_12

#define DS18B20_SET     GPIO_SetBits(DS18B20_PORT, DS18B20_PIN)
#define DS18B20_RESET   GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN)
#define DS18B20_READ    GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN)

void DS18B20_Init();
float DS18B20_GetTemp();

enum DS18B20_CMD
{
    DS18B20_CMD_Search_ROM = 0xf0,      //����ROM
    DS18B20_CMD_Read_ROM = 0x33,        //��ROM
    DS18B20_CMD_Match_ROM = 0x55,       //ƥ��ROM
    DS18B20_CMD_Skip_ROM = 0xcc,        //����ROM
    DS18B20_CMD_Alarm_Search = 0xec,    //��������
    DS18B20_CMD_Covert_Temp = 0x44,     //�¶�ת��
    DS18B20_CMD_Write_Scratchpad = 0x4e,//д�ݴ���
    DS18B20_CMD_Read_Scratchpad = 0xbe, //���ݴ���
    DS18B20_CMD_Copy_Scratchpad = 0x48, //�����ݴ�����ֵ��EEPROM
    DS18B20_CMD_Recall_EE = 0xb8,       //��EEPROM����ֵ
    DS18B20_CMD_Read_Powersupply = 0xb4,//����Դ
};

#endif
