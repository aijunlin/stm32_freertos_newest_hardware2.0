/**
 ******************************************************************************
 * @file    system_it.c
 * @author  ajl
 * @version V0.1.2
 * @date    22-Octorber-2025
 * @brief
 *
 ******************************************************************************
 * @attention
 *
 *
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "system_it.h" // Device header

volatile uint16_t count =  0;



/**
 * @brief  系统定时的回调函数
 * @param  None
 * @note   系统定时的回调函数，注意不要使用systick的延时函数，会被重复装载
 * @retval None
 */

void SysTick_Handler(void)
{
    count++;
    if (count > 3000)
    {
        KEY_Open_beep();
        // KEY_Reset_beep();
        count = 0;    
    }

}

