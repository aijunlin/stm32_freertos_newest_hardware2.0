/**
 ******************************************************************************
 * @file    motor.c
 * @author  ajl
 * @version V0.0.1
 * @date    26-Octorber-2025
 * @brief
 *
 ******************************************************************************
 * @attention
 *
 * 本函数测试外部中断来控制
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "motor.h"

uint8_t motor_flag = 0;

void motor_init(void)
{
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(GPIOA, &GPIO_InitStruct);

}


