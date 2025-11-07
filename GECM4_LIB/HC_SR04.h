
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



/* Exported types ------------------------------------------------------------*/
#define Trig_IO PDout(7)
#define Echo_IO PCin(6) // 实际中不应该再使用软件轮询


/* Exported constants --------------------------------------------------------*/
extern volatile uint32_t Dis;
extern uint8_t HC_SR04_start_flag;


/* Exported functions --------------------------------------------------------*/

void HC_SR04_TIM8_Init(void);

void HC_SR04_Init(void);

uint32_t HC_SR04_Get_Distance(void);

void TIM8_CC_IRQHandler(void);

void HC_SR04_Timer_Tick(void);


#endif /* __HC_SR04_H */

/*****END OF FILE****/


