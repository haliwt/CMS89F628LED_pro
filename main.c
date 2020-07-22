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

#define TASK_NUM   (4)                  //  这里定义的任务数为3，表示有三个任务会使用此定时器定时。
typedef  unsigned char uint8;
typedef  unsigned int  uint16;
uint16 TaskCount[TASK_NUM] ;           //  这里为三个任务定义三个变量来存放定时值
uint8  TaskMark[TASK_NUM];             //  同样对应三个标志位，为0表示时间没到，为1表示定时时间到。

//#define	DEBUG

struct _TASK_COMPONENTS
{
    uint8 Run;                  // 程序运行标记：0-不运行，1运行
    uint8 Timer;                // 计时器
    uint8 ItvTime;              // 任务运行间隔时间
    void (*TaskHook)(void);    // 要运行的任务函数
} TASK_COMPONENTS;             // 任务定义

typedef enum _TASK_LIST
{
    TAST_DISP_CLOCK,           // 显示时钟
    TAST_KEY_SAN,             // 按键扫描
    TASK_RECE_IR,             // 接收IR
    TASK_TELEC_WS,            // 同控制主板通讯
    TASKS_MAX                                           // 总的可供分配的定时任务数目
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
    {0, 60, 60, TaskLEDDisplay},         // 显示时钟
    {0, 20, 20, TaskKeySan},               // 按键扫描
    {0, 30, 30, TaskReceiveIR},           // 接收IR
    {0, 50, 50, TaskTelecStatus}，         // 同主板通讯
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
    for (i=0; i<TASKS_MAX; i++)           // 逐个任务时间处理
    {
        if(TaskComps[i].Run)           // 时间不为0
        {
             TaskComps[i].TaskHook();         // 运行任务
             TaskComps[i].Run = 0;          // 标志清0
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
	uint8 i;
	if(TMR2IF)				//若只使能了一个中断源,可以略去判断
	{
		TMR2IF = 0;
		for (i=0; i<TASKS_MAX; i++)          // 逐个任务时间处理
		{
	        if (TaskComps[i].Timer)          // 时间不为0
	        {
	            TaskComps[i].Timer--;         // 减去一个节拍
	            if (TaskComps[i].Timer == 0)       // 时间减完了
	            {
	                 TaskComps[i].Timer = TaskComps[i].ItvTime;       // 恢复计时器值，从新下一次
	                 TaskComps[i].Run = 1;           // 任务可以运行
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