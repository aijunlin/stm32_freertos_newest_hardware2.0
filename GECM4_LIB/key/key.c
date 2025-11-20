#include "key.h"

/**
  * @brief  初始化按键
  * @note   GEC-M4板子：
  *         	S1-》KEY0-》PA0
  *         	S2-》KEY1-》PE2
  *         	S3-》KEY2-》PE3
  *         	S4-》KEY3-》PE4
  *
  *         仿真图：
  *				KEY1-》PA0
  *				KEY2-》PD2
  *				KEY3-》PD3
  *				KEY4-》PD4
  * @param  None
  * @retval None
  */
void KEY_Init(void)
{
 
	// 0、GPIO片内外设信息初始化结构体
	GPIO_InitTypeDef  GPIO_InitStructure;

	// 1、使能GPIO片内外设的硬件时钟					// a、开启教室灯的分电闸
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	// 2、配置GPIO片内外设的引脚						// b、装修师傅配置教室灯的电路，使其可以更好的对教室灯进行开关操作
	GPIO_InitStructure.GPIO_Pin      = GPIO_Pin_0;			// 引脚：第0根引脚;
	GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_IN;		// 模式：输入模式
	// GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	 	// 输出类型：推挽输出
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// 速度：高速(100MHz)
	GPIO_InitStructure.GPIO_PuPd     = GPIO_PuPd_NOPULL;	// 上下拉：不拉	
	GPIO_Init(GPIOA, &GPIO_InitStructure);					// 使用该函数，将配置的信息写入到相应的寄存器中

	GPIO_InitStructure.GPIO_Pin      = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;			// 引脚：第2、3、4根引脚;
	GPIO_Init(GPIOE, &GPIO_InitStructure);					// 使用该函数，将配置的信息写入到相应的寄存器中 
}


/**
 * @brief  FreeRTOS适配版 按键扫描函数
 * @param  mode: 0=不支持连按; 1=支持连按
 * @retval 键值 (KEY0_PRES... 或 KEY_NONE)
 */
uint8_t KEY_Scan(uint8_t mode)
{
    static uint8_t key_up = 1; // 按键松开标志
    
    // 如果支持连按，每次都重置标志位
    if (mode) key_up = 1;

    // 1. 检测按键是否按下 
    // (KEY0==1 或 其他==0)
    if (key_up && (KEY1 == 1 || KEY2 == 0 || KEY3 == 0 || KEY4 == 0))
    {
        // 2. RTOS消抖：挂起当前任务 20ms，让出CPU给其他任务
        vTaskDelay(pdMS_TO_TICKS(20)); 

        key_up = 0; // 标记已按下

        // 3. 再次检测，确认不是干扰
        if (KEY1 == 1) return 1;
        else if (KEY2 == 0) return 2;
        else if (KEY3 == 0) return 3;
        else if (KEY4 == 0) return 4;
    }
    // 4. 检测按键是否松开
    else if (KEY1 == 0 && KEY2 == 1 && KEY3 == 1 && KEY4 == 1)
    {
        key_up = 1;
    }

    return 0;
}


