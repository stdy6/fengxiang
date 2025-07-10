#include "SSD1306.h"
#include "SSD1306_font.h"
#include "math.h" 

//============================��ͨѶ������=================================
#include "debug.h"
#include "I2C_soft.h"

#ifdef SPI_SOFT
static void SPI_Soft_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE );

    GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( SPI_CS_PORT, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = I8080_DC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( I8080_DC_PORT, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = SPI_SCLK_PIN | SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( SPI_PORT, &GPIO_InitStructure );
}
#endif /*SPI_SOFT*/

#ifdef SPI_HARD
static void SPI3_HARD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    SPI_InitTypeDef SPI_InitStructure={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI3, ENABLE );

    GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( SPI_CS_PORT, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = I8080_DC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( I8080_DC_PORT, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = SPI_SCLK_PIN | SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( SPI_PORT, &GPIO_InitStructure );

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init( SPI3, &SPI_InitStructure );

    SPI_Cmd( SPI3, ENABLE );
}
#endif /*SPI_HARD*/

#ifdef I2C_HARD
static void I2C2_HARD_Init(u32 bound,u16 host_addr)
{
     GPIO_InitTypeDef GPIO_InitStructure = {0};
     I2C_InitTypeDef  I2C_InitTSturcture = {0};

     RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);     //ʹ��APB1 I2C2����ʱ��
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);    //ʹ��APB2 GPIO����ʱ��

     GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;         //����Ϊ���ÿ�©���ģʽ
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(I2C_PORT, &GPIO_InitStructure);

     I2C_InitTSturcture.I2C_ClockSpeed = bound;     //����I2C����
     I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
     I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_16_9;
     I2C_InitTSturcture.I2C_OwnAddress1 = host_addr;     //ָ�����豸��ַ
     I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
     I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
     I2C_Init(I2C2, &I2C_InitTSturcture);

     I2C_Cmd(I2C2, ENABLE);
     I2C_AcknowledgeConfig(I2C2, ENABLE);

     Delay_Ms(50);
}
#endif /*I2C_HARD*/

static void I8080_Write_Byte(unsigned char _data,unsigned char cmd)
{
#ifdef I2C_SOFT
    I2C_Soft.Start();
    while(I2C_Soft.SendData(SSD1306_I2C_ADDR) == ACK);
    while(I2C_Soft.SendData(cmd?0x40:0x00) == ACK);
    while(I2C_Soft.SendData(_data) == ACK);
    I2C_Soft.Stop();
#endif

#ifdef I2C_HARD
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) != RESET);     //�ж�I2C�Ƿ�æµ

	I2C_GenerateSTART(I2C2, ENABLE);				            //������ʼ�ź�
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C2, SSD1306_I2C_ADDR, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    if(cmd)I2C_SendData(I2C2,0x40);	//ѡ��д����
    else I2C_SendData(I2C2,0x00);	//ѡ��д����
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(I2C2,_data);       //��������
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTOP(I2C2, ENABLE);	//������ֹ�ź�
#endif

#ifdef SPI_SOFT
	SPI_RESET_SCLK();
	SPI_RESET_CS();
	I8080_DC(cmd);
    for (int var = 0; var < 8; ++var)
    {
        SPI_MOSI(_data & 0x80);
        _data <<= 1;

        SPI_RESET_SCLK();
        Delay_Us(2);
        SPI_SET_SCLK();
        Delay_Us(2);
    }
    SPI_SET_CS();
#endif

#ifdef SPI_HARD
    SPI_RESET_CS();
    I8080_DC(cmd);
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI3, _data);
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY));
	SPI_SET_CS();
#endif
} 
//========================================================================

#ifdef I2C_HARD
static void DMA1_Channel4_Init()
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&I2C2->DATAR);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure );

    I2C_DMACmd(I2C2, ENABLE);
}
#endif

#ifdef SPI_HARD
static void DMA2_Channel2_Init()
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA2, ENABLE);

    DMA_DeInit(DMA2_Channel2);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&SPI3->DATAR);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel2, &DMA_InitStructure );
    DMA_Cmd(DMA2_Channel2, ENABLE);
    SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
}
#endif

