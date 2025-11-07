/**
 ******************************************************************************
 * @file    DHT11.c
 * @author  ajl
 * @version V0.0.1
 * @date    27-Octorber-2025
 * @brief   温湿度传感器数据的获取
 *
 ******************************************************************************
 * @attention
 *
 * 
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "DHT11.h" // Device header


uint8_t DHT11_Data[5] = {0};  //湿度温度校验和
uint8_t DHT11_start_flag = 1;
static uint16_t cnt = 0; 


/**
 * @brief  DHT11的初始化函数
 * @param  None
 * @note   初始化温湿度传感器，使用PG9 引脚进行收发
 * @retval None
 */

void DHT11_Init(void)
{
	// 0、GPIO片内外设信息初始化结构体
	GPIO_InitTypeDef  GPIO_InitStructure;

	// 1、使能GPIO片内外设的硬件时钟					
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	// 2、配置GPIO片内外设的引脚						
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_9;				// 引脚：第9根引脚
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;			// 模式：输出模式
	GPIO_InitStructure.GPIO_Speed   = GPIO_High_Speed;			// 速度：高速(100MHz)
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;			// 上下拉：不拉
	GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;			// 输出类型：推挽输出
	GPIO_Init(GPIOG, &GPIO_InitStructure);						// 使用该函数，将配置的信息写入到相应的寄存器中
	
	// 3、一开始将引脚设置为高电平
	DHT11_OUT = 1;
	
}

/**
 * @brief  单片机的引脚切换函数
 * @param  GPIO_MODE：传参用来改变引脚的选择输入输出
 * @note   切换PG9引脚来切换输入输出模式
 * @retval None
 */
void DHT11_Mode_Switch(uint8_t GPIO_MODE)
{

	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_9;				// 引脚：第9根引脚

    if (GPIO_MODE == GPIO_IN)
    {
        GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;			// 模式：输入模式
		GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;		// 上下拉：不拉
    }
    else
    {
        GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;		// 模式：输出模式
		GPIO_InitStructure.GPIO_Speed   = GPIO_High_Speed;		// 速度：高速(100MHz)
		GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;		// 上下拉：不拉
		GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;		// 输出类型：推挽输出
    }

    GPIO_Init(GPIOG, &GPIO_InitStructure);


}

/**
 * @brief  DHT11的开始时序的启动
 * @param  NONE
 * @note   DHT11传感器的启动启动触发时序
 * @retval 成功:success : 0   超时error ：-1
 */

int8_t DHT11_Start(void)
{
    uint32_t DHT11_Count = 0;
    DHT11_Mode_Switch(GPIO_OUT);
	
    DHT11_OUT = 0;
	
    Delay_ms(20);
	
    DHT11_OUT = 1;
	
    Delay_us(30);

    DHT11_Mode_Switch(GPIO_IN); // 转换成输入模式
	
	DHT11_Count = 0;
    while (DHT11_IN == 1) // 在这里做等待和超时判断
    {
        DHT11_Count++;
        Delay_us(1);
        if (DHT11_Count > 200) // 判断超时
        {
            return -1; // 准备阶段时序退出
        }
    }

    DHT11_Count = 0;
    while (DHT11_IN == 0) // 在这里做等待和超时判断
    {
        DHT11_Count++;
        Delay_us(1);
        if (DHT11_Count > 100) // 判断超时
        {
            return -1; // 准备阶段时序退出
        }
    }
	
	DHT11_Count = 0;
    while (DHT11_IN == 1)
    {
        DHT11_Count++;
        Delay_us(1);
        if (DHT11_Count > 100) // 判断超时
        {
            return -1; // 准备阶段时序退出
        }
    }
	return 0;
}

int8_t DHT11_ReadData(void)
{
    // 1. 等待 50us 低电平的到来（同步到低电平的起点）
    uint32_t DHT11_Count = 0;
    while (DHT11_IN == 0);
    
    // 2. 延时 40us，进入采样窗口（判断是 0 还是 1）
    // 此时低电平已结束，脉冲变为高电平
    Delay_us(40);
    
    int8_t bit_value = 0;
    if(DHT11_IN == 1)
    {
        bit_value = 1; // 采样到高电平，判定为 1
    }
    if (bit_value == 1)// 采样到高电平，进行超时判断防止无限等待
    {
        while(DHT11_IN == 1) 
        {
            DHT11_Count++;
            Delay_us(1);
            if (DHT11_Count > 40) // 确保不会无限等待
            {
                break; //超时退出
            }
        }
    }
    
    return bit_value;// 采样到低电平，判定为 0
}

uint8_t DHT11_Data_Assamble(void)
{

    uint8_t DHT11_Data_Temp = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
		DHT11_Data_Temp <<= 1;
		DHT11_Data_Temp |= DHT11_ReadData();//接受数据
        
    }

    return DHT11_Data_Temp;
}

int8_t DHT11_Data_Get(void)
{

    if (DHT11_Start() == -1)
    {
        return -1;
    }

    for (uint8_t i = 0; i < 5; i++)
    {
        DHT11_Data[i] = DHT11_Data_Assamble();
    }
    if ((DHT11_Data[0] + DHT11_Data[1] + DHT11_Data[2] + DHT11_Data[3]) != DHT11_Data[4]) //校验数据 //采集的数据 0是湿度 2是温度
    {
        return -1;//校验错误
    }
	
	Delay_us(100); //通信结束
	return 0;
}

void DHT11_Timer_Tick(void)
{
	cnt++;
    if (cnt > 600)  //计数6s
    {
		
        DHT11_start_flag = 1;
        cnt = 0;
    }
}

