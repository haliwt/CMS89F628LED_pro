#include <cms.h>
#include "TouchKey.h"

/*----------------------------------------------------------------------------
	* 
	*Function Name: void Delay(uint16 ms)
	*Function: delay times 1us ,1/8 *4 = 0.5us double instruct 
	*Input Ref:NO
	*Output Ref:NO
	*                                                 
----------------------------------------------------------------------------*/
void Delay_1us(uint16 ms)
{
  for(;ms>0;ms--)
    asm("nop");
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
	TRISA = 0x01;//0x65; WT.EDIT.20200722
	TRISB = 0xEF; //WT.EDIT.2020.07.22
	TRISC = 0x00; //WT.EDIT.2020.07.22 add new item
	TRISD = 0;
	PORTD = 0;

	LED_KEY1 = 1; //TIMER
	LED_KEY2 = 1; //UP
	LED_KEY3 = 1; //DOWN
	LED_KEY4 = 1; //RUN
	LED_KEY5 = 1; //SETUP
	LED_KEY6 = 1; //KILL
	LED_KEY7 = 1; //POWER
	
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
	static uint8 tkflag=0,timflg =0,runflg=0,sterflg=0;
	uint8 subutton=0;
	unsigned int i = (unsigned int)((KeyFlag[1]<<8) | KeyFlag[0]);
	if(i)
	{
		if(i != KeyOldFlag)
		{
			KeyOldFlag = i;
			switch(i)
			{
				case 0x1: //定时
					timflg = timflg ^ 0x01;
					 if(timflg==1){
                         Telec.timerq=1;
                         Telec.showtimes= 10+Telec.showtimes;//每次增加 10分钟
                         LEDDisplay_TimerTim();
                    }
					else{
                         Telec.timerq=1; //第二次按定时按键，是减少
                         Telec.showtimes= Telec.showtimes -10;//每次减少 10分钟
                         if(Telec.showtimes <=0)Telec.showtimes=0;
                         LEDDisplay_TimerTim();
                    }

				break;

				case 0x2: //向上调节
				     if(subutton==0)subutton=1;
				     if(subutton==1){
				     	  Telec.setWind_levels++;
				     	if(Telec.setWind_levels >=6)
				   	      Telec.setWind_levels =5;
				     	  subutton = 2;
				     }
				   
				break;

				case 0x4: //向下调节
				 	if(subutton==0)subutton=1;
				     if(subutton==1){
				     	  Telec.setWind_levels--;
				       if(Telec.setWind_levels <=0)
				   	      Telec.setWind_levels =1;
				     	 subutton = 2;
				     }
				break;

				case 0x8: //runs
				    runflg = runflg ^ 0x01;
					if(runflg==1) Telec.runstart=1;
					else Telec.runstart=0;

				break;

				case 0x10: //set Timer value
				     if(subutton==0)subutton=1;
				     if(subutton ==1){
						Telec.setTimerValue = Telec.setTimerValue +10;
				     	if(Telec.setTimerValue >=240)
				     	   Telec.setTimerValue=0;
				     	subutton = 2;
				     }
				break;

				case 0x20: //KILL
					 sterflg = sterflg ^ 0x01;
					if(sterflg==1) Telec.sterilize=1;
					else Telec.sterilize=0;

				break;

				case 0x40:
				  tkflag = tkflag ^ 0x01;
				  if(tkflag ==1){
				  	 PortMos =1; //开启按键背光,通知主控制板，开启电源
				  	 Telec.power_state =1;
				  }
				  else{
				  	PortMos =0;
				  	Telec.power_state =0;
				  }
				break;

			}
		}
	}
	else
	{
		KeyOldFlag = 0;
	}
}
/*************************************************************************
 	*
	*Function Name: USART_SendData(uint8 data)
	*Function : GPIO口模拟串口，波特率 =9600bps ，间隔发送时间= 1s/9600=104us
	*Input Ref: data ，需要发送的数据8bit
	*Output Ref:No
	*
******************************************************************************/
void USART_SendData(uint8 data)
{
	static uint8 interflag;
	uint8 i,pro=0;
	PortTx =0;
	while(pro==0){
		if(Telec.get_4_microsecond==4){ //延时104us
			Telec.get_4_microsecond=0;
			interflag ++ ;
			if(interflag >=1 && interflag <=8){ //发送8个bit数据
				
					PortTx =data & 0x01; //发送最低字节
					data = data >> 1;
				
			}

			if(interflag ==9){ //停止位
				PortTx = 1;
			}
			if(interflag ==10){//发送完成
			   Telec.get_4_microsecond = 0; 
			   interflag =0;
			   pro =1;
			}

		}
	}
}
