#include "rtc.h"

uint8_t Hours   = 10;
uint8_t Minutes = 50;
uint8_t Seconds = 5;
uint8_t rtc_alarm_flag = 0;

//1、RTC配置初始化
void RTC_ConfigInit(void)
{
    /* 0、外设信息配置结构体 */
	RTC_InitTypeDef  RTC_InitStructure;		//RTC基础配置

	/* 1、使能电源管理时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		
	/* 2、允许使用RTC时钟 */
	PWR_BackupAccessCmd(ENABLE);

	/* 3、重置RTC域(注意：重置后，需重新开启RTC时钟) */
	//RCC_BackupResetCmd(ENABLE);
	//RCC_BackupResetCmd(DISABLE);
	
	/* 4、使能RTC时钟 */
	RCC_RTCCLKCmd(ENABLE);
	

#if SELECT_RTC_CLK //(为真选择LSE时钟，为假选择LSI时钟)
	
	/* 5、使能外部低速时钟振荡器 */
	RCC_LSEConfig(RCC_LSE_ON);

	/* 6、等到外部低速时钟振荡器准备好 */ 	
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

	/* 6、选择RTC时钟源 */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	

	/* 7、等待直到RTC时间和日期寄存器(RTC_TR和RTC_DR)完成与RTCAPB时钟同步 */
	RTC_WaitForSynchro();
	
	/* 8、配置RTC数据寄存器和RTC预分频器 */
	/* 公式：ck_spre(1Hz) = RTCCLK(LSE) /(AsynchPrediv + 1)*(SynchPrediv + 1)*/
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;					//异步分频系数(127+1)
	RTC_InitStructure.RTC_SynchPrediv  = 0xFF;					//同步分频系数(255+1)
	RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;		//24小时格式
	RTC_Init(&RTC_InitStructure);								//将配置的外设信息，写入到相对应的寄存器中
	
#else
	/* 4、使能内部低速时钟振荡器 */
	RCC_LSICmd(ENABLE);

	/* 5、等到内部低速时钟振荡器准备好 */ 	
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	/* 6、选择RTC时钟源 */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

	/* 7、等待直到RTC时间和日期寄存器(RTC_TR和RTC_DR)完成与RTCAPB时钟同步 */
	RTC_WaitForSynchro();
	
	/* 8、配置RTC数据寄存器和RTC预分频器 */
	/* 公式：ck_spre(1Hz) = RTCCLK(LSI) /(AsynchPrediv + 1)*(SynchPrediv + 1)*/
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;					//异步分频系数(127+1)
	RTC_InitStructure.RTC_SynchPrediv  = 0xF9;					//同步分频系数(249+1)
	RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;		//24小时格式
	RTC_Init(&RTC_InitStructure);								//将配置的外设信息，写入到相对应的寄存器中
	
#endif

}


//2、设置RTC的日期和时间
void RTC_SetDateTime(uint8_t Year, uint8_t Month, uint8_t Date, uint8_t Hours, uint8_t Minutes, uint8_t Seconds)
{
	/* 0、外设信息配置结构体 */
	RTC_DateTypeDef  RTC_DateStruct;					//RTC日期配置
	RTC_TimeTypeDef  RTC_TimeStruct;					//RTC时间配置
	

	/* 1、设置日期(年月日、星期) */
	RTC_DateStruct.RTC_Year		=	Year;				//年份(0-99)
	RTC_DateStruct.RTC_Month	=	Month;				//月份
	RTC_DateStruct.RTC_Date		=	Date;				//日期(1-31)
	RTC_DateStruct.RTC_WeekDay	=	RTC_Weekday_Tuesday;//星期
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);		//将配置的外设信息，写入到相对应的寄存器中
	
	/* 2、设置时间(时分秒) */
	RTC_TimeStruct.RTC_Hours    =  Hours;				//小时(24小时制：0-23，12小时制：0-11) 				
	RTC_TimeStruct.RTC_Minutes  =  Minutes;				//分钟(0-59) 
	RTC_TimeStruct.RTC_Seconds  =  Seconds;  			//秒数(0-59) 
	//RTC_TimeStruct.RTC_H12    =  RTC_H12_PM;
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);		//将配置的外设信息，写入到相对应的寄存器中


}


