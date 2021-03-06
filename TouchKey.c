#include <cms.h>
#include "TouchKey.h"
unsigned int TimerEvent;
/*----------------------------------------------------------------------------
	* 
	*Function Name: void Delay(uint16 ms)
	*Function: delay times 1us ,1/8 *4 = 0.5us double instruct 
	*Input Ref:NO
	*Output Ref:NO
	*                                                 
----------------------------------------------------------------------------*/
void Delay_1us(uint16_t ms)
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
	//LED SEG 驱动选择
	TRISA = 0x01;//0x65; WT.EDIT.20200722 0x0000 0001 GPIO-A0 --输入
	PORTA = 0x00;

	TRISB = 0x7F; //WT.EDIT.2020.07.22 0b0111 1111 --GPIO -B6 -触摸按键输入
	TRISC = 0x00; //WT.EDIT.2020.07.22 add new item

	PORTC = 0x00; //GPIO 口方向输出
	PORTC = 0x00; //设置GPIO 输出，LED驱动功能

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
	PR2= 13;//PR2 = 250;	//8M下将TMR2设置为125us中断 定时时间T2 = {1/[(Fosc)*预分频比*后分频比]}*PR2
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
	
	//LED SEG 驱动选择
	TRISA = 0x01;//0x65; WT.EDIT.20200722 0x0000 0001 GPIO-A0 --输入
	PORTA = 0x00;

	TRISB = 0x7F; //WT.EDIT.2020.07.22 0b0111 1111 --GPIO -B7 -触摸按键输入
	TRISC = 0x00; //WT.EDIT.2020.07.22 add new item

	PORTC = 0x00; //GPIO 口方向输出
	PORTC = 0x00; //设置GPIO 输出，LED驱动功能

	TRISD = 0;
	PORTD = 0;
	
	
	//刷新中断相关控制寄存器
	PIE2 = 0;
	PIE1 = 0x02; //peripheral interrupt enable 1 eable TIMER2 and PIR2 matching interrupt.
	PR2= 13;//T=26us,T=13us HIGH T=13us LOW level//WT.EDIT 38KHZ modulation to carrier,WT.EDIT.2020.07.22 
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
	static uint8_t tkflag=0,timflg =0,runflg=0,sterflg=0;
	uint8_t subutton=0;
	unsigned int i = (unsigned int)((KeyFlag[1]<<8) | KeyFlag[0]);
	if(i)
	{
		if(i != KeyOldFlag)
		{
			KeyOldFlag = i;
			switch(i)
			{
				case 0x1: //定时
				    TimerEvent=0;
					Telec->timerq=1;
                    Telec->showtimes= 10+Telec->showtimes;//每次增加 10分钟
                    LEDDisplay_TimerTim();
                break;

				case 0x2: //向上调节
				     if(subutton==0)subutton=1;
				     if(subutton==1){
				     	  Telec->setWind_levels++;
				     	if(Telec->setWind_levels >=6)
				   	      Telec->setWind_levels =5;
				     	  subutton = 2;
				     }
				   
				break;

				case 0x4: //向下调节
				 	if(subutton==0)subutton=1;
				     if(subutton==1){
				     	  Telec->setWind_levels--;
				       if(Telec->setWind_levels <=0)
				   	      Telec->setWind_levels =1;
				     	 subutton = 2;
				     }
				break;

				case 0x8: //runs
				    runflg = runflg ^ 0x01;
					if(runflg==1) Telec->runstart=1;
					else Telec->runstart=0;

				break;

				case 0x10: //set Timer value
				     if(subutton==0)subutton=1;
				     if(subutton ==1){
						Telec->setTimerValue = Telec->setTimerValue +10;
				     	if(Telec->setTimerValue >=240)
				     	   Telec->setTimerValue=0;
				     	subutton = 2;
				     }
				break;

				case 0x20: //KILL
					 sterflg = sterflg ^ 0x01;
					if(sterflg==1) Telec->sterilize=1;
					else Telec->sterilize=0;

				break;

				case 0x40:
				  tkflag = tkflag ^ 0x01;
				  if(tkflag ==1){
				  	 PortMos =1; //开启按键背光,通知主控制板，开启电源
				  	 Telec->power_state =1;
				  }
				  else{
				  	PortMos =0;
				  	Telec->power_state =0;
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
	*Function Name: USART_SendData(uint8_t data)
	*Function : GPIO口模拟串口，波特率 =9600bps ，间隔发送时间= 1s/9600=104us
	*Input Ref: data ，需要发送的数据32bit
	*Output Ref:No
	*
**************************************************************************/
void USART_SendData(uint8_t *arr)
{
	static uint8_t interflag;
	uint8_t i,pro=0;
    uint8_t bcc_data;
    //CRC_data = CRC8(arr, 3);
	bcc_data=BCC(arr,3);
	arr[3]=bcc_data;
	Telec->get_8_microsecond = 0; //定时器计时值，清零。
	PortTx =0;
	while(pro==0){
		if(Telec->get_8_microsecond==8){ //延时104us
			Telec->get_8_microsecond=0;
			interflag ++ ;
			if(interflag >=1 && interflag <=32){ //发送4个字节32bit
				
				    if(interflag <=8){
						PortTx =arr[0] & 0x01; //发送最低字节 head code 
						arr[0] = arr[0] >> 1;
					}
					else if(interflag >8 && interflag <=16){
						PortTx =arr[1] & 0x01; //发送最低字节  wind speed code hight 
						arr[1] = arr[1] >> 1;
					}
					else if(interflag >16 && interflag <= 24){
						PortTx =arr[2] & 0x01; //发送最低字节  wind speed code hight 
						arr[1] = arr[2] >> 1;
					}
					else if(interflag >24 && interflag <=32){
						PortTx =arr[3] & 0x01; //发送最低字节   crc value
						arr[1] = arr[3]>> 1;
					}
			}
			if(interflag ==33){ //停止位
				PortTx = 1;
			}
			if(interflag ==34){//发送完成
			   Telec->get_8_microsecond = 0; 
			   interflag =0;
			   pro =1;
			}

		}
	}
}
/*************************************************************************
 	*
	*Function Name: uint8_t BCC(uint8_t *sbyte,uint8_t width); 
    *Input Ref:be used to send data  
			   witdth :数据的字节数
	*Return Ref: crc,send data 
	*
**************************************************************************/
#if 0
uint8_t CRC8(uint8_t *ptr, uint8_t num)
{
   uint8_t cacbit ; //bit mask;
   uint8_t crc = 0XFF;  //calculaed checksum
   uint8_t byteCtr ; //byte counter
   
   //calculate 8 bit checksum with given polynomial 
   for(byteCtr =0; byteCtr<num; byteCtr++)
   {
		crc ^=(arr[byteCtr]);
		for(cacbit=8;cacbit>0;--cacbit) //8bit 
		{
		  if(crc & 0x80) 
		  	crc =(crc<<1) ^ POLY;
		  else
			crc = (crc <<1); // 高位零不作比较，直接移位
		}
   
   }

   return crc;
    
}
#endif 
uint8_t BCC(uint8_t *sbytes,uint8_t width)
{
     uint8_t i;
	 uint8_t tembyte = sbytes[0];
    for (i = 1; i <width; i++) {
        tembyte ^= sbytes[i];
    }
    return tembyte;

}
