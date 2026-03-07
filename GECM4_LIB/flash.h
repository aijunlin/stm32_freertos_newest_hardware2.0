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
#include "uart.h"
#include "FreeRTOS.h"
#include "task.h"

// (2)、宏定义(函数、变量、常量)
#define Sector0_ADDR  0x08000000
#define Sector1_ADDR  0x08004000
#define Sector2_ADDR  0x08008000
#define Sector3_ADDR  0x0800C000
#define Sector4_ADDR  0x08010000
#define Sector5_ADDR  0x08020000
#define Sector6_ADDR  0x08040000

// (3)、数据类型(结构体、联合体、枚举)
extern uint32_t distance_up;
extern uint32_t distance_down;

// 定义存储参数的 Flash 扇区和地址 
// 强烈建议使用扇区11 (STM32F407 1MB 版本的最后一个扇区，最安全，不会覆盖程序)
#define PARAM_FLASH_SECTOR   FLASH_Sector_7
#define PARAM_FLASH_ADDR     0x08060000
#define FLASH_MAGIC_WORD     0x55AA1234  // 数据有效标志
// (4)、全局变量声明

// (5)、函数声明
void FLASH_WriteData(uint32_t FLASH_Sector, uint32_t Address, uint8_t *Data_buf, uint16_t len); 
void FLASH_ReadData(uint32_t Address, uint8_t *Data_buf, uint16_t len);

void Save_Distance_To_Flash(void);
void Load_Distance_From_Flash(void);
// (6)、全局静态函数定义

#endif /* __FLASH_H */





