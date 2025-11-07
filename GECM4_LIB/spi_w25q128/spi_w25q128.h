/**
  ******************************************************************************
  * @file    spi_w25q128.h
  * @author  FZetc飞贼
  * @version V0.0.1
  * @date    2025.11.03
  * @brief   1、spi协议编写(模式0和模式3)
  *          2、初始化spi_w25q128模块
  *          3、读写spi_w25q128模块
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

#ifndef __SPI_W25Q128_H		// 定义以防止递归包含
#define __SPI_W25Q128_H


// 一、其它头文件
#include "stm32f4xx.h"
#include "my_config.h"
#include "my_reg.h"


// 二、宏定义(函数、变量、常量)

// W25Q128模块
#define SPI_W25Q128_SCLK  PBout(3)
#define SPI_W25Q128_MISO  PBin(4) 
#define SPI_W25Q128_MOSI  PBout(5) 
#define SPI_W25Q128_CS    PBout(14) 
 

// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明

// 五、函数声明
extern uint8_t  SPI_W25Q28_MODE0_SendRecvByte(uint8_t byte);	
extern uint8_t  SPI_W25Q28_MODE3_SendRecvByte(uint8_t byte);	

extern void     SPI_W25Q28_Init(void);	
extern uint16_t SPI_W25Q28_ReadID(void);
extern int8_t   SPI_W25Q28_ReadStatus(uint8_t statu_addr);	
extern int8_t   SPI_W25Q28_ReadData(uint32_t addr, uint8_t* pbuf, uint16_t len);	
extern void     SPI_W25Q28_WriteEnable(void);	
extern void     SPI_W25Q28_WriteDisable(void);	
extern void     SPI_W25Q28_SectorErase(uint32_t addr);	
extern void     SPI_W25Q28_WritePageData(uint32_t addr, uint8_t* pbuf, uint16_t len);	

	
// 六、静态变量、函数定义

#endif /* __SPI_W25Q128_H */


