#include "led.h"
#include "delay.h"

/**
  * @brief  初始化LED灯
  * @note   GEC-M4板子：
  *         	D1-》LED0-》PF9
  *         	D2-》LED1-》PF10
  *         	D3-》FSMC_D10-》PE13
  *         	D4-》FSMC_D11-》PE14
  *
  *         仿真图：
  *				LED1-》PB0
  *				LED2-》PB1
  *				LED3-》PB2
  *				LED4-》PB3
  * @param  None
  * @retval None
  */
void LED_Init(void)
{
	// 0、GPIO片内外设信息初始化结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 1、使能GPIO片内外设的硬件时钟					// a、开启教室灯的分电闸
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	// 2、配置GPIO片内外设的引脚						// b、装修师傅配置教室灯的电路，使其可以更好的对教室灯进行开关操作
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_9|GPIO_Pin_10;	// 引脚：第9、10根引脚
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;			// 模式：输出模式
	GPIO_InitStructure.GPIO_Speed   = GPIO_High_Speed;			// 速度：高速(100MHz)
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;			// 上下拉：不拉
	GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;			// 输出类型：推挽输出
	GPIO_Init(GPIOF, &GPIO_InitStructure);						// 使用该函数，将配置的信息写入到相应的寄存器中

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_13|GPIO_Pin_14;	// 引脚：第13、14根引脚
	GPIO_Init(GPIOE, &GPIO_InitStructure);						// 使用该函数，将配置的信息写入到相应的寄存器中
	
	// 3、设置GPIO片内外设引脚的高低电平值			// c、可以对教室灯进行开关操作
	LED1(OFF);
	LED2(OFF);
	LED3(OFF);
	LED4(OFF);
	
}

/**
  * @brief  指定led灯亮
  * @note   一共四盏led灯(LED1到LED4),输入不是1-4的参数，就全亮
  * @param  led_num：灯的序号(1到4)
  * @retval None
  */
void LED_ON(uint8_t led_num)
{
	switch(led_num)
	{
		case 1:  LED1(ON); break;
		case 2:  LED2(ON); break;
		case 3:  LED3(ON); break;
		case 4:  LED4(ON); break;
		default: LED1(ON); LED2(ON); LED3(ON); LED4(ON);break;
	}
}

/**
  * @brief  指定led灯灭
  * @note   一共四盏led灯(LED1到LED4),输入不是1-4的参数，就全灭
  * @param  led_num：灯的序号(1到4)
  * @retval None
  */
void LED_OFF(uint8_t led_num)
{
	switch(led_num)
	{
		case 1:  LED1(OFF); break;
		case 2:  LED2(OFF); break;
		case 3:  LED3(OFF); break;
		case 4:  LED4(OFF); break;
		default: LED1(OFF); LED2(OFF); LED3(OFF); LED4(OFF);break;
	}
}


/**
  * @brief  流水灯
  * @note   一共四盏led灯(LED1到LED4),先一个个亮，然后再一个个灭
  * @param  speed：    灯的流速
  *         direction：灯的流向(1：顺着流， 0：逆着流、其他顺着流)
  * @retval None
  */
void LED_WaterRunning(uint16_t speed, uint8_t direction)
{
	int8_t i = 0;
	
	// 逆着流
	if(direction == REVERS)
	{
		for(i=4; i>0; i--)
		{
			LED_ON(i);
			DELAY_MyNms(speed);
		}
		for(i=4; i>0; i--)
		{
			LED_OFF(i);
			DELAY_MyNms(speed);
		}
	}
	// 默认流、顺着流
	else
	{
		for(i=1; i<5; i++)
		{
			LED_ON(i);
			DELAY_MyNms(speed);
		}
		for(i=1; i<5; i++)
		{
			LED_OFF(i);
			DELAY_MyNms(speed);
		}
	}
	
	
}

/**
  * @brief  跑马灯
  * @note   一共四盏led灯(LED1到LED4),一亮一灭循环进行
  * @param  speed：    灯的跑速
  *         direction：灯的跑向(1：顺着跑， 0：逆着跑、其他顺着跑)
  * @retval None
  */
void LED_HorseRunning(uint16_t speed, uint8_t direction)
{
	int8_t i = 0;
	
	// 逆着跑
	if(direction == REVERS)
	{
		for(i=4; i>0; i--)
		{
			LED_ON(i);
			DELAY_MyNms(speed);
	
			LED_OFF(i);
			DELAY_MyNms(speed);
		}
	}
	
	// 默认跑、顺着跑
	else
	{
		for(i=1; i<5; i++)
		{
			LED_ON(i);
			DELAY_MyNms(speed);

			LED_OFF(i);
			DELAY_MyNms(speed);
		}
	}
}


/**
  * @brief  二进制灯
  * @note   利用四盏led灯，以二进制的方式显示数值
  * @param  num：0-9的数值
  * @retval None
  */
void LED_ShowBin(int8_t num)
{
	int8_t i = 0;
	
	for(i=0; i<4; i++)			// (num:7) & (1<<i) ==> 0000 0111 & 0000 0001 ==> 0000 0001(非0为真)
	{
		if( num & (1<<i) )		//                  ==> 0000 0111 & 0000 0010 ==> 0000 0010(非0为真)
		{
			LED_ON(i+1);		//                  ==> 0000 0111 & 0000 0100 ==> 0000 0100(非0为真)
		}
		else					//                  ==> 0000 0111 & 0000 1000 ==> 0000 0000(0为假)
		{
			LED_OFF(i+1);
		}
	}
}











