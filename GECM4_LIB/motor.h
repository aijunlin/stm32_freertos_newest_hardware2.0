/**
  ******************************************************************************
  * @file    motor.h
  * @author  ajl  
  * @version V0.0.1
  * @date    26-Octorber-2025
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
#ifndef __MOTOR_H
#define __MOTOR_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "sys.h"


/* Exported types ------------------------------------------------------------*/
#define    MOTORON  PAout(2) = 1;
#define    MOTOROFF PAout(2) = 0;


/* Exported constants --------------------------------------------------------*/
extern uint8_t motor_falg;


/* Exported functions --------------------------------------------------------*/
void motor_init(void);

#endif /* __MOTOR_H */



/*****END OF FILE****/


