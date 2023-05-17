#ifndef LCD1602_H
#define LCD1602_H

#include "stm32f10x_gpio.h"

/***********************************���Ŷ���********************************************/
#define BUSY 0x80     // æ��־
#define RS GPIO_Pin_5 // ����PB5ΪRS
#define RW GPIO_Pin_6 // PB6ΪRW
#define EN GPIO_Pin_7 // PB7ΪENʹ��
/***********************************���Ŷ���********************************************/

/***********************************��������********************************************/
void ReadBusy(void);
void LCD_WRITE_CMD(unsigned char CMD);
void LCD_WRITE_StrDATA(unsigned char *StrData, unsigned char row, unsigned char col);
void LCD_WRITE_ByteDATA(unsigned char ByteData);
void LCD_INIT(void);
void GPIO_INIT(void);

void LCD_SetCursor(unsigned char Line, unsigned char Columu);
void LCD_ShowChar(unsigned char Line, unsigned char Columu, unsigned char Char);
void LCD_ShowString(unsigned char Line, unsigned char Columu, unsigned char *String);
void LCD_ShowNum(unsigned char Line, unsigned char Columu, unsigned int Length, unsigned int Num);
void LCD_ShowNum_X(unsigned char Line, unsigned char Columu, unsigned int Num);
void LCD_ShowNum_Y(unsigned int Num);
/***********************************��������********************************************/
#endif
