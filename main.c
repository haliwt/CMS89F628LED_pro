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

#define TASK_NUM   (4)                  //  ���ﶨ���������Ϊ3����ʾ�����������ʹ�ô˶�ʱ����ʱ��
typedef  unsigned char uint8;
typedef  unsigned int  uint16;
uint16 TaskCount[TASK_NUM] ;           //  ����Ϊ����������������������Ŷ�ʱֵ
uint8  TaskMark[TASK_NUM];             //  ͬ����Ӧ������־λ��Ϊ0��ʾʱ��û����Ϊ1��ʾ��ʱʱ�䵽��

//#define	DEBUG

struct _TASK_COMPONENTS
{
    uint8 Run;                  // �������б�ǣ�0-�����У�1����
    uint8 Timer;                // ��ʱ��
    uint8 ItvTime;              // �������м��ʱ��
    void (*TaskHook)(void);    // Ҫ���е�������
} TASK_COMPONENTS;             // ������

typedef enum _TASK_LIST
{
    TAST_DISP_CLOCK,           // ��ʾʱ��
    TAST_KEY_SAN,             // ����ɨ��
    TASK_RECE_IR,             // ����IR
    TASK_TELEC_WS,            // ͬ��������ͨѶ
    TASKS_MAX                                           // �ܵĿɹ�����Ķ�ʱ������Ŀ
} TASK_LIST;

volatile unsigned char MainTime;
volatile bit	B_MainLoop;

void Init_System(void);
void Refurbish_Sfr(void);
void KeyServer(void);
void TaskLEDDisplay(void);
void TaskKeySan(void);
void TaskReceiveIR(void);
void TaskTelecStatus(void);
void TaskProcess(void);

static struct _TASK_COMPONENTS TaskComps[] =
{
    {0, 60, 60, TaskLEDDisplay},         // ��ʾʱ��
    {0, 20, 20, TaskKeySan},               // ����ɨ��
    {0, 30, 30, TaskReceiveIR},           // ����IR
    {0, 50, 50, TaskTelecStatus}��         // ͬ����ͨѶ
};

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

	Init_System();
	while(1)
	{
		
		TaskProcess();
		#if 0
		if(B_MainLoop)
		{
			B_MainLoop = 0;
			CLRWDT();
			
			CheckTouchKey();

		
			
			KeyServer();
			Refurbish_Sfr();
		//	while(!(TKC0&0x40));
		}
		#endif 
	}
}
/***********************************************************
	*
	*Function Name: void TaskProcess(void)
	*Function : process
	*Input Ref:No
	*Output Ref:No
	*
***********************************************************/
void TaskProcess(void)
{
	uint8 i;
    for (i=0; i<TASKS_MAX; i++)           // �������ʱ�䴦��
    {
        if(TaskComps[i].Run)           // ʱ�䲻Ϊ0
        {
             TaskComps[i].TaskHook();         // ��������
             TaskComps[i].Run = 0;          // ��־��0
        }
    }

}
/***********************************************************
	*
	*Function Name: void TaskDisplayClock(void)
	*Function : display LED numbers
	*Input Ref:No
	*Output Ref:No
	*
***********************************************************/
void TaskLEDDisplay(void)
{


}
/***********************************************************
	*
	*Function Name: void TaskKeySan(void)
	*Function : Toch key scan 
	*Input Ref:No
	*Output Ref:No
	*
***********************************************************/
void TaskKeySan(void)
{
	CLRWDT();
	CheckTouchKey();
	KeyServer();
	Refurbish_Sfr();
}
/***********************************************************
	*
	*Function Name: void TaskReceiveIR(void)
	*Function : receive IR logic level
	*Input Ref:No
	*Output Ref:No
	*
***********************************************************/
void TaskReceiveIR(void)
{


}
/***********************************************************
	*
	*Function Name: void TaskTelecStatus(void)
	*Function : ommunicate with to mainboard
	*Input Ref:No
	*Output Ref:No
	*
***********************************************************/
void TaskTelecStatus(void)
{


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
	uint8 i;
	if(TMR2IF)				//��ֻʹ����һ���ж�Դ,������ȥ�ж�
	{
		TMR2IF = 0;
		for (i=0; i<TASKS_MAX; i++)          // �������ʱ�䴦��
		{
	        if (TaskComps[i].Timer)          // ʱ�䲻Ϊ0
	        {
	            TaskComps[i].Timer--;         // ��ȥһ������
	            if (TaskComps[i].Timer == 0)       // ʱ�������
	            {
	                 TaskComps[i].Timer = TaskComps[i].ItvTime;       // �ָ���ʱ��ֵ��������һ��
	                 TaskComps[i].Run = 1;           // �����������
	            }
	        }
		}
		
	}
	else
	{
		PIR1 = 0;
		PIR2 = 0;
	}
}