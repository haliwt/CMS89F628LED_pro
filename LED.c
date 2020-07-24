#include <cms.h>
#include "LED.h"
/**********************************************************
 	*
	*函数名称：void LEDDisplay_Data(void)
	*函数功能：显示数据
	*入口参数：无
	*出口参数：无 
	*备    注：
	*
**********************************************************/
void LEDDisplay_Data(void)
{
	asm("clrwdt");
	
	//运行显示,2位9段	
	
	if(getMinute ==60 ){
			 	getHour++;
			 	DispData[1] = seg[getHour % 10]; //SMG_0;		//COM0显示0
	            DispData[0] = seg[getHour /10];//SMG_1;		//COM1显示1
	            PoutPwm=0;                                 //打开时间小时，小数点LED灯
	            if(DispData[1] !=1 || DispData[1] !=4 || DispData[1]!=7){

	            		DispData[1]=SMG_A1 ;
	            		DispData[1]=SMG_D1 ;
	            }
	             if(DispData[0] !=1 || DispData[0] !=4 || DispData[0]!=7){

	            		DispData[0]=SMG_A1 ;
	            		DispData[0]=SMG_D1 ;
	            }
	            if(DispData[1]==7)DispData[1]=SMG_A1 ;
	            if(DispData[0]==7)DispData[0]=SMG_A1 ;
	            getMinute =0;
	            if(getHour ==100) getHour =0;
	}
	else if(getHour < 1){
		PoutPwm=1;  
		DispData[1] = seg[getMinute % 10];   //低位置
        DispData[0] = seg[getMinute/10];     //高位置
    }
		
		
	Led_Moudle_Device();		//将数据写入LEDDATA

}
/**********************************************************
 	*
	*函数名称：Set_LED_Moudle
	*函数功能：初始化LED/LCD模块设置
	*入口参数：无
	*出口参数：无 
	*备    注：
	*
**********************************************************/
void Set_LED_Moudle()
{
	LEDCON0 = (0x40 | FRENQUENCY);//使能LED,禁止LCD,设置频率
	LEDCON0 |= ((C_COMSEL_COM & 0x03) << 4); //选择COM口数量，6个
	COMEN =  C_LED_COM;   //LED驱动COM口设置-使用COM设置成1，GPIO 口设置 = 0
	SEGEN0 = C_LED_SEG0; //0--对应普通 GPIO (x=0~7) , 1--对应SEGx是LED/LCD SEG口(x=0~7)
	SEGEN1 = C_LED_SEG1; //0--对应普通 GPIO (x=8~15) , 1--对应SEGx是LED/LCD SEG口(x=8~15)
	SEGEN2 = C_LED_SEGCUR; //LED/LCD驱动电流寄存器配置
}
/**********************************************************
    *
	*函数名称：Led_Moudle_Device
	*函数功能：更新LED模块内部所有用到的显示LEDDATA的数据
	*入口参数：无
	*出口参数：无 
	*备    注：
	*
**********************************************************/
void Led_Moudle_Device()
{
	//设置LED内部显示RAM的值
	Set_Addr_Value(C_LED_SEGA|0X80, 0x01);//设置SEGA
	#if SEG_NUMBER >=2
	Set_Addr_Value(C_LED_SEGB|0X80, 0x02);//设置SEGB
	#endif
	#if SEG_NUMBER >=3
	Set_Addr_Value(C_LED_SEGC|0X80, 0x04);//设置SEGC
	#endif
	#if SEG_NUMBER >=4
	Set_Addr_Value(C_LED_SEGD|0X80, 0x08);//设置SEGD
	#endif
	#if SEG_NUMBER >=5
	Set_Addr_Value(C_LED_SEGE|0X80, 0x10);//设置SEGE
	#endif
	#if SEG_NUMBER >=6
	Set_Addr_Value(C_LED_SEGF|0X80, 0x20);//设置SEGF
	#endif
	#if SEG_NUMBER >=7
	Set_Addr_Value(C_LED_SEGG|0X80, 0x40);//设置SEGG
	#endif
	#if SEG_NUMBER >=8
	Set_Addr_Value(C_LED_SEGP|0X80, 0x80);//设置SEGP
	#endif
	#if SEG_NUMBER >=9
	Set_Addr_Value(C_LED_SEGH|0X80, 0x100);//设置SEGF
	#endif
	#if SEG_NUMBER >=10
	Set_Addr_Value(C_LED_SEGI|0X80, 0x200);//设置SEGG
	#endif
	#if SEG_NUMBER >=11
	Set_Addr_Value(C_LED_SEGJ|0X80, 0x400);//设置SEGP
	#endif
	
	LEDCON1 = 0x20;						//SEG口输出LCDDATA的数据
}

/**********************************************************
 * 	*
	*函数名称：Set_Addr_Value
	*函数功能：更新LED模块内部某个显示LEDDATA的数据
	*入口参数：Addr - 需要更新的地址
	*		Mask - 数据掩码
	*出口参数：
	*备    注：
**********************************************************/
void Set_Addr_Value(unsigned char Addr, unsigned int Mask)
{
	unsigned char data = 0;
	LEDADD = Addr | ((C_COMSEL_COM & 0x04) << 3);

	if(DispData[0] & Mask)
		BSET(data, C_LED_COM0);
	#if COM_NUMBER >=2
	if(DispData[1] & Mask)
		BSET(data, C_LED_COM1);
	#endif
	#if COM_NUMBER >=3
	if(DispData[2] & Mask)
		BSET(data, C_LED_COM2);
	#endif
	#if COM_NUMBER >=4
	if(DispData[3] & Mask)
		BSET(data, C_LED_COM3);
	#endif
	#if COM_NUMBER >=5
	if(DispData[4] & Mask)
		BSET(data, C_LED_COM4);
	#endif
	#if COM_NUMBER >=6
	if(DispData[5] & Mask)
		BSET(data, C_LED_COM5);
	#endif
	#if COM_NUMBER >=7
	if(DispData[6] & Mask)
		BSET(data, C_LED_COM6);	
	#endif
	LEDDATA = data;
}
/**********************************************************
 * 	*
	*函数名称：void LEDDisplay_TimerTim(void)
	*函数功能：定时时间显示
	*入口参数：NO
	*出口参数：NO
	*
**********************************************************/
void LEDDisplay_TimerTim(void)
{
	 //定时显示，4位7段
    if(Telec.showtimes<=60 && Telec.getTimerHour < 1){
        if(Telec.showtimes ==60)Telec.getTimerHour++;
       	DispData[5] = seg[Telec.showtimes %10];//SMG_2;		//COM2显示2
       	DispData[4] = seg[Telec.showtimes /10];//SMG_3;		//COM3显示3
       	DispData[3] = seg[0];//SMG_4;       //COM4显示4
       	DispData[2] = seg[0]; //SMG_5;      //COM2显示2---显示最高位时间，定时最大时间24小时

    }
    else if(Telec.getTimerHour >=1){
        if(Telec.showtimes ==60)Telec.getTimerHour++;
        DispData[5] = seg[Telec.showtimes %10];//SMG_2;		//COM2显示2
        DispData[4] = seg[Telec.showtimes /10];//SMG_3;		//COM3显示3
        DispData[3] = seg[Telec.getTimerHour % 10];//SMG_4;       //COM4显示4
        DispData[2] = seg[Telec.getTimerHour / 10]; //SMG_5;      //COM2显示2---显示最高位时间，定时最大时间24小时
        if(Telec.getTimerHour >24)Telec.getTimerHour =0;  //最大定时时间是 24小时
    }

   	Led_Moudle_Device();		//将数据写入LEDDATA

}

