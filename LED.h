#ifndef _LED_H_
#define _LED_H_
#include "TouchKey.h"

//用户可根据硬件情况修改SEG口电流及个数，修改SEG口和COM口对应点
/**************************************************
	*设置SEG口
**************************************************/
#define		C_COMSEL_COM	0x02//0x00 6//4个COM	//设置COMSEL，选择COM口个数，范围0x00~0x07，详情请查看芯片手册
#define     C_LED_SEGCUR    SEGCUR_2MA  //SEG口电流
#define		SEG_NUMBER		9//8        //SEG口个数
#define		COM_NUMBER		6//4		//COM口个数
#define		FRENQUENCY		Fosc_8192	//频率

volatile unsigned int DispData[COM_NUMBER];//显示数组，当使用的SEG口不大于8个时，可以只声明char型变量，并修改响应的函数声明参数类型。

//各个段对应的芯片驱动SEG口（根据SEG口个数设置前n个值）
//LED 显示对应芯片的SEG口，根据原理图设置
#if 0
#define		C_LED_SEGA		SEG0  //A段对应MCU的SEG口
#define		C_LED_SEGB		SEG1  //B段对应MCU的SEG口
#define		C_LED_SEGC		SEG2  //C段对应MCU的SEG口
#define		C_LED_SEGD		SEG3  //D段对应MCU的SEG口
#define		C_LED_SEGE		SEG4  //E段对应MCU的SEG口
#define		C_LED_SEGF		SEG5  //F段对应MCU的SEG口
#define		C_LED_SEGG		SEG6  //G段对应MCU的SEG口
#define		C_LED_SEGP		SEG7  //DP段对应MCU的SEG口
#define		C_LED_SEGH		SEG8  //A'段
#define		C_LED_SEGI		SEG9  //E'段
#define		C_LED_SEGJ		SEG10 //

#endif 
/**************************************************
	*设置COM口
**************************************************/
//各个位对应的芯片驱动COM口（根据COM口个数设置前n个值）
#define		C_LED_COM0		COM0  //COM0对应MCU的COM口--对应LED 运行显示的“十”位 位选地址
#define		C_LED_COM1		COM1  //COM1对应MCU的COM口--对应LED 运行显示的“个”位
#define		C_LED_COM2		COM2  //COM2对应MCU的COM口--对应定时 LED 显示“十”位
#define		C_LED_COM3		COM3  //COM3对应MCU的COM口--对应定时 LED 显示“个”位
#define		C_LED_COM4		COM4  //COM4对应MCU的COM口--对应定时 LED 小数点后的“十”位
#define		C_LED_COM5		COM5  //COM5对应MCU的COM口--对应定时 LED 小数点后的“个”位
#define		C_LED_COM6		0x00  //COM6  //COM6对应MCU的COM口


/**************************************************
	*XinTong LED Display Data Data:2020.07.23

	    //                a
        //            ---------
        //           |         |
        //         f |         | b
        //           |    g    |
        //            ---------
        //           |         |
        //         e |         | c
        //           |    d    |
        //            ---------   O <- h
**************************************************/

#define		C_LED_SEGA		SEG1   //A段对应MCU的SEG口，段选地址       
#define		C_LED_SEGB		SEG3   //B段对应MCU的SEG口
#define		C_LED_SEGC		SEG4   //C段对应MCU的SEG口
#define		C_LED_SEGD		SEG5   //D段对应MCU的SEG口
#define		C_LED_SEGE		SEG6   //E段对应MCU的SEG口
#define		C_LED_SEGF		SEG7   //F段对应MCU的SEG口
#define		C_LED_SEGG		SEG8   //G段对应MCU的SEG口
//运行显示模式
#define		C_LED_SEGP		SEG9   //A'段对应MCU的SEG口，段选位置
#define		C_LED_SEGH		SEG10  //D'段


extern volatile unsigned int getMinute;
extern volatile unsigned int  getHour;
extern unsigned int TimerEvent;
/***************************************************/
/***************************************************/
/***************************************************/
/**************************************************
LED模块常量定义,请勿修改
**************************************************/
#define     SEG0    0
#define     SEG1    1
#define     SEG2    2
#define     SEG3    3
#define     SEG4    4
#define     SEG5    5  //MCU的SEG口地址
#define     SEG6    6
#define     SEG7    7
#define     SEG8    8
#define     SEG9    9
#define     SEG10   10

