/*-------------------------------------------

程序名称：CMS89FT628框架示例程序
日期版本：2018/6/15 <V1.1>

备注：

*本程序由 中微半导体有限公司 &应用支持部& 编写整理
*公司网址 www.mcu.com.cn
*技术支持QQ 3001082102  罗工
            3001085706	陈工
-------------------------------------------*/
#include <cms.h>
#include "Touch_Kscan_Library.h"

#define	LED_KEY1	RB0//RA4
#define	LED_KEY2	RB1//RA7
#define	LED_KEY3	RB2//RD2
#define	LED_KEY4	RB3//RD5
#define	LED_KEY5	RB4//RA3
#define	LED_KEY6	RB5//RD0
#define	LED_KEY7	RB6//RD3
//#define	LED_KEY8	RD6
//#define	LED_KEY9	RA1
//#define	LED_KEY10	RD1
//#define	LED_KEY11	RD4
//#define	LED_KEY12	RD7

//#define	DEBUG

volatile unsigned char MainTime;
volatile bit	B_MainLoop;
void Init_System(void);
void Refurbish_Sfr(void);
void KeyServer(void);

/***********************************************************
	*
	*Function Name: void mian()
	*Function : process
	*Input Ref:No
	*Output Ref:No
	*
***********************************************************/
void main()
{
#ifdef	DEBUG
	volatile unsigned char maxtime = 0,maxtime1 = 0;
#endif
	Init_System();
	while(1)
	{
		if(B_MainLoop)
		{
			B_MainLoop = 0;
			CLRWDT();
			
			CheckTouchKey();

#ifdef	DEBUG			
			maxtime1 = MainTime;
			if(maxtime1 > maxtime)
			{
				maxtime = maxtime1;
			}
#endif			
			
			KeyServer();
			Refurbish_Sfr();
		//	while(!(TKC0&0x40));
		}
	}
}
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
	TRISA = 0x05;//0x65; WT.EDIT.20200722
	TRISD = 0;
	PORTA = 0;
	PORTD = 0;
	
	LED_KEY1 = 1;
	LED_KEY2 = 1;
	LED_KEY3 = 1;
	LED_KEY5 = 1;
	LED_KEY6 = 1;
	LED_KEY7 = 1;
	//LED_KEY8 = 1;
	//LED_KEY10 = 1;
	//LED_KEY11 = 1;
	
	PIE2 = 0;
	PIE1 = 0x02;
	PR2 = 250;				//8M下将TMR2设置为125us中断 定时时间T2 = {1/[(Fosc)*预分频比*后分频比]}*PR2
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
	
	TRISA = 0x05; // 0;//x65; WTEDIT.20200722
	TRISB = 0x3F;//0xFF; //WT.EDIT 2020-07-22
	TRISC = 0;//0xFF;//WT.EDIT 2020-07-22
	TRISD = 0;
	
	SSPCON = 0;
	EECON1 = 0;
	
	OPTION_REG = 0;
	WDTCON = 9;
	
	
	//刷新中断相关控制寄存器
	PIE2 = 0;
	PIE1 = 0x02; //peripheral interrupt enable 1 eable TIMER2 and PIR2 matching interrupt.
	PR2 = 250;
	INTCON = 0XC0;
	if(4 != T2CON)
		T2CON = 4;
}


/***********************************************************
 	*
	*Function Name: keyServer()
	*Function : 键处理函数
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
				case 0x1:break;
				case 0x2:break;
				case 0x4:break;
				case 0x8:break;
				case 0x10:break;
				case 0x20:break;
				case 0x40:break;
				case 0x80:break;
				case 0x100:break;
				case 0x200:break;
				case 0x400:break;
				case 0x800:break;
			}
		}
	}
	else
	{
		KeyOldFlag = 0;
	}
}

/***********************************************************
	*
	*Function Name: interrupt Isr_Timer()
	*Function : 中断服务函数
	*Input Ref:No
	*Output Ref:No
	*
***********************************************************/
void interrupt Isr_Timer()
{
	if(TMR2IF)				//若只使能了一个中断源,可以略去判断
	{
		TMR2IF = 0;
		if(++MainTime >= 32)
		{
			MainTime = 0;
			B_MainLoop = 1;
		}
	}
	else
	{
		PIR1 = 0;
		PIR2 = 0;
	}
}