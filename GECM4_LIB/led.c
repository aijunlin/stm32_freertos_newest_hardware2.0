/**
  ******************************************************************************
  * @file    led.c
  * @author  ajl  
  * @version V0.0.1
  * @date    21-Octorber-2025
  * @brief   
  *          
  ******************************************************************************
  * @attention
  *
  * 本函数测试外部中断来控制
  *
  * 
  * 
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "led.h"                  // Device header

/**
  * @brief  led初始化函数
  * @param  None  
  * @note   初始化 PF9 - led1  PF10 - led2  PE13 - led3 PE14 - key4
  * @retval None
  */

void Led_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

    GPIO_Init(GPIOF,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;

    GPIO_Init(GPIOE,&GPIO_InitStructure);

    //初始化led电平为高
    PFout(9)  = 1;
    PFout(10) = 1;
    PEout(13) = 1;
    PEout(14) = 1;


}

/**
  * @brief  led的流水灯函数
  * @param  None  
  * @note   点亮led PF9 - led1  PF10 - led2  PE13 - led3 PE14 - key4
  * @retval None
  */

void Led_Flush_fuc(void)
{
    PFout(9)  = !PFout(9);
    Delay_ms(200);
    PFout(10) = !PFout(10);
    Delay_ms(200);
    PEout(13) = !PEout(13);
    Delay_ms(200);
    PEout(14) = !PEout(14);

}

/**
  * @brief  led的跑马灯函数
  * @param  None  
  * @note   点亮led PF9 - led1  PF10 - led2  PE13 - led3 PE14 - key4
  * @retval None
  */

void Led_Running_fuc(void)
{
    PFout(9)  = !PFout(9);
    Delay_ms(200);
    PFout(9)  = !PFout(9);
    PFout(10) = !PFout(10);
    Delay_ms(200);
    PFout(10) = !PFout(10);
    PEout(13) = !PEout(13);
    Delay_ms(200);
    PEout(13) = !PEout(13);
    PEout(14) = !PEout(14);
    Delay_ms(200);
    PEout(14) = !PEout(14);
}


