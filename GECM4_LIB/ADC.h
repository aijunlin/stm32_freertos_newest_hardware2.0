/**
  ******************************************************************************
  * @file    ADC.h
  * @author  ajl  
  * @version V0.0.1
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
#ifndef _ADC_H_
#define _ADC_H_

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

extern uint16_t ADC1_buf[5];
extern uint16_t ADC3_buf[5];



uint16_t ADC1_Convert(uint8_t ADC_Channel);

void ADC1_Init(void);

void ADC3_Init(void);

void ADC1_Tick(void);

void ADC3_Tick(void);



#endif


