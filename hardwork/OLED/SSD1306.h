#ifndef __SSD1306_H
#define __SSD1306_H			

//============================【参数设置】=================================
#define SSD1306_I2C_ADDR ((0x3C<<1)|0)	//I2C地址

#define SSD1306_DIR 0	            //0正常显示，1旋转180°
#define SSD1306_PHASE 0             //0正常显示，1反相显示
#define SSD1306_BRIGHTNESS 255	    //亮度（0-255）

#define SSD1306_PO 0                    //正相
#define SSD1306_NE 1                    //反相

#define SSD1306_SIZE_8  8          //字体大小8
#define SSD1306_SIZE_16 16         //字体大小16

#define I8080_CMD  0          //写命令
#define I8080_DATA 1          //写数据

//============================【类型选择】=================================
#define OLED12864
//#define OLED12832

#define SSD1306_INCH 0  //0:0.96inch, 2:1.3inch

#ifdef OLED12864
    #define SSD1306_WIDTH 128           //屏幕宽度
    #define SSD1306_HEIGHT 64           //屏幕高度
#endif

#ifdef OLED12832
    #define SSD1306_WIDTH 128           //屏幕宽度
    #define SSD1306_HEIGHT 32           //屏幕高度
#endif

//============================【通讯方式】=================================
#define I2C_HARD
//#define SPI_HARD
//#define I2C_SOFT
//#define SPI_SOFT

//============================【引脚定义】=================================
#define I2C_PORT       GPIOB
#define I2C_SCL_PIN    GPIO_Pin_10
#define I2C_SDA_PIN    GPIO_Pin_11

#define SPI_PORT         GPIOB
#define SPI_SCLK_PIN     GPIO_Pin_3
#define SPI_MOSI_PIN     GPIO_Pin_5

#define I8080_DC_PORT    GPIOE
#define I8080_DC_PIN     GPIO_Pin_2

#define SPI_CS_PORT    GPIOA
#define SPI_CS_PIN     GPIO_Pin_15

#define SPI_SET_SCLK()      GPIO_WriteBit(SPI_PORT, SPI_SCLK_PIN, Bit_SET)
#define SPI_RESET_SCLK()    GPIO_WriteBit(SPI_PORT, SPI_SCLK_PIN, Bit_RESET)
#define SPI_SET_CS()        GPIO_WriteBit(SPI_CS_PORT, SPI_CS_PIN, Bit_SET)
#define SPI_RESET_CS()      GPIO_WriteBit(SPI_CS_PORT, SPI_CS_PIN, Bit_RESET)
#define I8080_DC(n)         GPIO_WriteBit(I8080_DC_PORT, I8080_DC_PIN, n)
#define SPI_MOSI(n)         GPIO_WriteBit(SPI_PORT, SPI_MOSI_PIN, n)
  
void SSD1306_Init();
void SSD1306_Disp_EN(unsigned char EN);
void SSD1306_Fill(unsigned char _data, unsigned char DISP_PHASE);
void SSD1306_DrawPic(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char *_data, unsigned char DISP_PHASE);
void SSD1306_DMA_DrawPic(unsigned char *_data);
void SSD1306_DrawDot(unsigned char x,unsigned char y);
void SSD1306_DrawWave_Sin(unsigned char x);
void SSD1306_ShowNum(unsigned char x, unsigned char y, signed long num, unsigned char len, unsigned char _size, unsigned char DISP_PHASE);
void SSD1306_ShowNumf(unsigned char x, unsigned char y, float num, unsigned char len, unsigned char _size, unsigned char DISP_PHASE);
void SSD1306_ShowChar(unsigned char x, unsigned char y, unsigned char chr, unsigned char _size, unsigned char DISP_PHASE);
void SSD1306_ShowStr(unsigned char x, unsigned char y, unsigned char *chr, unsigned char _size, unsigned char DISP_PHASE);
void SSD1306_ShowCN(unsigned char x, unsigned char y, unsigned char *chr, unsigned char DISP_PHASE);
void SSD1306_DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void SSD1306_DrawRectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char fill);
void SSD1306_DrawCircle(unsigned int x, unsigned int y, unsigned int radius, unsigned char fill);

#endif
  
