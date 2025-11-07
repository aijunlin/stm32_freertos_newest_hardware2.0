/**
 ******************************************************************************
 * @file    key.c
 * @author  ajl
 * @version V0.0.1
 * @date    20-Octorber-2025
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
#include "key.h" // Device header




uint8_t Beep_Flag = 0; //监测扬声器状态的函数
uint8_t Song_Flag = 0;




/**
 * @brief  key初始化函数
 * @param  None
 * @note   初始化 PA0 - key1  PE2 - key2 PE3 - key3 PE4 - key4
 * @retval None
 */

void Key_Init(void)
{

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_SYSCFGEN, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;

    GPIO_Init(GPIOE, &GPIO_InitStruct);

    // 将中源绑定到引脚0
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);

    EXTI_InitTypeDef EXTI_InitStruct;

    EXTI_InitStruct.EXTI_Line = EXTI_Line0 | EXTI_Line2 | EXTI_Line3 | EXTI_Line4;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;

    EXTI_Init(&EXTI_InitStruct);

    // 初始化nvic的结构体
    NVIC_InitTypeDef NVIC_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannel                      = EXTI0_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority    = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority           = 0;
	
	NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel                      = EXTI2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority    = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority           = 1;
	
	NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel                      = EXTI3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority    = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority           = 2;
	
	NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel                      = EXTI4_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority    = 2;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority           = 2;

    NVIC_Init(&NVIC_InitStruct);
}

/**
 * @brief  BEEP初始化函数
 * @param  None
 * @note   初始化 PF8 - beep
 * @retval None
 */

void Key_beep_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed  = GPIO_Fast_Speed;
    GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;

    GPIO_Init(GPIOF, &GPIO_InitStructure);
}
/**
 * @brief  beep的启动函数和关闭函数
 * @param   none
 * @note   初始化 PF8 - beep
 *         蜂鸣器为有源蜂鸣器无需pwm直接置位1启动
 * @retval None
 */
void KEY_Open_beep(void)
{
    PFout(8) = 1;
}

void KEY_Close_beep(void)
{
    PFout(8) = 0;
}

/**
 * @brief  key的外部中断函数
 * @param  None
 * @note   初始化 PA0 - key1
 * @retval None
 */

void EXTI0_IRQHandler(void)
{

    if (EXTI_GetITStatus(EXTI_Line0) == SET) // 当检测到外部中断线被置位的时候
    {
        //在这里播放第一首音乐
        Song_Flag = 1;
        
        EXTI_ClearITPendingBit(EXTI_Line0); // 清除外部中断的标志位
    }
}
/**
 * @brief  key的外部中断函数
 * @param  None
 * @note   初始化 PE2 - key2
 * @retval None
 */
void EXTI2_IRQHandler(void)
{

    if (EXTI_GetITStatus(EXTI_Line2) == SET) // 当检测到外部中断线被置位的时候
    {
        

        EXTI_ClearITPendingBit(EXTI_Line2); // 清除外部中断的标志位
    }
}
/**
 * @brief  key的外部中断函数
 * @param  None
 * @note   初始化 PE3 - key3
 * @retval None
 */
void EXTI3_IRQHandler(void)
{

    if (EXTI_GetITStatus(EXTI_Line3) == SET) // 当检测到外部中断线被置位的时候
    {
        

        EXTI_ClearITPendingBit(EXTI_Line3); // 清除外部中断的标志位
    }
}
/**
 * @brief  key的外部中断函数
 * @param  None
 * @note   初始化 PE4 - key4
 * @retval None
 */
void EXTI4_IRQHandler(void)
{

    if (EXTI_GetITStatus(EXTI_Line4) == SET) // 当检测到外部中断线被置位的时候
    {
        

        EXTI_ClearITPendingBit(EXTI_Line4); // 清除外部中断的标志位
    }
}

void Key_Song_fuc(void)
{
    
    for (uint8_t i = 0; i < sizeof(happy_birthday); i++)
    {
		if(i == happy_birthday_hight[i])
		{
			Pwm_Set_Frequency(hight_pronounce[(happy_birthday[i])-1]);
		}
		if(i == happy_birthday_low[i])
		{
			Pwm_Set_Frequency(low_pronounce[(happy_birthday[i])-1]);
		}

        Pwm_Set_Frequency(pronounce[(happy_birthday[i])-1]);
		Pwm_Set_Duty(30);
        Delay_ms(500);
    }
}

