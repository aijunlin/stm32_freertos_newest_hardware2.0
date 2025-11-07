#include "exti.h"
#include "delay.h"
#include "buzzer.h"
#include "hs0038.h"
#include <stdio.h>

/**
  * @brief  初始化红外模块触发的外部中断
  * @note   GEC-M4板子上               hs0038红外接收头模块
  *         U6-》REMOTE_IN -》PA8	   data      
  * @param  None
  * @retval None
  */
void EXTI8_HS0038_Init(void)
{
	// 外设信息配置结构体变量
	EXTI_InitTypeDef  EXTI_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	// 1、配置引脚为输入模式
	HS0038_Init();

	// 2、将GPIO引脚连接到相应的EXTI中
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);  						// 使能SYSCFG寄存器
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8); 					// EXTI0线被PA8用了

	// 3、配置外部中断
	EXTI_InitStructure.EXTI_Line    = EXTI_Line8;									// 外部中断线：8线
	EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;							// 模式：中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;							// 触发方式：下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;										// 使能外部中断线
	EXTI_Init(&EXTI_InitStructure);													// 使用该函数，将配置的信息写入到相应的寄存器中
	

	// 4、配置事件到的优先等级
	NVIC_InitStructure.NVIC_IRQChannel                    = EXTI9_5_IRQn;			// 中断请求通道：外部中断5到9通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0;						// 抢占式优先级：0x00
	NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0;						// 响应式优先级：0x00
	NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;					// 使能NVIC相应的中断请求通道
	NVIC_Init(&NVIC_InitStructure);													// 使用该函数，将配置的信息写入到相应的寄存器中			
													
}



/**
  * @brief  外部中断5到9的的中断服务函数
  * @note   None
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)	
{	
	int8_t hs0038_ret     = 0;
	uint8_t hs0038_buf[4] = {0};
	
	if(EXTI_GetITStatus( EXTI_Line8) == SET)  				// 判断是否出发了外部中断0线
	{
		
		hs0038_ret = HS0038_GetVal(hs0038_buf);
		if(hs0038_ret == 0)
		{
			printf("接收到的红外数据为: 0x%X\r\n", hs0038_buf[2]);	// 经过测算，获取的地址码为0，所以可以不用
			if( hs0038_buf[2] == 0xC)	// 按键1
			{
				BUZZER(ON);
			}
			
			if( hs0038_buf[2] == 0x18)	// 按键2
			{
				BUZZER(OFF);
			}
		}
		
		
		EXTI_ClearITPendingBit(EXTI_Line8);	  				// 清除外部中断0线的中断挂起标志位
	}				
}