void SSD1306_Init()
{
//=============================����ʼ����=================================

#ifdef I2C_SOFT
    I2C_Soft.Init();
#endif

#ifdef I2C_HARD
    I2C2_HARD_Init(400000,0x00);    //I2C�����뱾����ַ
    DMA1_Channel4_Init();
    Delay_Ms(100);
#endif

#ifdef SPI_HARD
    SPI3_HARD_Init();
    DMA2_Channel2_Init();
#endif

#ifdef SPI_SOFT
    SPI_Soft_Init();
#endif

    I8080_Write_Byte(0xae,I8080_CMD);//�ر���ʾ
	
	if(SSD1306_DIR)
	{
	    I8080_Write_Byte(0xa0,I8080_CMD);//0xA1->������ʾ 0xA0->���ҵߵ�
	    I8080_Write_Byte(0xc0,I8080_CMD);//0xC8->������ʾ 0xC0->���µߵ�
	}
	else
	{
		I8080_Write_Byte(0xa1,I8080_CMD);//0xA1->������ʾ 0xA0->���ҵߵ�
		I8080_Write_Byte(0xc8,I8080_CMD);//0xC8->������ʾ 0xC0->���µߵ�
	}
	if(SSD1306_PHASE)I8080_Write_Byte(0xa7,I8080_CMD);//0xa6->������ʾ 0xa7->������ʾ
	else I8080_Write_Byte(0xa6,I8080_CMD);//0xa6->������ʾ 0xa7->������ʾ
	
#ifdef OLED12864
	I8080_Write_Byte(0xb0,I8080_CMD);
	I8080_Write_Byte(0x00,I8080_CMD);
	I8080_Write_Byte(0x10,I8080_CMD);
	I8080_Write_Byte(0x40,I8080_CMD);
	I8080_Write_Byte(0x81,I8080_CMD);
	I8080_Write_Byte(SSD1306_BRIGHTNESS,I8080_CMD);//��΢�����ȣ���0-255
	I8080_Write_Byte(0xd3,I8080_CMD);
	I8080_Write_Byte(0x00,I8080_CMD);
	I8080_Write_Byte(0xd5,I8080_CMD);
	I8080_Write_Byte(0x80,I8080_CMD);
	I8080_Write_Byte(0xd9,I8080_CMD);
	I8080_Write_Byte(0xf1,I8080_CMD);
	I8080_Write_Byte(0xda,I8080_CMD);
	I8080_Write_Byte(0x12,I8080_CMD);
	I8080_Write_Byte(0xdb,I8080_CMD);
	I8080_Write_Byte(0x40,I8080_CMD);
	I8080_Write_Byte(0x8d,I8080_CMD);
	I8080_Write_Byte(0x14,I8080_CMD);//0x14->��ѹʹ�� 0x10->������ѹʹ��
#endif

#ifdef OLED12832
	I8080_Write_Byte(0x40,I8080_CMD);
	I8080_Write_Byte(0xb0,I8080_CMD);
	I8080_Write_Byte(0x81,I8080_CMD);
	I8080_Write_Byte(SSD1306_BRIGHTNESS,I8080_CMD);//��΢�����ȣ���0-255
	I8080_Write_Byte(0xa8,I8080_CMD);
	I8080_Write_Byte(0x1f,I8080_CMD);
	I8080_Write_Byte(0xd3,I8080_CMD);
	I8080_Write_Byte(0x00,I8080_CMD);
	I8080_Write_Byte(0xd5,I8080_CMD);
	I8080_Write_Byte(0xf0,I8080_CMD);
	I8080_Write_Byte(0xd9,I8080_CMD);
	I8080_Write_Byte(0x22,I8080_CMD);
	I8080_Write_Byte(0xda,I8080_CMD);
	I8080_Write_Byte(0x02,I8080_CMD);
	I8080_Write_Byte(0xdb,I8080_CMD);
	I8080_Write_Byte(0x49,I8080_CMD);
	I8080_Write_Byte(0x8d,I8080_CMD);
	I8080_Write_Byte(0x14,I8080_CMD);//0x14->��ѹʹ�� 0x10->������ѹʹ��
#endif	

	SSD1306_Fill(0x00,SSD1306_PO);//����
	I8080_Write_Byte(0xaf,I8080_CMD);//����ʾ
}

