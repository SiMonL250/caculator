#include <stm32f10x.h>
#include "Delay.h"
#include "LCD1602.h"
#include "key.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define  uchar unsigned char
#define uint unsigned int
	
void readKey(void);
void caculate(uint dat1);
void display(float fin);

uchar inputs[] = {'0','1','2','3','4','5','6','7','8','9','=','/','*','-','+','.'};
uchar ansChar[4] = {'a','n','s','\0'};
float ans = 0;
int Num_1=0,Num_2=0;//two numbets beside the operator;
uint whichOp = 0, FLAG_X = 0;
static float Total = 0;
int flag_start = 0;//用于判断是否上一次按下的按键为“=”
int firstIsNum = 0;
int flagIsOp = 0;


int main()
{

    LCD_INIT();
    Key4x4_GPIO_Config();
    // LCD_ShowChar(1,1,'x');
    // LCD_ShowString(2,2,(unsigned char*)"Hello!");
    // LCD_ShowNum(1,1,16,0000000000000000);
    // LCD_WRITE_ByteDATA('2');
    //LCD_ShowNum_X(1, 1, 1);
	//LCD_ShowNum_X(1, 2, 1);//uasages of functions;
    while (1)
    {
		readKey();
    }
}

void readKey(){
	//uchar inputs[] = {'0','1','2','3','4','5','6','7','8','9','=','/','*','-','+','.'};
	unsigned int flag;
	int KeyValue;

	while( (KeyValue=(int)Key_Scan())==17 );//waiting key pressed
	if(flag_start == 1) LCD_WRITE_CMD(0x01);
	
	flag_start = 0;//若上次按下的按键为“=”则清空LCD屏幕，并将标志符flag_start重新置为0；
	if(KeyValue<10) { flag=1; }
	else {flag=2; } // two types : numbers and chars

	if(KeyValue>10) flagIsOp = 1;
	
	
	switch(flag){
		case 1:
			firstIsNum = 1;
			LCD_WRITE_ByteDATA(inputs[KeyValue]);
			if(whichOp==0)	
			{
				Num_1 = Num_1 * 10 + KeyValue;
				readKey();
			}
			if(whichOp>=1 && FLAG_X!=1)	
			{
				Num_2 = Num_2 * 10 + KeyValue;
				readKey();
			}
			
			break;
		case 2:
			if(KeyValue == 10 ) FLAG_X=1;//equals
			else if(KeyValue==11) {whichOp = 1; if(firstIsNum == 0 &&flagIsOp == 1) LCD_ShowString(1,1,ansChar);}//divide
			else if(KeyValue==12) {whichOp = 2; if(firstIsNum == 0 &&flagIsOp == 1) LCD_ShowString(1,1,ansChar);}//times
			else if(KeyValue==13) {whichOp = 3; if(firstIsNum == 0 &&flagIsOp == 1) LCD_ShowString(1,1,ansChar);}//minus
			else if(KeyValue==14) {whichOp = 4; if(firstIsNum == 0 &&flagIsOp == 1) LCD_ShowString(1,1,ansChar);}//plus
		
			caculate(KeyValue);
			break;
		

	}
	
}


void caculate(uint dat1){
	if(firstIsNum == 0 &&flagIsOp == 1) Num_1 = ans;
	
	switch(inputs[dat1]){
		case '/':
			LCD_WRITE_ByteDATA('/');
			readKey();
			if(whichOp==0) break;
			else if(whichOp>=1 && FLAG_X!=1) readKey();//not press '=' ,keep reading key
			else if(FLAG_X==1) caculate(10);//‘=’ //press '=' ,caculate 
			break;
		case '*':
			LCD_WRITE_ByteDATA('*');
			readKey();
			if(whichOp==0) break;
			else if(whichOp>=1 && FLAG_X!=1) readKey();
			else if(FLAG_X==1) caculate(10);//‘=’
			break;
		case '-':
			LCD_WRITE_ByteDATA('-');
			readKey();
			if(whichOp==0) break;
			else if(whichOp>=1 && FLAG_X!=1) readKey();
			else if(FLAG_X==1) caculate(10);//‘=’
			break;
		case '+':
			LCD_WRITE_ByteDATA('+');
			readKey();
			if(whichOp==0) break;
			else if(whichOp>=1 && FLAG_X!=1) readKey();
			else if(FLAG_X==1) caculate(10);//‘=’
			break;
		
		case '=':
 
			if( (Num_1>0||Num_1==0) && Num_2==0 )
			{
				Total = (float)Num_1;
				if(whichOp==1||whichOp==2)	Total = 0;
			}
			else if(Num_1==0 && Num_2!=0 && whichOp!=0 )
			{
				switch(whichOp)
				{
					default: Total = 0; break;
					case 3:  Total = (float)Num_1 - (float)Num_2;	break;
					case 4:  Total = (float)Num_1 + (float)Num_2; 	break;
				}
			}
			else if(Num_1!=0 && Num_2!=0 && whichOp!=0 )
			{
				switch(whichOp)
				{
					case 1:  Total = (float)Num_1 / (float)Num_2;	break;
					case 2:  Total = (float)Num_1 * (float)Num_2; 	break;
					case 3:  Total = (float)Num_1 - (float)Num_2;	break;
					case 4:  Total = (float)Num_1 + (float)Num_2; 	break;
				}
			}
			
			ans = Total;
			flag_start = 1;
			firstIsNum = 0;//reset flag 
			flagIsOp = 0;
			display(Total);
			break;

				
	}
}


void display(float fin)
{
	uint k;
	char buf[16];
	LCD_WRITE_CMD(0xc0);	//display in second line	
	LCD_WRITE_ByteDATA('=');
	sprintf(buf,"%.2f",fin);//reserve two demical to show 
	for(k=0;k<strlen(buf);k++)
		LCD_WRITE_ByteDATA(buf[k]);
	
	LCD_WRITE_CMD(0x80);//back to first line
	Num_1 = Num_2 = whichOp = FLAG_X = Total = 0;//initialize
	
}











