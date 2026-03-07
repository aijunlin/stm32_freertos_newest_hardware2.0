
/**
  ******************************************************************************
  * @file    HC_SR04.h
  * @author  ajl  
  * @version V0.0.1
  * @date    23-Octorber-2025
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
#ifndef __HC_SR04_H
#define __HC_SR04_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"



/* Exported types ------------------------------------------------------------*/


/* Exported constants --------------------------------------------------------*/
extern uint8_t HC_SR04_start_flag;
extern uint32_t distance;
extern uint32_t distance_up;
extern uint32_t distance_down;

// extern SemaphoreHandle_t xHCSR04Semaphore;


/* Exported functions --------------------------------------------------------*/

void HC_SR04_Init(void);

uint32_t HC_SR04_Get_Distance(void);

void TIM1_CC_IRQHandler(void);

void HC_SR04_Timer_Tick(void);

uint32_t Distance_Filter(uint32_t new_data);
#endif /* __HC_SR04_H */

/*****END OF FILE****/


