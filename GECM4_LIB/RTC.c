/**
 ******************************************************************************
 * @file    RTC.c
 * @author  ajl
 * @version V0.0.1
 * @date    5-November-2025
 * @brief
 *
 ******************************************************************************
 * @attention
 *
 * 本文件是ADC的使用
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx.h"

static uint8_t count; // 1s扫描一次0.1s容易跑飞

RTC_TimeTypeDef RTC_TimeStruct; // 储存时间和日期的结构体
RTC_DateTypeDef RTC_DateStruct;

ErrorStatus RTC_Set_Time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;

	RTC_TimeTypeInitStructure.RTC_Hours = hour;
	RTC_TimeTypeInitStructure.RTC_Minutes = min;
	RTC_TimeTypeInitStructure.RTC_Seconds = sec;
	RTC_TimeTypeInitStructure.RTC_H12 = ampm;	// 12小时制还是24小时制

	return RTC_SetTime(RTC_Format_BIN, &RTC_TimeTypeInitStructure);
}
// RTC日期设置
// year,month,date:年(0~99),月(1~12),日(0~31)
// week:星期(1~7,0,非法!)
// 返回值:SUCEE(1),成功
//        ERROR(0),进入初始化模式失败
ErrorStatus RTC_Set_Date(uint8_t year, uint8_t month, uint8_t date, uint8_t week)
{

	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date = date;
	RTC_DateTypeInitStructure.RTC_Month = month;
	RTC_DateTypeInitStructure.RTC_WeekDay = week;
	RTC_DateTypeInitStructure.RTC_Year = year;
	return RTC_SetDate(RTC_Format_BIN, &RTC_DateTypeInitStructure);
}

// RTC初始化
// 返回值:0,初始化成功;
//        1,LSE开启失败;
//        2,进入初始化模式失败;
uint8_t My_RTC_Init(void)
{

	u16 retry = 0X1FFF;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); // 使能PWR时钟
	PWR_BackupAccessCmd(ENABLE);						// 使能后备寄存器访问

	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x5050) // 是否第一次配置?
	{
		RCC_LSEConfig(RCC_LSE_ON);							// LSE 开启
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) // 检查指定的RCC标志位设置与否,等待低速晶振就绪
		{
			retry++;
			Delay_ms(10);
		}
		if (retry == 0)
			return 1; // LSE 开启失败.

		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); // 设置RTC时钟(RTCCLK),选择LSE作为RTC时钟
		RCC_RTCCLKCmd(ENABLE);					// 使能RTC时钟

		RTC_InitTypeDef RTC_InitStructure;

		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;			  // RTC异步分频系数(1~0X7F)
		RTC_InitStructure.RTC_SynchPrediv = 0xFF;			  // RTC同步分频系数(0~7FFF)
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; // RTC设置为,24小时格式
		RTC_Init(&RTC_InitStructure);

		RTC_Set_Time(0, 59, 0, RTC_H12_AM); // 设置时间	表示上午时间
		RTC_Set_Date(25, 11, 6, 1);			// 设置日期

		RTC_WriteBackupRegister(RTC_BKP_DR0, 0x5050); //对bkp内存进行写入
	}

	return 0;
}

// 设置闹钟时间(按星期闹铃,24小时制)
// week:星期几(1~7) @ref  RTC_Alarm_Definitions
// hour,min,sec:小时,分钟,秒钟
void RTC_Set_AlarmA(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec)
{

	RTC_AlarmCmd(RTC_Alarm_A, DISABLE); // 关闭闹钟A


	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	RTC_TimeTypeInitStructure.RTC_Hours = hour;	 // 小时
	RTC_TimeTypeInitStructure.RTC_Minutes = min; // 分钟
	RTC_TimeTypeInitStructure.RTC_Seconds = sec; // 秒
	RTC_TimeTypeInitStructure.RTC_H12 = RTC_H12_AM;


	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay = week;								  // 星期
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay; // 按星期闹
	RTC_AlarmTypeInitStructure.RTC_AlarmMask = RTC_AlarmMask_None;						  // 精确匹配星期，时分秒
	RTC_AlarmTypeInitStructure.RTC_AlarmTime = RTC_TimeTypeInitStructure;
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmTypeInitStructure);

	RTC_ClearITPendingBit(RTC_IT_ALRA);	 // 清除RTC闹钟A的标志
	EXTI_ClearITPendingBit(EXTI_Line17); // 清除LINE17上的中断标志位

	RTC_ITConfig(RTC_IT_ALRA, ENABLE); // 开启闹钟A中断
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE); // 开启闹钟A


	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;			   // LINE17
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	   // 中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;			   // 使能LINE17
	EXTI_Init(&EXTI_InitStructure);						   // 配置

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; // 抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		 // 子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 // 使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);								 // 配置
}

// 周期性唤醒定时器设置
/*wksel:  @ref RTC_Wakeup_Timer_Definitions
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)		//RTC时钟/16
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)		//RTC时钟/8
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)		//RTC时钟/4	
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)		//RTC时钟/2
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)		//1Hz时钟
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)		//1Hz时钟
*/
// cnt:自动重装载值.减到0,产生中断.
void RTC_Set_WakeUp(uint32_t wksel, uint16_t cnt)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	RTC_WakeUpCmd(DISABLE); // 关闭WAKE UP

	RTC_WakeUpClockConfig(wksel); // 唤醒时钟选择

	RTC_SetWakeUpCounter(cnt); // 设置WAKE UP自动重装载寄存器   唤醒周期 = (cnt + 1) × 时钟周期

	RTC_ClearITPendingBit(RTC_IT_WUT);	 // 清除RTC WAKE UP的标志
	EXTI_ClearITPendingBit(EXTI_Line22); // 清除LINE22上的中断标志位

	RTC_ITConfig(RTC_IT_WUT, ENABLE); // 开启WAKE UP 定时器中断
	RTC_WakeUpCmd(ENABLE);			  // 开启WAKE UP 定时器　

	EXTI_InitStructure.EXTI_Line = EXTI_Line22;			   // LINE22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	   // 中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;			   // 使能LINE22
	EXTI_Init(&EXTI_InitStructure);						   // 配置

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; // 抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		 // 子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 // 使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);								 // 配置
}

