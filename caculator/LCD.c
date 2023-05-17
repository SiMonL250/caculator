/*********************************************************************
 * �ļ���  ��LCD.c
 * ����    ��LCD1602Ӧ�ú�����

 * Ӳ�����ӣ�-------------------------
 *            |     PA0 - D0     |
 *            |     PA1 - D1     |
 *            |     PA2 - D2     |
 *            |     PA3 - D3     |
 *            |     PA4 - D4     |
 *            |     PA5 - D5     |
 *            |     PA6 - D6     |
 *            |     PA7 - D7     |
 *            |     PB5 - RS     |
 *            |     PB6 - RW     |
 *            |     PB7 - EN     |
 *          -------------------------
 *       EN �ߵ�ƽ����
 *
 *       RS��0��RW��0 ���� ��ʾ��LCDд��ָ�
 *
 *       RS��0��RW��1 ���� ��ʾ��ȡBusy��־��
 *
 *       RS��1��RW��0 ���� ��ʾ��LCDд�����ݡ�
 *
 *       RS��1��RW��1 ���� ��ʾ��LCD��ȡ���ݡ�
*********************************************************************/
#include "LCD1602.h"
#include "stm32f10x_rcc.h"
#include "Delay.h"
#include <stdio.h>
/*
void delay_us(unsigned int us){
    unsigned int  i;

    do{
        i = 10;
        while(i--) __nop();
    } while (--us);

}
*/
/***********************************GPIO��ʼ��********************************************/
void GPIO_INIT(void)
{ // GPIO��ʼ��
    GPIO_InitTypeDef PB;
    GPIO_InitTypeDef PA;

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // ����jtag����Ȼд�����ͳ���ִ�ж�����Ӱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // ��GPIOA~B

    PB.GPIO_Pin = EN | RW | RS;
    PB.GPIO_Mode = GPIO_Mode_Out_PP;
    PB.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &PB);

    PA.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
                  GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 |
                  GPIO_Pin_6 | GPIO_Pin_7;
    PA.GPIO_Mode = GPIO_Mode_Out_PP;
    PA.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &PA);
}
/***********************************GPIO��ʼ��********************************************/

/***********************************LCD��ʼ��********************************************/
void LCD_INIT(void)
{                // ��ʼ��
    GPIO_INIT(); // GPIO�ĳ�ʼ������LCD_INIT������ʱ�Զ�����

    GPIO_Write(GPIOA, 0x0000); // ��ʼ��GPIOA��GOIOB��ֵΪ0x0000�����Բ��ã�����ϰ��
    GPIO_Write(GPIOB, 0x0000);

    Delay_us(500);

    LCD_WRITE_CMD(0x38);
    LCD_WRITE_CMD(0x0C); // �رչ�����˸
    LCD_WRITE_CMD(0x06);
    LCD_WRITE_CMD(0x01);
}
/***********************************LCD��ʼ��********************************************/

/***********************************д�������********************************************/
void LCD_WRITE_CMD(unsigned char CMD)
{ // д�������
    ReadBusy();
    GPIO_ResetBits(GPIOB, RS); // RS����
    GPIO_ResetBits(GPIOB, RW); // RW����  ��ʾ��LCDд��ָ��
    GPIO_ResetBits(GPIOB, EN); // EN����
    GPIO_Write(GPIOA, CMD);    // ��GPIOA��д��ָ��
    GPIO_SetBits(GPIOB, EN);   // EN����
    GPIO_ResetBits(GPIOB, EN); // EN����
}
/***********************************д�������********************************************/

/***********************************д�뵥��Byte����********************************************/
void LCD_WRITE_ByteDATA(unsigned char ByteData)
{ // д�뵥��Byte����
    ReadBusy();
    GPIO_SetBits(GPIOB, RS);     // RS����
    GPIO_ResetBits(GPIOB, RW);   // RW����  ��ʾ��LCDд������
    GPIO_ResetBits(GPIOB, EN);   // EN����
    GPIO_Write(GPIOA, ByteData); // ��GPIOA��д������
    GPIO_SetBits(GPIOB, EN);     // EN����
    GPIO_ResetBits(GPIOB, EN);   // EN����
}

/***********************************д�뵥��Byte����********************************************/

