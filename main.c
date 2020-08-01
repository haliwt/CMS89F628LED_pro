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
//#include "Touch_Kscan_Library.h"
#include "LED.h"
#include "TouchKey.h"
#define TASK_NUM   (4)                  //  这里定义的任务数为4，表示有4个任务会使用此定时器定时。

 volatile uint16_t getMinute;
 volatile uint16_t getHour;

uint16_t TaskCount[TASK_NUM] ;           //  这里为4个任务定义4个变量来存放定时值
uint8_t  TaskMark[TASK_NUM];             //  同样对应4个标志位，为0表示时间没到，为1表示定时时间到。

//#define	DEBUG

struct _TASK_COMPONENTS
{
    uint8_t Run;                  // 程序运行标记：0-不运行，1运行
    uint16_t Timer;                // 计时器
    uint16_t ItervalTime;              // 任务运行间隔时间
    void (*TaskHook)(void);    // 要运行的任务函数
} TASK_COMPONENTS;             // 任务定义

typedef enum _TASK_LIST
{
    TAST_DISP_NUMBER,          // 显示数字
    TAST_KEY_SAN,             // 按键扫描
    TASK_RECE_IR,             // 接收IR
    TASK_TELEC_WS,            // 同控制主板通讯
    TASKS_MAX                 // 总的可供分配的定时任务数目
} TASK_LIST;

 uint8_t ptpwm_flag=0;

void TaskLEDDisplay(void);
void TaskKeySan(void);
void TaskReceiveIR(void);
void TaskTelecStatus(void);
void TaskProcess(void);

static struct _TASK_COMPONENTS TaskComps[] =
{
    {0, 769, 769, TaskLEDDisplay},           // 显示数字 20ms = 13us * 1538，扫描一次
    {0, 154, 154, TaskKeySan},               // 按键扫描 4ms=13us * 308 扫描一次
    {0, 308, 308, TaskReceiveIR},            // 接收IR   8ms = 13us * 616 执行一次
    {0, 384, 384, TaskTelecStatus}           // 同主板通讯 10ms = 13us * 160  执行一次 
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
	Set_LED_Moudle();
	while(1)
	{
		TaskProcess();
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
	uint8_t i;
    for (i=0; i<TASKS_MAX; i++)           // 逐个任务轮询时间处理
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
   LEDDisplay_Data();

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
  if(Pin0==0){//接收到IR信号,就把背光全部打开

		PortMos =1;
  }

}
/***********************************************************************************************
	*
	*Function Name: void TaskTelecStatus(void)
	*Function : ommunicate serial baud rate =9600bps
	*Input Ref:No
	*Output Ref:No
	*
*************************************************************************************************/
void TaskTelecStatus(void)
{
    uint8_t data[4];       // 待检查数据
    
   Telec->setWind_levels |=Telec->setWind_levels <<0; //风扇5级参数值
    
	Telec->power_state |= Telec->power_state << 0;       //电源开关量
	Telec->sterilize  |=Telec->sterilize<<1;           //杀菌开关量
    Telec->runstart  |=Telec->runstart<<2;             //电机开启开关量

	data[0]=Telec->power_state |Telec->sterilize|Telec->runstart;  //head code 8bit
	data[1]=0x0; 												//wind speed of code hig code 8bit
	data[2]=Telec->setWind_levels;								//wind speed of code low code 8 bit
	
   
    USART_SendData(data);
}
/*************************************************************************************
	*
	*Function Name: interrupt Isr_Timer()
	*Function : 中断服务函数 13us中断
	*Input Ref:No
	*Output Ref:No
	*
*****************************************************************************************/
void interrupt Isr_Timer()
{
	static uint16_t seconds=0,minutes=0;
	uint8_t i;
	if(TMR2IF)				//若只使能了一个中断源,可以略去判断
	{
	  TMR2IF = 0;
	  seconds++;
	  Telec->get_8_microsecond++;
	  ptpwm_flag=ptpwm_flag^0x1;
  	  if(ptpwm_flag==1)
  	  {
  	  	PortPwm =1;
  	  }
  	  else
  	  {
  	  	PortPwm =0 ;
  	  }
	  
	  for (i=0; i<TASKS_MAX; i++)          // 逐个任务轮询时间处理
	  {
	        if (TaskComps[i].Timer)          // 时间不为0
	        {
	            TaskComps[i].Timer--;         // 减去一个节拍
	            if (TaskComps[i].Timer == 0)       // 时间减完了
	            {
	                 TaskComps[i].Timer = TaskComps[i].ItervalTime;       // 恢复计时器值，从新下一次
	                 TaskComps[i].Run = 1;           // 任务可以运行
	            }
	        }
		}

		if(seconds==65535){ //计时：852ms//1.7s
			seconds =0;
			 minutes ++;
			if(minutes ==71){ //1分钟时间
				minutes =0;
			    getMinute++; 
		    }
			
		}
		
	}
	else
	{
		PIR1 = 0;
		PIR2 = 0;
	}
}