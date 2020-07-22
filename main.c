/*-------------------------------------------

�������ƣ�CMS89FT628���ʾ������
���ڰ汾��2018/6/15 <V1.1>

��ע��

*�������� ��΢�뵼�����޹�˾ &Ӧ��֧�ֲ�& ��д����
*��˾��ַ www.mcu.com.cn
*����֧��QQ 3001082102  �޹�
            3001085706	�¹�
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
	*�������ƣ�Init_Systim()
	*�������ܣ�ϵͳ��ʼ��
	*��ڲ�������
	*���ڲ������� 
	*��    ע��ÿ��һ��ʱ��ˢ��һ��SFR����ǿ����������
	*
**********************************************************/
void Init_System()
{
	asm("nop");
	asm("clrwdt");
	INTCON = 0;				//��ֹ�ж�
	OSCCON = 0X71;			//������Ϊ8M
	OPTION_REG = 0;
	
	//��ʱ�ȴ���Դ��ѹ�ȶ�
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
	PR2 = 250;				//8M�½�TMR2����Ϊ125us�ж� ��ʱʱ��T2 = {1/[(Fosc)*Ԥ��Ƶ��*���Ƶ��]}*PR2
	T2CON = 4;				//ʹ�ܶ�ʱ��2
	
	INTCON = 0XC0;			//ʹ���ж�
}


/**********************************************************
	*
	*�������ƣ�Refurbish_Sfr
	*�������ܣ�ˢ��һЩ���⹦�ܼĴ���
	*��ڲ�������
	*���ڲ������� 
	*��    ע��ÿ��һ��ʱ��ˢ��һ��SFR����ǿ����������
	*
**********************************************************/
void Refurbish_Sfr()
{
	//��Ϊ���ֿ�
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
	
	
	//ˢ���ж���ؿ��ƼĴ���
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
	*Function : ��������
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
	*Function : �жϷ�����
	*Input Ref:No
	*Output Ref:No
	*
***********************************************************/
void interrupt Isr_Timer()
{
	if(TMR2IF)				//��ֻʹ����һ���ж�Դ,������ȥ�ж�
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