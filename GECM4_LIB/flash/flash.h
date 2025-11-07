/**
  ******************************************************************************
  * @file    flash.h
  * @author  xxxxxxx
  * @version V0.0.5
  * @date    2024.6.24
  * @brief   1、片内flash初始化
  *          2、读写flash的数据
  ******************************************************************************
  * @attention
  *
  *  本文档只供学习使用，不得商用，违者必究
  *  有建议可发邮箱xxxxxx@163.com
  * 
  ******************************************************************************
  */

#ifndef __FLASH_H			// 定义以防止递归包含
#define __FLASH_H


// (1)、函数头文件
#include "stm32f4xx.h"
#include "my_config.h"
#include "my_reg.h"

// (2)、宏定义(函数、变量、常量)
#define Sector0_ADDR  0x08000000
#define Sector1_ADDR  0x08004000
#define Sector2_ADDR  0x08008000
#define Sector3_ADDR  0x0800C000
#define Sector4_ADDR  0x08010000
#define Sector5_ADDR  0x08020000
#define Sector6_ADDR  0x08040000

// (3)、数据类型(结构体、联合体、枚举)

// (4)、全局变量声明

// (5)、函数声明
extern void FLASH_WriteData(uint32_t FLASH_Sector, uint32_t Address, uint8_t *Data_buf, uint16_t len); 
extern void FLASH_ReadData(uint32_t Address, uint8_t *Data_buf, uint16_t len);

// (6)、全局静态函数定义

#endif /* __FLASH_H */





