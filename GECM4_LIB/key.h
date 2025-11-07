/**
  ******************************************************************************
  * @file    led.h
  * @author  ajl  
  * @version V0.0.1
  * @date    20-Octorber-2025
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
#ifndef __KEY_H
#define __KEY_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "sys.h"
#include "pwm.h"
#include "delay.h"



/* Exported types ------------------------------------------------------------*/

#define  KEY1     PAin(0)
#define  KEY2     PEin(2)
#define  KEY3     PEin(3)
#define  KEY4     PEin(4)

#define BEEP_ON   PFout(8) = 1
#define BEEP_OFF  PFout(8) = 1

/* Exported constants --------------------------------------------------------*/


extern uint8_t Beep_Flag;
extern uint8_t Song_Flag;


/* Exported functions --------------------------------------------------------*/
void Key_Init(void);

void Key_beep_init(void);

void KEY_Open_beep(void);

void KEY_Close_beep(void);

void Key_Song_fuc(void);


#endif /* __KEY_H */



/*****END OF FILE****/


