#include "buzzer.h"
#include "delay.h"

/**
  * @brief  初始化蜂鸣器模块
  * @note   GEC-M4板子：
  *         	LS2-》BEEP-》PF8
  *
  *         仿真图：
  *				BEEP-》PB8
  *
  * @param  None
  * @retval None
  */
void BUZZER_Init(void)
{
	// 0、GPIO片内外设信息初始化结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 1、使能GPIO片内外设的硬件时钟					// a、开启教室灯的分电闸
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	// 2、配置GPIO片内外设的引脚						// b、装修师傅配置教室灯的电路，使其可以更好的对教室灯进行开关操作
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_8;			// 引脚：第8根引脚
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;		// 模式：输出模式
	GPIO_InitStructure.GPIO_Speed   = GPIO_High_Speed;		// 速度：高速(100MHz)
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;		// 上下拉：不拉
	GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;		// 输出类型：推挽输出
	GPIO_Init(GPIOF, &GPIO_InitStructure);					// 使用该函数，将配置的信息写入到相应的寄存器中

	
	// 3、设置GPIO片内外设引脚的高低电平值			// c、可以对教室灯进行开关操作
	BUZZER(OFF);

}


/**
  * @brief  蜂鸣器提示音
  * @note   None
  * @param  speed：提示音的长度
  * @retval None
  */
void BUZZER_TipVoice(uint16_t speed)
{
	BUZZER(ON);
	DELAY_MyNms(speed);
	BUZZER(OFF);
}

/**
  * @brief  蜂鸣器警报声
  * @note   None
  * @param  None
  * @retval None
  */
void BUZZER_WarnVoice(uint16_t speed, uint16_t count)
{
	uint16_t i = 0;
	
	for(i=0; i<count; i++)
	{
		BUZZER(ON);
		DELAY_MyNms(speed);
		
		BUZZER(OFF);
		DELAY_MyNms(speed);	
	}
}






