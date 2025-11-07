/**
  ******************************************************************************
  * @file    RTC.h
  * @author  ajl  
  * @version V0.0.1
  * @date    5-November-2025
  * @brief   
  *          
  ******************************************************************************
  * @attention
  *		demo尚未完成测试
  * 
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H_
#define __RTC_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stdint.h"
#include "Delay.h"
#include "oled.h"


extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;

uint8_t My_RTC_Init(void);                                                         // RTC初始化
ErrorStatus RTC_Set_Time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t ampm);    // RTC时间设置
ErrorStatus RTC_Set_Date(uint8_t year, uint8_t month, uint8_t date, uint8_t week); // RTC日期设置
void RTC_Set_AlarmA(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec);         // 设置闹钟时间(按星期闹铃,24小时制)
void RTC_Set_WakeUp(uint32_t wksel, uint16_t cnt);                                 // 周期性唤醒定时器设置

void get_rtc(void); // 获取rtc时间
void myrtc_initready(void);
// void lcd_show(void);
void RTC_Tick(void);

#endif
