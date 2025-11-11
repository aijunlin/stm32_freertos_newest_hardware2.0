/**
  ******************************************************************************
  * @file    HS0038.h
  * @author  ajl  
  * @version V0.0.2
  * @date    5-November-2025
  * @brief   
  *          
  ******************************************************************************
  * @attention
  *
  * 
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HS0038_H
#define __HS0038_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"



/* Exported types ------------------------------------------------------------*/

#define HS0038_READ PAin(8)


/* Exported constants --------------------------------------------------------*/
extern uint8_t HS0038_Start_Flag;

extern uint8_t data_array[4];   //方便调试
extern xSemaphoreHandle xIRSemaphore;
/* Exported functions --------------------------------------------------------*/
void HS_0038_Init(void);

void HS0038_ReadStartdata(void);

int8_t HS0038_ReadOnedata(void);

uint8_t HS0038_ReadBytedata(void);

void HS0038_Readdata(void);

void Enable_HS0038_EXTI8(void);
#endif /* __HS0038_H */



/*****END OF FILE****/


