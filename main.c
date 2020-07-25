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
//#include "Touch_Kscan_Library.h"
#include "LED.h"
#include "TouchKey.h"
#define TASK_NUM   (4)                  //  ���ﶨ���������Ϊ4����ʾ��4�������ʹ�ô˶�ʱ����ʱ��

 volatile uint16 getMinute;
 volatile uint16 getHour;

//typedef  unsigned char uint8;
//typedef  unsigned int  uint16;
uint16 TaskCount[TASK_NUM] ;           //  ����Ϊ4��������4����������Ŷ�ʱֵ
uint8  TaskMark[TASK_NUM];             //  ͬ����Ӧ4����־λ��Ϊ0��ʾʱ��û����Ϊ1��ʾ��ʱʱ�䵽��

//#define	DEBUG

struct _TASK_COMPONENTS
{
    uint8 Run;                  // �������б�ǣ�0-�����У�1����
    uint16 Timer;                // ��ʱ��
    uint16 ItvTime;              // �������м��ʱ��
    void (*TaskHook)(void);    // Ҫ���е�������
} TASK_COMPONENTS;             // ������

typedef enum _TASK_LIST
{
    TAST_DISP_NUMBER,          // ��ʾ����
    TAST_KEY_SAN,             // ����ɨ��
    TASK_RECE_IR,             // ����IR
    TASK_TELEC_WS,            // ͬ��������ͨѶ
    TASKS_MAX                 // �ܵĿɹ�����Ķ�ʱ������Ŀ
} TASK_LIST;

 uint8 ptpwm_flag=0;

void TaskLEDDisplay(void);
void TaskKeySan(void);
void TaskReceiveIR(void);
void TaskTelecStatus(void);
void TaskProcess(void);

static struct _TASK_COMPONENTS TaskComps[] =
{
    {0, 769, 769, TaskLEDDisplay},           // ��ʾ���� 20ms = 125us * 160��ɨ��һ��
    {0, 154, 154, TaskKeySan},               // ����ɨ�� 4ms=125us * 32 ɨ��һ��
    {0, 308, 308, TaskReceiveIR},            // ����IR   8ms = 125us * 64 
    {0, 384, 384, TaskTelecStatus}��         // ͬ����ͨѶ 10ms = 125us * 80   
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
    uint8 i, value;       // ���������
    uint8  parity = 0;  //��ʼ���
   
    Telec.setWind_levels |=Telec.setWind_levels <<5;
    Telec.sterilize  |=Telec.sterilize<<4;
    Telec.power_state |= Telec.power_state<<3;
    Telec.runstart  |=Telec.runstart<<2;

    value =Telec.setWind_levels | Telec.sterilize |Telec.power_state|Telec.runstart;
    while (value)
    {
      parity = !parity;
      value = value & (value - 1);
    }
    if(parity ==1){
    	value =value | 0x01;
    }
    Telec.get_4_microsecond = 0; //��ʱ����ʱֵ�����㡣
    USART_SendData(value);
}
/***********************************************************
	*
	*Function Name: interrupt Isr_Timer()
	*Function : �жϷ����� 26us�ж�
	*Input Ref:No
	*Output Ref:No
	*
***********************************************************/
void interrupt Isr_Timer()
{
	static uint16 seconds=0,minutes=0;
	uint8 i;
	if(TMR2IF)				//��ֻʹ����һ���ж�Դ,������ȥ�ж�
	{
	  TMR2IF = 0;
	  seconds++;
	  Telec.get_4_microsecond++;
	  ptpwm_flag=ptpwm_flag^0x1;
  	  if(ptpwm_flag==1)
  	  {
  	  	PortPwm =1;
  	  }
  	  else
  	  {
  	  	PortPwm =0 ;
  	  }
	  
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

		if(seconds==65535){ //��ʱ��1.7s
			seconds =0;
		    minutes ++;
		   if(minutes ==35){
				minutes =0;
			    getMinute++; //һ����ʱ��
		    }
			
		}
		
	}
	else
	{
		PIR1 = 0;
		PIR2 = 0;
	}
}