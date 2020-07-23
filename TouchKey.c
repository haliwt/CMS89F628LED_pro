#include <cms.h>
#include "TouchKey.h"

/**********************************************************
	*
	*函数名称：Init_Systim()
	*函数功能：系统初始化
	*入口参数：无
	*出口参数：无 
	*备    注：每隔一定时间刷新一次SFR可增强抗干扰能力
	*
**********************************************************/
void Init_System()
{
	asm("nop");
	asm("clrwdt");
	INTCON = 0;				//禁止中断
	OSCCON = 0X71;			//配置振荡为8M
	OPTION_REG = 0;
	
	//延时等待电源电压稳定
	//DelayXms(200);
	TRISA = 0x01;//0x65; WT.EDIT.20200722
	TRISB = 0xEF; //WT.EDIT.2020.07.22
	TRISC = 0x00; //WT.EDIT.2020.07.22 add new item
	TRISD = 0;
	PORTD = 0;
	
	TIMER_KEY1 = 1;
	UP_KEY2 = 1;
	DOWN_KEY3 = 1;
	RUN_KEY4 = 1;
	SETUP_KEY5 = 1;
	KILL_KEY6 = 1;
	POWER_KEY7 = 1;

	
	PIE2 = 0;
	PIE1 = 0x02;
	PR2= 26;//PR2 = 250;	//8M下将TMR2设置为125us中断 定时时间T2 = {1/[(Fosc)*预分频比*后分频比]}*PR2
	T2CON = 4;				//使能定时器2
	
	INTCON = 0XC0;			//使能中断
}


/**********************************************************
	*
	*函数名称：Refurbish_Sfr
	*函数功能：刷新一些特殊功能寄存器
	*入口参数：无
	*出口参数：无 
	*备    注：每隔一定时间刷新一次SFR可增强抗干扰能力
	*
**********************************************************/
void Refurbish_Sfr()
{
	//均为数字口
//	ANSEL = 0;
//	ANSELH = 0;
	
	TRISA = 0x01; // 0;//x65; WTEDIT.20200722
	TRISB = 0xEF;//0xFF; //WT.EDIT 2020-07-22
	TRISC = 0;//0xFF;//WT.EDIT 2020-07-22
	TRISD = 0;
	
	SSPCON = 0;
	EECON1 = 0;
	
	OPTION_REG = 0;
	WDTCON = 9;
	
	
	//刷新中断相关控制寄存器
	PIE2 = 0;
	PIE1 = 0x02; //peripheral interrupt enable 1 eable TIMER2 and PIR2 matching interrupt.
	PR2= 26;//PR2 = 250;//WT.EDIT 38KHZ modulation to carrier,WT.EDIT.2020.07.22 
	INTCON = 0XC0;
	if(4 != T2CON)
		T2CON = 4;
}
/***********************************************************
 	*
	*Function Name: keyServer()
	*Function : 按键处理函数
	*Input Ref:No
	*Output Ref:No
	*
***********************************************************/
void KeyServer()
{
	static unsigned int KeyOldFlag = 0;
	unsigned int i = (unsigned int)((KeyFlag[1]<<8) | KeyFlag[0]);
	if(i)
	{
		if(i != KeyOldFlag)
		{
			KeyOldFlag = i;
			switch(i)
			{
				case 0x1:
				break;
				case 0x2:
				break;
				case 0x4:
				break;
				case 0x8:
				break;
				case 0x10:
				break;
				case 0x20:
				break;
				case 0x40:
				break;

			}
		}
	}
	else
	{
		KeyOldFlag = 0;
	}
}