void SSD1306_Display_EN(unsigned char EN)
{
//============================��������Ļ��================================
//������
//EN:ENABLE->�� DISABLE->��
//=======================================================================
	I8080_Write_Byte(0X8D,I8080_CMD);  //��ѹʹ��
	if(EN)
	{
		I8080_Write_Byte(0X14,I8080_CMD);  //������ѹʹ��
		I8080_Write_Byte(0XAF,I8080_CMD);  //����ʾ
	}
	else
	{
		I8080_Write_Byte(0X10,I8080_CMD);  //������ѹʹ��
		I8080_Write_Byte(0XAE,I8080_CMD);  //�ر���ʾ
	}
}

void SSD1306_Fill(unsigned char _data,unsigned char DISP_PHASE)
{
//============================�������Ļ��================================
//������
//_data:Ҫ��������
//DISP_PHASE:���෴�࣬SSD1306_PO->���࣬SSD1306_NE->����
//======================================================================
    unsigned char i,j;
    for(i = 0;i < SSD1306_HEIGHT / 8;i++)
    {
        I8080_Write_Byte(0xb0 + i,I8080_CMD);
        I8080_Write_Byte(0x00,I8080_CMD);
        I8080_Write_Byte(0x10,I8080_CMD);
        for(j = 0;j < SSD1306_WIDTH;j++)
        {
            if(!DISP_PHASE)I8080_Write_Byte(_data,I8080_DATA);
            else I8080_Write_Byte(~_data,I8080_DATA);
        }
    }
}

static void SSD1306_Pos(unsigned char x, unsigned char y)
{ 
	I8080_Write_Byte(0xb0 + y,I8080_CMD);
	I8080_Write_Byte((x + SSD1306_INCH & 0x0f),I8080_CMD);
	I8080_Write_Byte(((x + SSD1306_INCH & 0xf0) >> 4) | 0x10,I8080_CMD);
}

void SSD1306_DrawPic(unsigned char x, unsigned char y,unsigned char width,unsigned char height,unsigned char *_data,unsigned char DISP_PHASE)
{
//============================����ʾͼƬ��================================
//������
//x:x����
//y:y���꣬��ҳ
//width:ͼƬ��ȣ�0-128���أ�
//height:ͼƬ�߶ȣ�0-8ҳ��
//_data:ͼƬ��ģ
//DISP_PHASE:���෴�࣬SSD1306_PO->���࣬SSD1306_NE->����
//========================================================================
	unsigned int i = 0;
	unsigned char x0,y0;
	for(y0 = y;y0 < y + height;y0++)
	{
		x0 = x;
		SSD1306_Pos(x0,y0);
    	for(x0 = x;x0 < x + width;x0++)
		{
			if(!DISP_PHASE)I8080_Write_Byte(_data[i++],I8080_DATA);
			else I8080_Write_Byte(~_data[i++],I8080_DATA);
		}		    	
	}
}

void SSD1306_DMA_DrawPic(unsigned char *_data)
{
//============================����ʾͼƬ��================================
//������
//_data:ͼƬ��ģ,��֧��ȫ��ˢ�£���֧��OLED12864����֧��Ӳ��I2C��SPI
//========================================================================

#ifdef I2C_HARD
    uint8_t buffer[(SSD1306_WIDTH*SSD1306_HEIGHT/8)+1];//һ�������ֽڽ�����N�������ֽڣ���+1
    uint16_t var, i = 1;

    buffer[0] = 0x40;//Co = 0,DC# = 1
    for (var = 0; var < SSD1306_WIDTH*SSD1306_HEIGHT/8; var++)buffer[var+1] = *(_data + var);

    for (var = 0; var < 8; ++var)
    {
        SSD1306_Pos(0,var);

        while( I2C_GetFlagStatus( I2C2, I2C_FLAG_BUSY ) != RESET );
        I2C_GenerateSTART( I2C2, ENABLE );

        while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_MODE_SELECT ) );
        I2C_Send7bitAddress( I2C2, SSD1306_I2C_ADDR, I2C_Direction_Transmitter );
        while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

        DMA_Cmd(DMA1_Channel4, DISABLE);
        DMA_SetCurrDataCounter(DMA1_Channel4, 1);//һ�η��͵�����
        DMA1_Channel4->MADDR = (u32)buffer;//���õ�ַ
        DMA_Cmd(DMA1_Channel4, ENABLE);

        DMA_Cmd(DMA1_Channel4, DISABLE);
        DMA_SetCurrDataCounter(DMA1_Channel4, 128);//һ�η��͵�����
        DMA1_Channel4->MADDR = (u32)(buffer + i);//���õ�ַ
        DMA_Cmd(DMA1_Channel4, ENABLE);

        while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
        I2C_GenerateSTOP( I2C2, ENABLE );

        i += 128;
    }
