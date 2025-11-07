
/**
  ******************************************************************************
  * @file    DHT11.h
  * @author  ajl  
  * @version V0.0.1
  * @date    27-Octorber-2025
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
#ifndef __DHT11_H
#define __DHT11_H


/* Includes ---------------------------------------------------------S---------*/
#include "stm32f4xx.h"

#include "sys.h"

#include "delay.h"

/* Exported types ------------------------------------------------------------*/

#define DHT11_OUT PGout(9)
#define DHT11_IN  PGin(9)

#define GPIO_IN      1
#define GPIO_OUT     0

/* Exported constants --------------------------------------------------------*/
extern uint8_t DHT11_Data[5];
extern uint8_t DHT11_start_flag;

/* Exported functions --------------------------------------------------------*/

void DHT11_Init(void);

void DHT11_Mode_Switch(uint8_t GPIO_MODE);

int8_t DHT11_Start(void);

int8_t DHT11_ReadData(void);

uint8_t DHT11_Data_Assamble(void);

int8_t DHT11_Data_Get(void);

void DHT11_Timer_Tick(void);
#endif /* __DHT11_H */

/*****END OF FILE****/


