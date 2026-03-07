/**
 ******************************************************************************
 * @file    HC_SR04.c
 * @author  ajl
 * @version V0.0.3
 * @date    11-November-2025
 * @brief   超声波传感器的驱动函数
 ******************************************************************************
 * @attention
 *
 * V0.0.2  24-Octorber-2025 完成了超声波传感器的驱动函数
 * V0.0.3  11-November-2025 完成了超声波传感器的距离测量函数在freertos中的实现
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "HC_SR04.h" // Device header


// 添加宏定义
#define HC_SR04_TIMEOUT_US       30000   // 30ms超时
#define HC_SR04_MIN_DISTANCE_CM   2       // 最小有效距离
#define HC_SR04_MAX_DISTANCE_CM   400     // 最大有效距离
#define HC_SR04_ERROR_VALUE       0xFFFFFFFF  // 错误值

#define FILTER_WEIGHT  60  //新数据占 30% 权重，历史数据占 70% 权重


#define TRIG_H()   PEout(8) = 1;
#define TRIG_L()   PEout(8) = 0;


volatile uint32_t Dis_up = 0;
volatile uint32_t Dis_down = 0;
volatile uint8_t Dis_flag = 0;
volatile uint8_t Dis_rise_ok = 0;
uint8_t HC_SR04_start_flag = 0;
static uint8_t cnt = 0;   //静态变量函数退出后值不会变

// SemaphoreHandle_t xHCSR04Semaphore = NULL;
uint32_t distance;
uint32_t distance_up = 100;     //最高水位
uint32_t distance_down = 0;   //最低水位

/**
 * @brief  hc_sr04初始化函数
 * @param  None
 * @note   初始化超声波传感器 pe8发射   pe9接收
 * @retval None
 */

 void HC_SR04_Init(void)
 {

    /* 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
    
    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed  = GPIO_High_Speed;
    GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_8 ;  
    GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP;  

    GPIO_Init( GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 ;  
    GPIO_Init( GPIOE, &GPIO_InitStruct);
    
    /* 将引脚连接上定时器2 */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);

    
    /* 定时器基础配置 */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;          //配置自动重装载寄存器为最大值  
    TIM_TimeBaseStructure.TIM_Prescaler = 168-1;        //计数频率1mhz   1us
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    
    /* 通道一配置*/
    TIM_ICInitTypeDef  TIM_ICInitStruct;
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStruct.TIM_ICFilter = 0xf;                      //使用最大滤波
    TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;  //通道1使用上升沿计数
    TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInit(TIM1, &TIM_ICInitStruct);
    
    /* 通道二配置*/
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStruct.TIM_ICFilter = 0xf;
    TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Falling;  //通道二使用下降沿计数
    TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_IndirectTI;
    TIM_ICInit(TIM1, &TIM_ICInitStruct);

    /* 通道使能*/
    TIM_CCxCmd(TIM1,TIM_Channel_1,TIM_CCx_Enable);
    TIM_CCxCmd(TIM1,TIM_Channel_2,TIM_CCx_Enable);

    /* 中断开启*/
    TIM_ITConfig(TIM1,TIM_IT_CC1, ENABLE);
    TIM_ITConfig(TIM1,TIM_IT_CC2, ENABLE);
    
    /* 配置NVIC */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    /* 设置从模式触发门模式，当通道1有效电平到来时开启计数器，通道1无效电平关闭计数器*/
    /* 降低功耗，及时关闭定时器*/
    // TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);
    // TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Gated);
    
    /* 使能定时器开始计数 */
    TIM_Cmd(TIM1, ENABLE);

 }


/**
 * @brief  hc_sr04的距离计数函数
 * @param  None
 * @note   初始化超声波传感器 pd7发射   pc6接收
 *         首先由pd7发射计数的起始波，然后使用pc6来接收
 * @retval None
 */


uint32_t HC_SR04_Get_Distance(void)
{ 
    uint32_t pulse_time_us  = 0;
    uint32_t distance_cm    = 0;
    uint32_t start_time     = 0;
    uint32_t rise_time      = 0;
    uint32_t fall_time      = 0;

    // 1. 初始化状态
    Dis_flag = 0;
    Dis_rise_ok = 0;
    Dis_up   = 0;
    Dis_down = 0;
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
    
    TRIG_L();
    Delay_us(5);
    // 2. 发送触发脉冲
    TRIG_H();
    Delay_us(15);
    TRIG_L();

    // 3. 等待回波（带超时）
   start_time = TIM1->CNT;
    while (Dis_flag == 0) {
    // 强制转换为 uint16_t，利用二进制回绕特性安全计算差值
    if ((uint16_t)(TIM1->CNT - start_time) > (uint16_t)HC_SR04_TIMEOUT_US) {
        return HC_SR04_ERROR_VALUE; // 超时无回波
    }
    // 最好加入这一行，释放 CPU 给其他任务，防止同优先级任务饿死
    // vTaskDelay(pdMS_TO_TICKS(1)); 
}

    // 4. 处理定时器数据
    // taskENTER_CRITICAL();
    rise_time = Dis_up;
    fall_time = Dis_down;
    // taskEXIT_CRITICAL();

    if (fall_time >= rise_time) {
        pulse_time_us = fall_time - rise_time;
    } else {
        pulse_time_us = (0xFFFF - rise_time) + fall_time + 1;
    }

    // 5. 转换为距离
    distance_cm = (pulse_time_us * 10 + 291) / 583; // 四舍五入

    // 6. 检查距离范围
    if (distance_cm < HC_SR04_MIN_DISTANCE_CM || distance_cm > HC_SR04_MAX_DISTANCE_CM) {
        return HC_SR04_ERROR_VALUE;
    }

    return distance_cm;
}

/**
 * @brief  带重试的距离获取
 */
uint32_t HC_SR04_Get_Distance_With_Retry(uint8_t max_retries)
{
    uint32_t distance = 0;
    uint8_t retry_count = 0;
    
    for (retry_count = 0; retry_count < max_retries; retry_count++) {
        distance = HC_SR04_Get_Distance();
        
        if (distance != HC_SR04_ERROR_VALUE) {
            return distance;
        }
        
        Delay_ms(30); // 重试间隔
    }
    
    return HC_SR04_ERROR_VALUE;
}

void TIM1_CC_IRQHandler(void)
{
    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;

 // 捕获上升沿的中断
    if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET) 
    {
        Dis_up = TIM_GetCapture1(TIM1);
        Dis_rise_ok = 1;
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
    }

    // 捕获下降沿的中断
    if (TIM_GetITStatus(TIM1, TIM_IT_CC2) != RESET) 
    {
        if (Dis_rise_ok == 1)
        {
            Dis_down = TIM_GetCapture2(TIM1);
            Dis_flag = 1;
            Dis_rise_ok = 0;
        }
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
    }
    
    // // 如果需要，进行任务切换
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

uint32_t Distance_Filter(uint32_t new_data)
{
    static uint32_t last_val = 0;
    static uint8_t is_first = 1;

    // 1. 刚开机时的第一次采集，直接作为历史基准，防止从 0 开始缓慢爬升
    if (is_first) 
    {
        last_val = new_data;
        is_first = 0;
        return new_data;
    }

    // 2. 核心滤波计算 (全整数运算，极速搞定)
    last_val = (new_data * FILTER_WEIGHT + last_val * (100 - FILTER_WEIGHT)) / 100;

    return last_val;
}


void HC_SR04_Timer_Tick(void)
{

	if(cnt>=50) //等待500ms
	{
		cnt =0;
		HC_SR04_start_flag = 1;
	}

}

