/**
  ******************************************************************************
  * @file    delay.h
  * @author  FZetc飞贼
  * @version V0.0.2
  * @date    2025.10.22
  * @brief   1、延时函数(粗延时)
  *          2、延时函数(系统定时器实现的：精准延时(微秒级、毫秒级))
  *
  ******************************************************************************
  * @attention
  *	
  *	本文档只供学习使用，不得商用，违者必究
  *
  * 微信公众号：    FZetc飞贼
  * CSDN博客：      https://blog.csdn.net/qq_58629108?type=blog
  * 有疑问或者建议： FZetcSnitch@163.com
  *
  ******************************************************************************
  */

#ifndef __DELAY_H		// 定义以防止递归包含
#define __DELAY_H


// 一、其它头文件
#include "stm32f4xx.h"
#include "my_config.h"
#include "FreeRTOS.h"  // 包含 FreeRTOS 基本类型定义
#include "task.h"      // 包含任务相关函数和类型定义

// 二、宏定义(函数、变量、常量)

// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明

// 五、函数声明
// (1)、粗延时
extern void DELAY_MyNms(uint16_t nms);

// (2)、精准延时
extern int8_t DELAY_SysTickInit(uint8_t sysclk);
extern int8_t Delay_us(uint32_t nus);
extern int8_t delay_ms(uint32_t nms);
void delay_ms_rtos(uint32_t nms);

// 六、静态变量、函数定义

#endif /* __DELAY_H */

