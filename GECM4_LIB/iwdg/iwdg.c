#include "iwdg.h"

//1、独立看门狗初始化
void IWDG_Init(uint8_t time)
{
	uint16_t reload_val = 0;
	
	//0、查看开发板是否被重启了
	printf("This is iwdg test!\r\n");
	
	//1、判断当前的芯片是否从独立看门狗复位
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		/* IWDGRST flag set */
		printf("iwdg reset cpu\r\n");
	}
	else
	{
		/* IWDGRST flag is not set */
		printf("normal reset cpu\r\n");
	}
	

	//1、使能IWDG硬件时钟
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	//2、设置分频值
	IWDG_SetPrescaler(IWDG_Prescaler_256);	//32KHZ/256 == 125HZ
	
	//3、设置计数值(0-4095)
	reload_val = time*125-1;
	IWDG_SetReload(reload_val);	
	
	//4、刷新计数值(喂狗)
	IWDG_ReloadCounter();
	
	//5、使能IWDG外设
	IWDG_Enable() ;
	
	
	
}
