#include "systick_it.h"
#include "buzzer.h"


static volatile int16_t systick_count = 0;

/**
  * @brief  SysTick中断服务函数
  * @note   None
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	static int8_t flag = 0;
	systick_count++;
	
	if(systick_count == 500)
	{
		flag = !flag;	// 逻辑非(真遇到此语句变为假，反之亦然)
		BUZZER(flag);

		systick_count = 0;
	}
}
