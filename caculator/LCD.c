/*********************************************************************
 * 文件名  ：LCD.c
 * 描述    ：LCD1602应用函数库

 * 硬件连接：-------------------------
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
 *       EN 高电平激活
 *
 *       RS＝0、RW＝0 —— 表示向LCD写入指令。
 *
 *       RS＝0、RW＝1 —— 表示读取Busy标志。
 *
 *       RS＝1、RW＝0 —— 表示向LCD写入数据。
 *
 *       RS＝1、RW＝1 —— 表示从LCD读取数据。
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
/***********************************GPIO初始化********************************************/
void GPIO_INIT(void)
{ // GPIO初始化
    GPIO_InitTypeDef PB;
    GPIO_InitTypeDef PA;

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // 禁用jtag，不然写入程序和程序执行都会受影响
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 打开GPIOA~B

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
/***********************************GPIO初始化********************************************/

/***********************************LCD初始化********************************************/
void LCD_INIT(void)
{                // 初始化
    GPIO_INIT(); // GPIO的初始化，在LCD_INIT被调用时自动调用

    GPIO_Write(GPIOA, 0x0000); // 初始化GPIOA和GOIOB的值为0x0000，可以不用，个人习惯
    GPIO_Write(GPIOB, 0x0000);

    Delay_us(500);

    LCD_WRITE_CMD(0x38);
    LCD_WRITE_CMD(0x0C); // 关闭光标和闪烁
    LCD_WRITE_CMD(0x06);
    LCD_WRITE_CMD(0x01);
}
/***********************************LCD初始化********************************************/

/***********************************写入命令函数********************************************/
void LCD_WRITE_CMD(unsigned char CMD)
{ // 写入命令函数
    ReadBusy();
    GPIO_ResetBits(GPIOB, RS); // RS拉低
    GPIO_ResetBits(GPIOB, RW); // RW拉低  表示向LCD写入指令
    GPIO_ResetBits(GPIOB, EN); // EN拉低
    GPIO_Write(GPIOA, CMD);    // 向GPIOA中写入指令
    GPIO_SetBits(GPIOB, EN);   // EN拉高
    GPIO_ResetBits(GPIOB, EN); // EN拉低
}
/***********************************写入命令函数********************************************/

/***********************************写入单个Byte函数********************************************/
void LCD_WRITE_ByteDATA(unsigned char ByteData)
{ // 写入单个Byte函数
    ReadBusy();
    GPIO_SetBits(GPIOB, RS);     // RS拉高
    GPIO_ResetBits(GPIOB, RW);   // RW拉低  表示向LCD写入数据
    GPIO_ResetBits(GPIOB, EN);   // EN拉低
    GPIO_Write(GPIOA, ByteData); // 向GPIOA中写入数据
    GPIO_SetBits(GPIOB, EN);     // EN拉高
    GPIO_ResetBits(GPIOB, EN);   // EN拉低
}

/***********************************写入单个Byte函数********************************************/

/***********************************读忙函数********************************************/
void ReadBusy(void)
{ // 读忙函数，读忙之前记得更改引脚的工作方式！！！因为STM32的IO不是准双向IO
    GPIO_Write(GPIOA, 0x00ff);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;             // 选定GPIOA的第七Pin
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 第七Pin的工作方式为浮空输入模式，用于检测LCD1602的忙状态
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, RS); // RS拉低
    GPIO_SetBits(GPIOB, RW);   // RW拉高  RS=0 RW=1，读取Busy标志

    GPIO_SetBits(GPIOB, EN); // 使能开
    while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7))
        ;                      // 读第七Pin状态，如果一直为1则循环等待
    GPIO_ResetBits(GPIOB, EN); // 使能关

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
                                  GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 |
                                  GPIO_Pin_6 | GPIO_Pin_7; // 使GPIOA的状态还原成推挽模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/***********************************读忙函数********************************************/

/***********************************设置显示光标位置********************************************/

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

/***********************************设置显示光标位置********************************************/

/***********************************显示字符函数********************************************/

void LCD_ShowChar(unsigned char Line, unsigned char Columu, unsigned char Char)
{
    LCD_SetCursor(Line, Columu);

    LCD_WRITE_ByteDATA(Char);
}

/***********************************显示字符函数********************************************/

/***********************************显示字符串函数********************************************/

void LCD_ShowString(unsigned char Line, unsigned char Columu, unsigned char *String)
{
    unsigned char i = 0;

    LCD_SetCursor(Line, Columu);

    for (i = 0; String[i] != '\0'; i++)
    {
        LCD_WRITE_ByteDATA(String[i]);
    }
}

/***********************************显示字符串函数********************************************/

/***********************************获取次方数********************************************/

int LCD_Pow(unsigned char x, unsigned char y) // 返回 x 的 y 次幂
{
    int result = 1;
    unsigned char i = 0;

    for (i = 0; i < y; i++)
    {
        result *= x;
    }

    return result;
}

/***********************************获取次方数********************************************/

/***********************************显示数字函数********************************************/

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

    char buffer[16];                         // 数字转换为字符串后的缓存区
    int length = sprintf(buffer, "%u", Num); // 将数字转换为字符串
    for (int i = 0; i < length; i++)
    {
        LCD_WRITE_ByteDATA(buffer[i]); // 依次显示每一位数字
    }
}

void LCD_ShowNum_Y(unsigned int Num)
{

    char buffer[16];                         // 数字转换为字符串后的缓存区
    int length = sprintf(buffer, "%u", Num); // 将数字转换为字符串
    for (int i = 0; i < length; i++)
    {
        LCD_WRITE_ByteDATA(buffer[i]); // 依次显示每一位数字
    }
}

/***********************************显示数字函数********************************************/

/***********************************写入字符串函数********************************************/
/*
void LCD_WRITE_StrDATA( unsigned char *StrData, unsigned char row, unsigned char col ){//写入字符串
    unsigned char baseAddr = 0x00;			//定义地址
    if ( row ){
        baseAddr = 0xc0;
    }else{
        baseAddr = 0x80;
    } 	//row为1用户选择第二行
        //row为0用户选择第一行
    baseAddr += col;

    while ( *StrData != '\0' ){

        LCD_WRITE_CMD( baseAddr );
        LCD_WRITE_ByteDATA( *StrData );

        baseAddr++;			   //每次循环地址加一，数据指针加一
        StrData++;
    }
}
*/
/***********************************写入字符串函数********************************************/
