#include "timer.h"
#include "delay.h"


// 定时器2函数初始化
void TIMER2_Init(void)
{
	/* 0、外设配置信息结构体 */
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;								//基本定时器配置结构体
	NVIC_InitTypeDef 		 	NVIC_InitStructure;									//嵌套向量中断控制器配置结构体

	TIM_Cmd(TIM2, DISABLE);
	
	/* 1、使能定时器2硬件时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* 2、使能定时器2全局中断,并配置优先级 */
	NVIC_InitStructure.NVIC_IRQChannel 						= TIM2_IRQn;			//定时器2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0x00;					//抢占优先级(0-15)
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 0x01;					//子优先级(响应优先级)(0-15)
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;				//使能定时器2中断
	NVIC_Init(&NVIC_InitStructure);													//将配置的外设信息，写入到相对应的寄存器中


	/* 3、定时器基本配置 */
	TIM_TimeBaseStructure.TIM_Prescaler 					= 8400-1;				//预分配系数(42MHZ*2=84MHZ，84MHZ/8400 = 10000HZ,计数频率)
	TIM_TimeBaseStructure.TIM_Period 						= 50000-1;				//计数周期(范围;0-65535)	
	//TIM_TimeBaseStructure.TIM_ClockDivision 				= 0;					//时钟分割系数(二次分频，STM32F407不支持)
	TIM_TimeBaseStructure.TIM_CounterMode					= TIM_CounterMode_Up;	//计数模式：向上计数
	//TIM_TimeBaseStructure.TIM_RepetitionCounter			= 0;					//重复计数器(只对tim1和tim8有效)(0-255)
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);									//将配置的外设信息，写入到相对应的寄存器中


	/* 4、使能定时器2中断 */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	/* 5、使能定时器2计数  */
	TIM_Cmd(TIM2, ENABLE);

}



// 中断服务函数(定时器2)
void TIM2_IRQHandler(void)	//每隔5秒钟喂一次狗
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{	
		IWDG_ReloadCounter();
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

