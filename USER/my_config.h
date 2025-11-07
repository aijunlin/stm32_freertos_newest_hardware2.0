/**
  ******************************************************************************
  * @file    my_config.h
  * @author  FZetc飞贼
  * @version V0.0.1
  * @date    2025.10.15
  * @brief   1、配置工程信息
  *
  ******************************************************************************
  * @attention
  *	
  *	本文档只供学习使用，不得商用，违者必究
  *
  * 微信公众号：     FZetc飞贼
  * CSDN博客：       https://blog.csdn.net/qq_58629108?type=blog
  * 有疑问或者建议： FZetcSnitch@163.com
  *
  ******************************************************************************
  */

#ifndef __MY_CONFIG_H		// 定义以防止递归包含
#define __MY_CONFIG_H


// 一、其它头文件
#include <stdio.h>
#include <string.h>

#include "stm32f4xx.h"
#include "misc.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_crc.h"
#include "stm32f4xx_cryp.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dbgmcu.h"
#include "stm32f4xx_dcmi.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_dma2d.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_flash.h"
#include "stm32f4xx_flash_ramfunc.h"
#include "stm32f4xx_fmc.h"
#include "stm32f4xx_fsmc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_hash.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_iwdg.h"
#include "stm32f4xx_ltdc.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rng.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_sai.h"
#include "stm32f4xx_sdio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_wwdg.h"


// 二、宏定义(函数、变量、常量)
#define ON  1
#define OFF 0
#define ALL 99

#define ORDER  1 // 顺序
#define REVERS 0 // 逆序

// 三、自定义的数据类型(结构体、联合体、枚举等)


// 四、全局变量声明

// 五、函数声明
extern void MY_CONFIG_Init(void);

// 六、静态变量、函数定义

#endif /* __MY_CONFIG_H */

