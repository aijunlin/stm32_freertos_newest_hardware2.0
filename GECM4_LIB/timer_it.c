/**
 ******************************************************************************
 * @file    timer_it.c
 * @author  ajl
 * @version V0.0.1
 * @date    20-Octorber-2025
 * @brief   1、撰写了定时器的头文件
 *
 ******************************************************************************
 * @attention
 *
 * 进行了定时器的定时实验，配置定时器来进行定时操作观察定时函数回调函数的状态
 *
 *
 *
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "timer_it.h" // Device header

/**
 * @brief  timer初始化函数
 * @param  None
 * @note   注意定时时间的计算
 * @retval None
 */

void Timer_Init(void)
{

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_TIM8, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3, ENABLE);


    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    TIM_TimeBaseInitStructure.TIM_ClockDivision     = 0;
    TIM_TimeBaseInitStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period            = 1000-1; // 定时100ms
    TIM_TimeBaseInitStructure.TIM_Prescaler         = 16800 - 1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_TimeBaseInitStructure.TIM_Period            = 40000-1; // 定时4000ms
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStructure);


    TIM_TimeBaseInitStructure.TIM_Prescaler         = 8400 - 1;

    TIM_TimeBaseInitStructure.TIM_Period            = 2000-1; // 定时200ms
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_TimeBaseInitStructure.TIM_Period            = 5000-1; // 定时500ms
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);



    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);



    TIM_ClearFlag(TIM1,TIM_IT_Update);//防止配置完成的标志位影响中断
    TIM_ClearFlag(TIM2,TIM_IT_Update);
    TIM_ClearFlag(TIM3,TIM_IT_Update);
    TIM_ClearFlag(TIM8,TIM_IT_Update);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM1_UP_TIM10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;

    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;
    NVIC_Init(&NVIC_InitStructure);
 
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM8_UP_TIM13_IRQn;
    NVIC_Init(&NVIC_InitStructure);


    TIM_Cmd(TIM1,ENABLE);
    TIM_Cmd(TIM2,ENABLE);
    TIM_Cmd(TIM3,ENABLE);
    TIM_Cmd(TIM8,ENABLE);
}
/**
 * @brief  timer1初始化函数
 * @param  None
 * @note
 * @retval None
 */

void TIM1_UP_TIM10_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {
        PFout(9) = !PFout(9); // 置位灯
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

/**
 * @brief  timer2初始化函数
 * @param  None
 * @note
 * @retval None
 */

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        PFout(10) = !PFout(10);
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
/**
 * @brief  timer3初始化函数
 * @param  None
 * @note
 * @retval None
 */

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {
        PEout(13) = !PEout(13);

        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}
/**
 * @brief  timer8初始化函数
 * @param  None
 * @note
 * @retval None
 */

void TIM8_UP_TIM13_IRQHandler()
{

    if (TIM_GetITStatus(TIM8, TIM_IT_Update) == SET)
    {
        PEout(14) = !PEout(14);
        TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
    }
}
