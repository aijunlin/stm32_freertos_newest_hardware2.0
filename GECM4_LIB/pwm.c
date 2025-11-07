/**
 ******************************************************************************
 * @file    pwm.c
 * @author  ajl
 * @version V0.0.1
 * @date    23-Octorber-2025
 * @brief
 *
 ******************************************************************************
 * @attention
 *
 * 本函数使用定时器13来控制产生pwm来控制蜂鸣器播放音乐
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "pwm.h" // Device header


uint16_t low_pronounce[7]   = {256, 288, 320, 341, 384, 426  ,480};

uint16_t pronounce[7]       = {523, 587, 659, 698, 784, 880  ,987};

uint16_t hight_pronounce[7] = {1046, 1174, 1318, 1396, 1568, 1760  ,1975};

uint16_t happy_birthday[25] = {5,5,6,5,1,7,   5,5,6,5,2,1,    5,5,5,3,1,7,6,   4,4,3,1,2,1};
	
uint16_t happy_birthday_hight[25]	= {4,10,11,14,15,16,19,20,21,22,23,24,25,26};
	
uint16_t happy_birthday_low[25]	= {0,1,6,7,12,13};

	
uint32_t tim13_cnt = 0;
/**
 * @brief  pwm的初始化函数
 * @param  None
 * @note   使用了timer13的ch1通道来产生pwm
 *          PF8/TIM13_CH1 -> beep
 * @retval None
 */
void Pwm_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Fast_Speed;

    GPIO_Init(GPIOF,&GPIO_InitStructure);


    GPIO_PinAFConfig(GPIOF,GPIO_PinSource8,GPIO_AF_TIM13);

    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period            = 60000 -1;//定时时间1000ms  1HZ
    TIM_TimeBaseStructure.TIM_Prescaler         = 1400 - 1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;//二分频

    TIM_TimeBaseInit(TIM13,&TIM_TimeBaseStructure);

    //配置输出比较 (OC) 模式
    TIM_OCInitTypeDef TIM_OCInitStructure;

    TIM_OCInitStructure.TIM_OCIdleState     = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse           = 0; //初始化占空比

    TIM_OC1Init(TIM13, &TIM_OCInitStructure);  //输出捕获初始化

    TIM_OC1PreloadConfig(TIM13, TIM_OCPreload_Enable);//减少毛刺的函数


    TIM_Cmd(TIM13,ENABLE);//开启定时器

}

/**
 * @brief  pwm的占空比配置函数
 * @param  Compare1：占空比的大小
 * @note   使用了TIM_SetCompare1来设置占空比的大小
 * @retval None
 */
void Pwm_Set_Duty(uint32_t Compare1)
{
    TIM_SetCompare1(TIM13, Compare1); // 设置占空比
}

/**
 * @brief  pwm的频率(自动重装载寄存器)
 * @param  frequency:设置频率的大小
 * @note   使用了TIM_SetCompare1来设置占空比的大小
 * @retval None
 */
void Pwm_Set_Frequency(uint16_t frequency)
{
     TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;					
	
	// 基本的定时器设置
	TIM_TimeBaseStructure.TIM_Prescaler     = 1400-1;					
	TIM_TimeBaseStructure.TIM_Period        = 60000/frequency-1;		
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;						
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;		
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);					
	
	// 记住现在的计数值
	tim13_cnt = TIM_TimeBaseStructure.TIM_Period;
}

//TIM_SetAutoreload(TIM13, (10000 - 1) / frequency); // 运行时修改 ARR