#define     COM0    0
#define     COM1    1
#define     COM2    2
#define     COM3    3  //MCU的COM口地址
#define     COM4    4
#define     COM5    5
#define     COM6    6

#define     SEGCUR_2MA      0X10
#define     SEGCUR_4MA      0X20
#define     SEGCUR_6MA      0X30
#define     SEGCUR_8MA      0X40
#define     SEGCUR_10MA     0X50
#define     SEGCUR_12MA     0X60
#define     SEGCUR_14MA     0X70  //MCU的SEG口电流
#define     SEGCUR_16MA     0X80
#define     SEGCUR_18MA     0X90
#define     SEGCUR_20MA     0XA0
#define     SEGCUR_22MA     0XB0
#define     SEGCUR_24MA     0XC0
#define     SEGCUR_26MA     0XD0
#define     SEGCUR_28MA     0XE0
#define     SEGCUR_30MA     0XF0

#define		Fosc_64			0x00
#define		Fosc_128		0x01
#define		Fosc_256		0x02
#define		Fosc_512		0x03
#define		Fosc_1024		0x04
#define		Fosc_2048		0x05
#define		Fosc_4096		0x06
#define		Fosc_8192		0x07
#define		Fosc_16384		0x08
#define		Fosc_32768		0x09
#define		Fosc_65536		0x0a
#define		Fosc_131072		0x0b
/**************************************************
	*使能段选，和位选 I/O 选择 和GPIO选择
**************************************************/
#define     C_LED_COM       ((1<<C_LED_COM0)|(1<<C_LED_COM1)|(1<<C_LED_COM2)|(1<<C_LED_COM3)\
                            |(1<<C_LED_COM4)|(1<<C_LED_COM5)|(1<<C_LED_COM6))
#define     C_LED_SEG       ((1<<C_LED_SEGA)|(1<<C_LED_SEGB)|(1<<C_LED_SEGC)|(1<<C_LED_SEGD)|\
							(1<<C_LED_SEGE)|(1<<C_LED_SEGF)|(1<<C_LED_SEGG)|(1<<C_LED_SEGP)|(1<<C_LED_SEGH))

#define     C_LED_SEG0      C_LED_SEG&0XFF                             
#define     C_LED_SEG1      (C_LED_SEG>>8)&0XFF 


#define		BSET(x,y)		x|=(1<<y)				//置1
#define		BCLR(x,y)		x&=~(1<<y)				//置0
/**************************************************
**************************************************/
#define		SMG_0			0x3F	// 0		
#define		SMG_1			0x06	// 1		
#define		SMG_2			0x5B	// 2		
#define		SMG_3			0x4F	// 3		
#define		SMG_4			0x66	// 4		
#define		SMG_5			0x6D	// 5		
#define		SMG_6			0x7D	// 6		
#define		SMG_7			0x07	// 7		
#define		SMG_8			0x7F	// 8		
#define		SMG_9			0x6F	// 9		
#define		SMG_A			0x77	// A		
#define		SMG_B			0x7C	// B		
#define		SMG_C			0x39	// C		
#define		SMG_D			0x5E	// D		
#define		SMG_E			0x79	// E		
#define		SMG_F			0x71	// F

#define     SMG_A1          0x09    //
#define     SMG_D1          0x0A     //

const unsigned char seg[]={
         SMG_0,         // char "0"  0x00
         SMG_1,         // char "1"  0x01
         SMG_2,         // char "2"  0x02
         SMG_3,         // char "3"  0x03
         SMG_4,         // char "4"  0x04
         SMG_5,         // char "5"  0x05
         SMG_6,        	// char "6"  0x06
         SMG_7,         // char "7"  0x07
         SMG_8,  		// char "8"  0x08
         SMG_9       	// char "9"  0x09
        
};

void LEDDisplay_TimerTim(void);
void LEDDisplay_Data(void);
void Set_LED_Moudle();
void Led_Moudle_Device();
void Set_Addr_Value(unsigned char Addr, unsigned int Mask);

/**************************************************
**************************************************/
#endif