#endif

#ifdef SPI_HARD
    uint16_t var;

    for (var = 0; var < 8; ++var)
    {
        SSD1306_Pos(0,var);

        SPI_RESET_CS();
        I8080_DC(I8080_DATA);

        DMA_Cmd(DMA2_Channel2, DISABLE);
        DMA_SetCurrDataCounter(DMA2_Channel2, 128);//һ�η��͵�����
        DMA2_Channel2->MADDR = (u32)(_data + 128 * var);//���õ�ַ
        DMA_Cmd(DMA2_Channel2, ENABLE);

        while(DMA_GetFlagStatus(DMA2_FLAG_TC2) != SET);
        DMA_ClearFlag(DMA2_FLAG_TC2);
        while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY));
    }
    SPI_SET_CS();
    DMA_Cmd(DMA2_Channel2, DISABLE);
#endif
}

unsigned char g_aLcdBuf[SSD1306_WIDTH][SSD1306_HEIGHT / 8];
void SSD1306_DrawDot(unsigned char x,unsigned char y)
{
//==============================����ʾ�㡿================================
//������
//x:x����
//y:y����
//========================================================================
    uint8_t PageNumber = y / 8;
    SSD1306_Pos(x, PageNumber);
    g_aLcdBuf[x][PageNumber] |= 1 << (y % 8);

    I8080_Write_Byte(g_aLcdBuf[x][PageNumber], I8080_DATA);
}

void SSD1306_DrawWave_Sin(unsigned char x)
{
/*==============================���������Ҳ��Ρ�==================================
 * x:������
 */
    SSD1306_DrawDot(x, (SSD1306_HEIGHT/2-1) - (sin(x*0.098125)*(SSD1306_HEIGHT/2-1)));//0.098125->2*2*PI/128
}

void SSD1306_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char _size,unsigned char DISP_PHASE)
{
//=========================����ʾASCII�ַ���==============================
//������
//x:x����
//y:y����
//chr:Ҫ��ʾ��ASCII�ַ�
//_size:����ߴ磬SSD1306_SIZE_x
//DISP_PHASE:���෴�࣬SSD1306_PO->���࣬SSD1306_NE->����
//========================================================================
	unsigned char i;	
	chr -= ' ';
	if(_size == 8)
	{
		SSD1306_Pos(x,y);
		for(i = 0;i < 6;i++)
		{
			if(!DISP_PHASE)I8080_Write_Byte(ASCII6x8[chr][i],I8080_DATA);
			else I8080_Write_Byte(~ASCII6x8[chr][i],I8080_DATA);
		}
	}
	else
    {
        SSD1306_Pos(x,y);//����һҳ
        for(i = 0;i < 8;i++)
        {
            if(!DISP_PHASE)I8080_Write_Byte(ASCII8x16[chr][i],I8080_DATA);//д������
            else I8080_Write_Byte(~ASCII8x16[chr][i],I8080_DATA);//д�밴λȡ���������
        }
        SSD1306_Pos(x,y + 1);//���ڶ�ҳ
        for(i = 0;i < 8;i++)
        {
            if(!DISP_PHASE)I8080_Write_Byte(ASCII8x16[chr][i + 8],I8080_DATA);
            else I8080_Write_Byte(~ASCII8x16[chr][i + 8],I8080_DATA);
        }
    }
}

static unsigned long Math_Pow(unsigned char y)//����10��y�η���10^y������
{
	unsigned long result = 1;	 
	while(y--)result *= 10;    
	return result;
}				  