// RTC闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{
	if (RTC_GetFlagStatus(RTC_FLAG_ALRAF) == SET) // ALARM A中断?
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF); // 清除中断标志
		// printf("ALARM A!\r\n");
	}
	EXTI_ClearITPendingBit(EXTI_Line17); // 清除中断线17的中断标志
}

// RTC WAKE UP中断服务函数
void RTC_WKUP_IRQHandler(void)
{
	if (RTC_GetFlagStatus(RTC_FLAG_WUTF) == SET)
	{
		RTC_ClearFlag(RTC_FLAG_WUTF); // 清除中断标志
		// LED1=!LED1;
	}
	EXTI_ClearITPendingBit(EXTI_Line22); // 清除中断线22的中断标志
}

void myrtc_initready(void)
{
	My_RTC_Init();
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits, 0);
}

// void lcd_show(void)
//{

//	LCD_ShowIntNum(36,20,RTC_DateStruct.RTC_Month,2,RED,BLACK,16);
//	LCD_ShowIntNum(68,20,RTC_DateStruct.RTC_Date,2,RED,BLACK,16);
//
//	LCD_ShowIntNum(20,40,+RTC_TimeStruct.RTC_Hours,2,RED,BLACK,16);
//	LCD_ShowIntNum(44,40,+RTC_TimeStruct.RTC_Minutes,2,RED,BLACK,16);
//	LCD_ShowIntNum(68,40,+RTC_TimeStruct.RTC_Seconds,2,RED,BLACK,16);

//}

void get_rtc(void) // 获取rtc时钟的数据
{

	// u8 tbuf[40];
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct); // 获取时间

	// sprintf((char*)tbuf,"Time:%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
	// LCD_ShowString(30,140,210,16,16,tbuf);

	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct); // 获取日期

	// LCD_ShowIntNum(20,20,(RTC_DateStruct.RTC_Year),2,RED,BLACK,16);

	// lcd_show();

	// sprintf((char*)tbuf,"Date:20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
	// LCD_ShowString(30,160,210,16,16,tbuf);
	// sprintf((char*)tbuf,"Week:%d",RTC_DateStruct.RTC_WeekDay);
	// LCD_ShowString(30,180,210,16,16,tbuf)
}

void RTC_Tick(void)
{

	count++;
	if (count >= 100) // 每1s更新一次显示数据
	{
		count = 0;
		get_rtc();
	}
}
