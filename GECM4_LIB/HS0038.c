/**
 ******************************************************************************
 * @file    HS0038.c
 * @author  ajl
 * @version V0.0.2
 * @date    5-November-2025
 * @brief   
 ******************************************************************************
 * @attention
 *         V0.0.1 成功完成了数据稳定的读取，但是数据的读取和实际的发送的数据不正确
 *         V0.0.2 修复了移位的错误成功实现了数据的准确读取
 * 
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "HS0038.h" // Device header


uint8_t HS0038_Start_Flag = 0;

uint8_t data_array[4] = {0};

xSemaphoreHandle xIRSemaphore = NULL;


/**
 * @brief  HS0038的初始化函数
 * @param  None
 * @note   pa8
 * @retval None
 */

void HS_0038_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;

    GPIO_Init(GPIOA,&GPIO_InitStructure);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource8);

    EXTI_InitTypeDef  EXTI_InitStructure;

    EXTI_InitStructure.EXTI_Line    = EXTI_Line8;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;

    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;

    NVIC_Init(&NVIC_InitStructure);

}

void HS0038_ReadStartdata(void)
{
    uint32_t HS0038_count = 0;
    while(HS0038_READ == 1);
    // 1. 测量 9ms Low 脉冲
    // EXTI 已经在 Low 脉冲的起点触发，这里只测量其持续时间
    while (HS0038_READ == 0) 
    {
        HS0038_count++;
        Delay_us(1); // 1us 步进
        // 9ms = 9000us，设置 9500us 超时
        if(HS0038_count > 9500)
        {
            return; // 9ms 脉冲超时或错误
        }
    }

    // 2. 测量 4.5ms High 脉冲
    HS0038_count = 0;
    while (HS0038_READ == 1)
    {
        HS0038_count++;
        Delay_us(1); // 1us 步进
        // 4.5ms = 4500us，设置 5000us 超时
        if(HS0038_count > 5000)
        {
            return; // 4.5ms 脉冲超时或错误
        }
    }
    // 此时引脚进入第一个数据位的 Low 脉冲
}

/**
 * @brief  HS0038读取一个数据的函数
 * @param  None
 * @note   本函数读取一位的红外的数据
 * @retval None
 */

int8_t HS0038_ReadOnedata(void)
{  

    uint32_t HS0038_count = 0;
    while( HS0038_READ == 0)
    {
				HS0038_count++;			
				Delay_us(1);
				if(HS0038_count > 650)
				{
					return -3;
				}
			}

    Delay_us(800);  //延时0.8ms

    uint8_t pin_value = 0;

    if (HS0038_READ == 1)
    {
        pin_value =  1;
    }
    HS0038_count = 0;
    if (pin_value == 1)// 采样到高电平，等高电平走完
    {
        while(HS0038_READ == 1) //精算等待时间需要
        {
            HS0038_count++;
            Delay_us(1);
            if (HS0038_count > 1000)
            {
                break;
            }
        }
    }

    return pin_value;

}

/**
 * @brief  HS0038的数据移位函数
 * @param  None
 * @note   data = data | (1<<i);从右往左添加数据    LSB   
 *         data <<= 1;
 *         data |= 1;           从左往右添加数据    MSB
 * @retval None
 */

uint8_t HS0038_ReadBytedata(void)
{
    uint8_t data = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        if (HS0038_ReadOnedata() == 1)
        {
            data = data | (1<<i);   //从右往左添加数据     LSB

        }
        
    }
    return data;

}
/**
 * @brief  HS0038的数据读取函数
 * @param  None
 * @note   HS0038的数据读取的接口
 * @retval None
 */
void HS0038_Readdata(void)
{
    HS0038_ReadStartdata();//跳过起始数据

    for (uint8_t i = 0; i < 4; i++)
    {
        data_array[i] = HS0038_ReadBytedata();  //数据读取
    }
    
    if ((data_array[0] + data_array[1] ) != 0 || (data_array[2] + data_array[3]) != 0)
    {
        return;
    }
    
    Delay_us(600);  //跳过剩余的时序

}


void IR_EXTI_Enable(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_Line = EXTI_Line8;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling; 
    EXTI_InitStruct.EXTI_LineCmd = ENABLE; // 开启中断请求
    EXTI_Init(&EXTI_InitStruct);
}

void IR_EXTI_Disable(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_Line = EXTI_Line8;
    // 只关心 LineCmd，其他设置将被忽略或保留。
    EXTI_InitStruct.EXTI_LineCmd = DISABLE; // 关闭中断请求
    EXTI_Init(&EXTI_InitStruct);
}


/**
 * @brief  HS0038的外部中断函数
 * @param  None
 * @note   使用外部中断检测调用这个函数
 * @retval None
 */
void EXTI9_5_IRQHandler(void)	
{	
	// 1. 声明标志位，用于检查是否需要任务切换
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(EXTI_GetITStatus(EXTI_Line8) == SET)  				// 判断是否出发了外部中断0线
	{
		// EXTI_InitTypeDef EXTI_InitStruct;
        // EXTI_InitStruct.EXTI_LineCmd = DISABLE; // 禁用 EXTI
        // EXTI_Init(&EXTI_InitStruct);

        // 给信号量发送信号
      if (xIRSemaphore != NULL) {
            xSemaphoreGiveFromISR(xIRSemaphore, &xHigherPriorityTaskWoken);
        }
		
		// printf("receive data%X\r\n", data_array[2]);	// 经过测算，获取的地址码为0，所以可以不用
		
		EXTI_ClearITPendingBit(EXTI_Line8);	  				// 清除外部中断0线的中断挂起标志位
	}				
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}




