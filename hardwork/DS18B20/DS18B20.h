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
    DS18B20_CMD_Search_ROM = 0xf0,      //搜索ROM
    DS18B20_CMD_Read_ROM = 0x33,        //读ROM
    DS18B20_CMD_Match_ROM = 0x55,       //匹配ROM
    DS18B20_CMD_Skip_ROM = 0xcc,        //跳过ROM
    DS18B20_CMD_Alarm_Search = 0xec,    //报警搜索
    DS18B20_CMD_Covert_Temp = 0x44,     //温度转换
    DS18B20_CMD_Write_Scratchpad = 0x4e,//写暂存器
    DS18B20_CMD_Read_Scratchpad = 0xbe, //读暂存器
    DS18B20_CMD_Copy_Scratchpad = 0x48, //复制暂存器的值到EEPROM
    DS18B20_CMD_Recall_EE = 0xb8,       //从EEPROM调出值
    DS18B20_CMD_Read_Powersupply = 0xb4,//读电源
};

#endif