//3、闹钟的设置
void RTC_Set_Alarm(void)
{
	
	/* 0、外设信息配置结构体	*/
	EXTI_InitTypeDef   EXTI_InitStructure;									//外部中断信息配置结构体
	NVIC_InitTypeDef   NVIC_InitStructure;									//嵌套向量中断控制器信息配置结构体
	RTC_AlarmTypeDef   RTC_AlarmStructure;									//RTC的闹钟信息配置结构体
	
	//1、EXTI中断线的设置
	EXTI_InitStructure.EXTI_Line 	= EXTI_Line17;							//外部中断线17
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;					//外部中断触发方式-中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  				//上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;								//使能外部中断线17
	EXTI_Init(&EXTI_InitStructure);											//将配置的外设信息，写入到相对应的寄存器中
	
	//2、NVIC信息配置
	NVIC_InitStructure.NVIC_IRQChannel 					 = RTC_Alarm_IRQn;	//RTC闹钟A和B外部中断线通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;			//抢占优先级：			 取值范围为(0-15)
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 = 0x0F;			//子优先级(响应优先级)： 取值范围为(0-15)
	NVIC_InitStructure.NVIC_IRQChannelCmd 				 = ENABLE;			//使能NVIC
	NVIC_Init(&NVIC_InitStructure);											//将配置的外设信息，写入到相对应的寄存器中
	
	//4、失能闹钟
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	
	
	//5、ALarm信息配置
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours			=	Hours;			//小时(24小时制：0-23，12小时制：0-11) 
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes        =   Minutes;		//分钟(0-59) 
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds        =   Seconds;  		//秒数(0-59) 
	
#if 1 
     //闹钟每天生效
    RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
#endif
 
#if 0           
    //闹钟指定20号生效
      RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x20;                				//20号
     RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date; 	//指定哪一天生效
     RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_None;                			//不屏蔽哪一天和星期的配置   
#endif

#if 0      
    //闹钟指定星期三
     RTC_AlarmStructure.RTC_AlarmDateWeekDay = RTC_Weekday_Wednesday;        		//星期三
     RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;	//指定星期几生效
     RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_None;                			//不屏蔽哪一天和星期的配置   
#endif


	//6、设置闹钟
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A, &RTC_AlarmStructure);
	
	//7、使能RTC中断
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	
	//8、使能闹钟
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
	
	//9、清除闹钟标志位
	RTC_ClearFlag(RTC_FLAG_ALRAF);


	
}

//4、中断服务函数(alarmA)
void RTC_Alarm_IRQHandler(void)
{
	if((RTC_GetITStatus(RTC_IT_ALRA) == SET) && (EXTI_GetITStatus(EXTI_Line17) == SET))			//判断中断标志位
	{
		rtc_alarm_flag = 1;
		
		RTC_ClearITPendingBit(RTC_IT_ALRA);	
		EXTI_ClearITPendingBit(EXTI_Line17);													//清楚中断标志位，便于下一次产生中断
	}
	
	
}



//5、RTC唤醒中断
void RTC_Wkup_Config(void)
{
	/* 0、外设信息配置结构体	*/
	EXTI_InitTypeDef   EXTI_InitStructure;									//外部中断信息配置结构体
	NVIC_InitTypeDef   NVIC_InitStructure;									//嵌套向量中断控制器信息配置结构体
	
	//1、EXTI中断线的设置
	EXTI_InitStructure.EXTI_Line 	= EXTI_Line22;							//外部中断线22
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;					//外部中断触发方式-中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  				//上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;								//使能外部中断线22
	EXTI_Init(&EXTI_InitStructure);											//将配置的外设信息，写入到相对应的寄存器中
	
	//2、NVIC信息配置
	NVIC_InitStructure.NVIC_IRQChannel 					 = RTC_WKUP_IRQn;	//RTC唤醒中断线通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;			//抢占优先级：			 取值范围为(0-15)
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 = 0x0F;			//子优先级(响应优先级)： 取值范围为(0-15)
	NVIC_InitStructure.NVIC_IRQChannelCmd 				 = ENABLE;			//使能NVIC
	NVIC_Init(&NVIC_InitStructure);											//将配置的外设信息，写入到相对应的寄存器中

    //关闭唤醒功能
    RTC_WakeUpCmd(DISABLE);
    
    //为唤醒功能选择RTC配置好的时钟源
    RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
    
    //设置唤醒计数值为自动重载，写入值默认是0
    RTC_SetWakeUpCounter(1-1);
    
    //清除RTC唤醒中断标志
    RTC_ClearITPendingBit(RTC_IT_WUT);
    
    //使能RTC唤醒中断
    RTC_ITConfig(RTC_IT_WUT, ENABLE);

    //使能唤醒功能
    RTC_WakeUpCmd(ENABLE);
	
	
}


//6、中断服务函数(唤醒功能)
void  RTC_WKUP_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_WUT) == SET)														//判断中断标志位
	{
		printf("RTC_WKUP_IRQHandler\r\n");
		RTC_ClearITPendingBit(RTC_IT_WUT);	
		EXTI_ClearITPendingBit(EXTI_Line22);													//清楚中断标志位，便于下一次产生中断
	}
		
}
