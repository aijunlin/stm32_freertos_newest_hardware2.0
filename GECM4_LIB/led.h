/**
  ******************************************************************************
  * @file    led.h
  * @author  ajl  
  * @version V0.0.1
  * @date    21-Octorber-2025
  * @brief   
  *          
  ******************************************************************************
  * @attention
  *
  * 
  *
  * 
  * 
  * ·
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H
#define __LED_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"

/* Exported types ------------------------------------------------------------*/


/* Exported constants --------------------------------------------------------*/



/* Exported functions --------------------------------------------------------*/
void Led_init(void);

void Led_Flush_fuc(void);

void Led_Running_fuc(void);


#endif /* __LED_H */



/*****END OF FILE****/

