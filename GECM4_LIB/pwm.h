/**
  ******************************************************************************
  * @file    pwm.h
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
#ifndef __PWM_H
#define __PWM_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"



/* Exported types ------------------------------------------------------------*/





/* Exported constants --------------------------------------------------------*/

extern uint16_t low_pronounce[7] ;

extern uint16_t pronounce[7];   

extern uint16_t hight_pronounce[7];

extern uint16_t happy_birthday[25];

extern uint16_t happy_birthday_hight[25];
	
extern uint16_t happy_birthday_low[25];

/* Exported functions --------------------------------------------------------*/
void Pwm_Init(void);

void Pwm_Set_Duty(uint32_t Compare1);

void Pwm_Set_Frequency(uint16_t frequency);



#endif /* __PWM_H */



/*****END OF FILE****/