void SSD1306_ShowNum(unsigned char x,unsigned char y,signed long num,unsigned char len,unsigned char _size,unsigned char DISP_PHASE)
{
//============================����ʾ���֡�===============================
//������
//x:x����
//y:y����
//num:Ҫ��ʾ�����֣�֧�ָ���
//len:����λ��
//_size:����ߴ磬SSD1306_SIZE_x
//DISP_PHASE:���෴�࣬SSD1306_PO->���࣬SSD1306_NE->����
//ע���������
//======================================================================
	unsigned char i, temp, step = 0, zero_flag = 0;
	if(_size == 8)step = 6;	//�������ߴ�Ϊ8���򽫲���ֵ����Ϊ6
	else if(_size == 16) step = 8;//�������ߴ�Ϊ16���򽫲���ֵ����Ϊ8
	if(num < 0)
	{
		num = -num;
		SSD1306_ShowChar(x,y,'-',_size,DISP_PHASE);//��ʾ����
		x += step;//ָ���ƶ�
	}
	else
	{
        SSD1306_ShowChar(x,y,' ',_size,DISP_PHASE);
        x += step;//ָ���ƶ�
    }
	for(i = 0;i < len;i++)//ѭ����ʾÿһ���ַ�
	{
		temp = (num / Math_Pow(len - i - 1)) % 10;//�����ǰλ����ֵ����������Ϊ25��lenΪ3����iΪ0ʱtemp = 0����iΪ1ʱtemp = 2������iΪ2ʱtemp = 5
		if(zero_flag == 0 && i < (len - 1))//δ��������ж������i�������һλʱ
		{
			if(temp == 0)
			{
				SSD1306_ShowChar(x + step * i,y,' ',_size,DISP_PHASE);//���Ϊ0�����Կո����
				continue;//��������ѭ��
			}
			else zero_flag = 1;//�Ӵ˲�������ж����	 
		}
	 	SSD1306_ShowChar(x + step * i,y,temp + '0',_size,DISP_PHASE);//ת��ΪASCII�ַ���ʾ
	}
} 

void SSD1306_ShowNumf(unsigned char x,unsigned char y,float num,unsigned char len,unsigned char _size,unsigned char DISP_PHASE)
{
//============================����ʾС����=================================
//������
//x:x����
//y:y����
//num:Ҫ��ʾ�����֣���λС����֧�ָ���
//len:����λ��������С��λ
//_size:����ߴ磬SSD1306_SIZE_x
//DISP_PHASE:���෴�࣬SSD1306_PO->���࣬SSD1306_NE->����
//ע���������
//========================================================================
	unsigned char i, temp, step = 0, zero_flag = 0;
	unsigned long num1;
	if(_size == 8)step = 6;	//�������ߴ�Ϊ8���򽫲���ֵ����Ϊ6
	else if(_size == 16) step = 8;//�������ߴ�Ϊ16���򽫲���ֵ����Ϊ8
	if(num < 0)
	{
		num = -num;
        SSD1306_ShowChar(x,y,'-',_size,DISP_PHASE);//��ʾ����
        x += step;//ָ���ƶ�
	}
    else
    {
        SSD1306_ShowChar(x,y,' ',_size,DISP_PHASE);
        x += step;//ָ���ƶ�
    }
	num1 = num * 100;
	for(i = 0;i < len;i++)//ѭ����ʾÿһ���ַ�
	{
		temp = (num1 / Math_Pow(len - i - 1)) % 10;//�����ǰλ����ֵ����������Ϊ25��lenΪ3����iΪ0ʱtemp = 0����iΪ1ʱtemp = 2������iΪ2ʱtemp = 5
		if(zero_flag == 0 && i < (len - 3))
		{
			if(temp == 0)
			{
				SSD1306_ShowChar(x + step * i,y,' ',_size,DISP_PHASE);//���Ϊ0�����Կո����
				continue;//��������ѭ��
			}
			else zero_flag = 1;//�Ӵ˲�������ж����	 
		}
		if(i == (len - 2))
		{
			SSD1306_ShowChar(x + step * i,y,'.',_size,DISP_PHASE);
			i++;
			len += 1;
		}		
	 	SSD1306_ShowChar(x + step * i,y,temp + '0',_size,DISP_PHASE);//ת��ΪASCII�ַ���ʾ
	}
}

