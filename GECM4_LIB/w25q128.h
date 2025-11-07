
/**
  ******************************************************************************
  * @file    w25q128.h
  * @author  ajl  
  * @version V0.0.1
 * @date    4-November-2025
  * @brief   
  *          
  ******************************************************************************
  * @attention
  *
  * 
  *
  * 
  * 
  * 
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __W25Q128_H
#define __W25Q128_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "stdint.h"
#include "w25q128_ins.h"



/* Exported constants --------------------------------------------------------*/
extern uint8_t MID;
extern uint16_t DID;
extern uint8_t arraywrite[8];//写入缓冲区
extern uint8_t arrayread[8];//读取缓冲区

/* Exported functions --------------------------------------------------------*/

void Myspi_w_ss(uint8_t BitValue);//芯片使能位
void W25q64_ReadID(uint8_t *MID, uint16_t *DID);
void Myspi_Start(void);
uint8_t Myspi_Swapbyte(uint8_t bytesend);
void myspi_stop(void);
void W25q128_Init(void);
void w25q64_pageprogram(uint32_t Address,uint8_t *DataArray,uint16_t Count);//页编程函数
void w25q64_sectorerasse(uint32_t Address);//擦除扇
void w25q64_ReadData(uint32_t Address,uint8_t *DataArray,uint32_t Count);//数据读取函数


#endif /* __W25Q128_H */

/*****END OF FILE****/