/***********************************��æ����********************************************/
void ReadBusy(void)
{ // ��æ��������æ֮ǰ�ǵø������ŵĹ�����ʽ��������ΪSTM32��IO����׼˫��IO
    GPIO_Write(GPIOA, 0x00ff);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;             // ѡ��GPIOA�ĵ���Pin
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ����Pin�Ĺ�����ʽΪ��������ģʽ�����ڼ��LCD1602��æ״̬
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, RS); // RS����
    GPIO_SetBits(GPIOB, RW);   // RW����  RS=0 RW=1����ȡBusy��־

    GPIO_SetBits(GPIOB, EN); // ʹ�ܿ�
    while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7))
        ;                      // ������Pin״̬�����һֱΪ1��ѭ���ȴ�
    GPIO_ResetBits(GPIOB, EN); // ʹ�ܹ�

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
                                  GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 |
                                  GPIO_Pin_6 | GPIO_Pin_7; // ʹGPIOA��״̬��ԭ������ģʽ
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/***********************************��æ����********************************************/

/***********************************������ʾ���λ��********************************************/

void LCD_SetCursor(unsigned char Line, unsigned char Columu)
{
    if (Line == 1)
    {
        LCD_WRITE_CMD(0x80 | (Columu - 1));
    }
    else
    {
        LCD_WRITE_CMD(0x80 | (Columu - 1) + 0x40);
    }
}

/***********************************������ʾ���λ��********************************************/

/***********************************��ʾ�ַ�����********************************************/

void LCD_ShowChar(unsigned char Line, unsigned char Columu, unsigned char Char)
{
    LCD_SetCursor(Line, Columu);

    LCD_WRITE_ByteDATA(Char);
}

/***********************************��ʾ�ַ�����********************************************/

/***********************************��ʾ�ַ�������********************************************/

void LCD_ShowString(unsigned char Line, unsigned char Columu, unsigned char *String)
{
    unsigned char i = 0;

    LCD_SetCursor(Line, Columu);

    for (i = 0; String[i] != '\0'; i++)
    {
        LCD_WRITE_ByteDATA(String[i]);
    }
}

/***********************************��ʾ�ַ�������********************************************/

/***********************************��ȡ�η���********************************************/

int LCD_Pow(unsigned char x, unsigned char y) // ���� x �� y ����
{
    int result = 1;
    unsigned char i = 0;

    for (i = 0; i < y; i++)
    {
        result *= x;
    }

    return result;
}

/***********************************��ȡ�η���********************************************/

/***********************************��ʾ���ֺ���********************************************/

void LCD_ShowNum(unsigned char Line, unsigned char Columu, unsigned int Length, unsigned int Num)
{
    unsigned char i = 0;

    LCD_SetCursor(Line, Columu);

    for (i = Length; i > 0; i--)
    {
        LCD_WRITE_ByteDATA('0' + Num / LCD_Pow(10, i - 1) % 10);
    }
}

void LCD_ShowNum_X(unsigned char Line, unsigned char Columu, unsigned int Num)
{
    LCD_SetCursor(Line, Columu);

    char buffer[16];                         // ����ת��Ϊ�ַ�����Ļ�����
    int length = sprintf(buffer, "%u", Num); // ������ת��Ϊ�ַ���
    for (int i = 0; i < length; i++)
    {
        LCD_WRITE_ByteDATA(buffer[i]); // ������ʾÿһλ����
    }
}

void LCD_ShowNum_Y(unsigned int Num)
{

    char buffer[16];                         // ����ת��Ϊ�ַ�����Ļ�����
    int length = sprintf(buffer, "%u", Num); // ������ת��Ϊ�ַ���
    for (int i = 0; i < length; i++)
    {
        LCD_WRITE_ByteDATA(buffer[i]); // ������ʾÿһλ����
    }
}

/***********************************��ʾ���ֺ���********************************************/

/***********************************д���ַ�������********************************************/
/*
void LCD_WRITE_StrDATA( unsigned char *StrData, unsigned char row, unsigned char col ){//д���ַ���
    unsigned char baseAddr = 0x00;			//�����ַ
    if ( row ){
        baseAddr = 0xc0;
    }else{
        baseAddr = 0x80;
    } 	//rowΪ1�û�ѡ��ڶ���
        //rowΪ0�û�ѡ���һ��
    baseAddr += col;

    while ( *StrData != '\0' ){

        LCD_WRITE_CMD( baseAddr );
        LCD_WRITE_ByteDATA( *StrData );

        baseAddr++;			   //ÿ��ѭ����ַ��һ������ָ���һ
        StrData++;
    }
}
*/
/***********************************д���ַ�������********************************************/
