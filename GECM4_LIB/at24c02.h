/**
  ******************************************************************************
  * @file    iic_at24c02.h
  * @author  FZetc飞贼
  * @version V0.0.1
  * @date    2025.11.01
  * @brief   1、iic协议
  *          2、初始化at24c02模块
  *          3、读写at24c02模块
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

#ifndef __IIC_AT24C02_H		// 定义以防止递归包含
#define __IIC_AT24C02_H


// 一、其它头文件
#include "stdio.h"
#include "stm32f4xx.h"
#include "delay.h"
#include "sys.h"


// 二、宏定义(函数、变量、常量)
#define IIC_AT24C02_SCL     PBout(8)
#define IIC_AT24C02_SDA_OUT PBout(9)
#define IIC_AT24C02_SDA_IN  PBin(9)

// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明

// 五、函数声明
// IIC协议
extern void    IIC_AT24C02_Start(void);
extern void    IIC_AT24C02_Stop(void);
extern void    IIC_AT24C02_SendByte(uint8_t byte);
extern uint8_t IIC_AT24C02_RecvByte(void);
extern void    IIC_AT24C02_SendAck(uint8_t ack);	
extern uint8_t IIC_AT24C02_RecvAck(void);	
	
// AT24C02通信
extern void    IIC_AT24C02_Init(void);
extern int8_t  IIC_AT24C02_SelectMode(GPIOMode_TypeDef GPIO_Mode);
extern int8_t  IIC_AT24C02_PageWrite(uint8_t addr, uint8_t *pbuf, uint8_t len);
extern int8_t  IIC_AT24C02_SequentialRead(uint8_t addr, uint8_t *pbuf, uint8_t len);


// 六、静态变量、函数定义

#endif /* __IIC_AT24C02_H */