void SSD1306_ShowStr(unsigned char x,unsigned char y,unsigned char *chr,unsigned char _size,unsigned char DISP_PHASE)
{
//=========================����ʾASCII�ַ�����============================
//������
//x:x����
//y:y����
//chr:Ҫ��ʾ��ASCII�ַ���
//_size:����ߴ磬SSD1306_SIZE_x
//DISP_PHASE:���෴�࣬SSD1306_PO->���࣬SSD1306_NE->����
//========================================================================
	while(*chr != '\0')
	{
		if(*chr == '\r' && *(chr + 1) == '\n')
		{
			if(_size == 8)++y;//����
			else if(_size == 16)y += 2;//����
			x = 0;
			chr += 2;
		}
		if((x > (SSD1306_WIDTH - 6) && _size == 8)||(x > (SSD1306_WIDTH - 8) && _size == 16))//�Զ�����
		{
			x = 0;
			if(_size == 8)++y;//����
			else if(_size == 16)y += 2;//����
		}
		
		SSD1306_ShowChar(x,y,*chr,_size,DISP_PHASE);
		
		if(_size == 8)x += 6;
		else if(_size == 16)x += 8;
			
		chr++;//�ַ���ָ���ƶ�
	}
}

#ifdef CN1616
void SSD1306_ShowCN(unsigned char x,unsigned char y,unsigned char *chr,unsigned char DISP_PHASE)
{
//=========================����ʾ��Ӣ���ַ�����===========================
//������
//x:x����
//y:y����
//chr:Ҫ��ʾ����Ӣ���ַ���
//DISP_PHASE:���෴�࣬SSD1306_PO->���࣬SSD1306_NE->����
//��ע��֧���Զ����У�\r\n����,������ֺ�����ʾ����������Ҫ������\xFD
//========================================================================
	unsigned char chinese_num = sizeof(CN16x16) / sizeof(font_CN_t);//�����ֿ��ַ���
	unsigned char index,i;
	while(*chr != '\0')
	{
		if(*chr == '\r' && *(chr + 1) == '\n')
		{
			y += 2;//����
			x = 0;
			chr += 2 ;
		}
		if((*chr) > 127)//���������
		{
			if(x > SSD1306_WIDTH - 16)//�Զ�����
			{
				x = 0;
				y += 2;//����
			}
			for(index = 0;index < chinese_num;index++)//ѭ������
			{
				if(CN16x16[index].CN_index[0] == *chr && CN16x16[index].CN_index[1] == *(chr + 1))
				{			
					SSD1306_Pos(x,y);
					/***���ֿ��в�����ģ����һҳ***/		
					for(i = 0;i < 16;i++)
					{
						if(!DISP_PHASE)I8080_Write_Byte(CN16x16[index].CN_library[i],I8080_DATA);
						else I8080_Write_Byte(~CN16x16[index].CN_library[i],I8080_DATA);
					}	
					SSD1306_Pos(x,y + 1);//һ������ռ��ҳ��������ת��һҳ
					/***���ֿ��в�����ģ���ڶ�ҳ***/		
					for(i = 0;i < 16;i++)
					{
						if(!DISP_PHASE)I8080_Write_Byte(CN16x16[index].CN_library[i + 16],I8080_DATA);
						else I8080_Write_Byte(~CN16x16[index].CN_library[i + 16],I8080_DATA);
					}
					x += 16;//xָ��������16λ��Ϊ��ʾ��һ��������׼��
					if(x > SSD1306_WIDTH - 16)//�Զ�����
					{
						x=0;
						y += 2;
					}	
				}
			}
			chr += 2;	
			index = 0;
		}
		else 
		{
			if(x > SSD1306_WIDTH - 8)//�Զ�����
			{
				x = 0;
				y += 2;//����
			}
			SSD1306_ShowChar(x,y,*chr,16,DISP_PHASE);
			x += 8;
			chr++;
		}
	}
}
void SSD1306_DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
//============================�����ߡ�=============================
//������x->0-127,y->0-63
//x1:��ʼx����
//y1:��ʼy����
//x2:��ֹx����
//y2:��ֹy����
//================================================================
    u16 Xpoint, Ypoint;
    int dx, dy;
    int XAddway,YAddway;
    int Esp;
    char Dotted_Len;
    Xpoint = x1;
    Ypoint = y1;
    dx = (int)x2 - (int)x1 >= 0 ? x2 - x1 : x1 - x2;
    dy = (int)y2 - (int)y1 <= 0 ? y2 - y1 : y1 - y2;

    XAddway = x1 < x2 ? 1 : -1;
    YAddway = y1 < y2 ? 1 : -1;

    Esp = dx + dy;
    Dotted_Len = 0;

    for (;;)
    {
        Dotted_Len++;
        SSD1306_DrawDot(Xpoint, Ypoint);
        if (2 * Esp >= dy)
        {
            if (Xpoint == x2)break;
            Esp += dy;
            Xpoint += XAddway;
        }
        if (2 * Esp <= dx)
        {
            if (Ypoint == y2)break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }
}

