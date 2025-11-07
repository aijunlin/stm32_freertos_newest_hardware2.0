/**
  ******************************************************************************
  * @file    hc_sr04.h
  * @author  FZetc飞贼
  * @version V0.0.1
  * @date    2025.10.25
  * @brief   1、初始化hc_sr04超声波模块
  *          2、获取hc_sr04超声波模块测到的距离
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

#ifndef __HC_SR04_H		// 定义以防止递归包含
#define __HC_SR04_H


// 一、其它头文件
#include "stm32f4xx.h"
#include "my_config.h"
#include "my_reg.h"

// 二、宏定义(函数、变量、常量)
#define TRIG PBout(6)
#define ECHO PEin(6)

// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明

// 五、函数声明
extern void HC_SR04_Init(void);
extern float HC_SR04_GetDis(void);
	

// 六、静态变量、函数定义

#endif /* __HC_SR04_H */

