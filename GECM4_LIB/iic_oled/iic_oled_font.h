/**
  ******************************************************************************
  * @file    iic_oled_font.h
  * @author  FZetc飞贼
  * @version V0.0.1
  * @date    2025.11.01
  * @brief   1、oled模块的字库
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

#ifndef __IIC_OLED_FONT_H		// 定义以防止递归包含
#define __IIC_OLED_FONT_H


// 一、其它头文件
#include "stm32f4xx.h"
#include "my_config.h"
#include "my_reg.h"

// 二、宏定义(函数、变量、常量)

// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明
extern const unsigned char F6x8[][6];
extern const unsigned char F8X16[];
extern char Hzk[][32];
	

// 五、函数声明

// 六、静态变量、函数定义

#endif /* __IIC_OLED_FONT_H */