void SSD1306_DrawRectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char fill)
{
//============================�������Ρ�=============================
//������
//x:��ʼx����
//y:��ʼy����
//width:���
//height:�߶�
//fill:�Ƿ����(ENABLE OR DISABLE)
//==================================================================
    u16 i;
    if (fill)
    {
        for(i = y; i < y + height; i++)SSD1306_DrawLine(x,i,x + width,i);
    }
    else
    {
        SSD1306_DrawLine(x, y, x+width, y);
        SSD1306_DrawLine(x, y, x, y+height);
        SSD1306_DrawLine(x+width, y+height, x+width, y);
        SSD1306_DrawLine(x+width, y+height, x, y+height);
    }
}

void SSD1306_DrawCircle(unsigned int x, unsigned int y, unsigned int radius, unsigned char fill)
{
//============================����Բ��=============================
//������
//x:��ʼx����
//y:��ʼy����
//radius:�뾶(����)
//fill:�Ƿ����(ENABLE OR DISABLE)
//================================================================
    int16_t sCurrentX, sCurrentY;
     int16_t sError;

     sCurrentX = 0;
     sCurrentY = radius;
     sError = 3 - (radius << 1);   //�ж��¸���λ�õı�־

     while(sCurrentX <= sCurrentY)
     {
         int16_t sCountY = 0;
         if(fill)
         {
             for(sCountY = sCurrentX; sCountY <= sCurrentY; sCountY++)
             {
                 SSD1306_DrawDot(x + sCurrentX, y + sCountY);     //1���о�����
                 SSD1306_DrawDot(x - sCurrentX, y + sCountY);     //2
                 SSD1306_DrawDot(x - sCountY,   y + sCurrentX);   //3
                 SSD1306_DrawDot(x - sCountY,   y - sCurrentX);   //4
                 SSD1306_DrawDot(x - sCurrentX, y - sCountY);     //5
                 SSD1306_DrawDot(x + sCurrentX, y - sCountY);     //6
                 SSD1306_DrawDot(x + sCountY,   y - sCurrentX);   //7
                 SSD1306_DrawDot(x + sCountY,   y + sCurrentX);   //0
             }
     }
         else
         {
             SSD1306_DrawDot(x + sCurrentX, y + sCurrentY);     //1���о�����
             SSD1306_DrawDot(x - sCurrentX, y + sCurrentY);     //2
             SSD1306_DrawDot(x - sCurrentY, y + sCurrentX);   //3
             SSD1306_DrawDot(x - sCurrentY, y - sCurrentX);   //4
             SSD1306_DrawDot(x - sCurrentX, y - sCurrentY);     //5
             SSD1306_DrawDot(x + sCurrentX, y - sCurrentY);     //6
             SSD1306_DrawDot(x + sCurrentY, y - sCurrentX);   //7
             SSD1306_DrawDot(x + sCurrentY, y + sCurrentX);   //0
     }
         sCurrentX++;
         if(sError < 0)sError += (4*sCurrentX + 6);
         else
         {
             sError += (10 + 4*(sCurrentX - sCurrentY));
             sCurrentY--;
         }
     }
}

#endif
