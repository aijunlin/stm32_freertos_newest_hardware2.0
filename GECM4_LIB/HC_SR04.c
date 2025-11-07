/**
 ******************************************************************************
 * @file    HC_SR04.c
 * @author  ajl
 * @version V0.0.1
 * @date    24-Octorber-2025
 * @brief   0.0.1超声波传感器的驱动函数
 *
 ******************************************************************************
 * @attention
 *
 * 本函数测试外部中断来控制
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "HC_SR04.h" // Device header

volatile uint32_t Dis_up = 0;
volatile uint32_t Dis_down = 0;
volatile uint8_t Dis_flag = 0;
uint8_t HC_SR04_start_flag = 0;
static uint8_t cnt = 0;   //静态变量函数退出后值不会变

/**
 * @brief  hc_sr04初始化函数
 * @param  None
 * @note   初始化超声波传感器 pd7发射   pc6接收
 * @retval None
 */

 void HC_SR04_TIM8_Init(void)
 {
    /* 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
    
    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 ;  
    GPIO_Init( GPIOC, &GPIO_InitStruct);
    
    /* 将引脚连接上定时器2 */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);

    
    /* 定时器基础配置 */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;          //配置自动重装载寄存器为最大值  
    TIM_TimeBaseStructure.TIM_Prescaler = 168-1;        //计数频率1mhz   1us
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
    
    /* 通道一配置*/
    TIM_ICInitTypeDef  TIM_ICInitStruct;
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStruct.TIM_ICFilter = 0xf;                      //使用最大滤波
    TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;  //通道1使用上升沿计数
    TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInit(TIM8, &TIM_ICInitStruct);
    
    /* 通道二配置*/
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStruct.TIM_ICFilter = 0xf;
    TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Falling;  //通道二使用下降沿计数
    TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_IndirectTI;
    TIM_ICInit(TIM8, &TIM_ICInitStruct);

    /* 通道使能*/
    TIM_CCxCmd(TIM8,TIM_Channel_1,TIM_CCx_Enable);
    TIM_CCxCmd(TIM8,TIM_Channel_2,TIM_CCx_Enable);

    /* 中断开启*/
    TIM_ITConfig(TIM8,TIM_IT_CC1, ENABLE);
    TIM_ITConfig(TIM8,TIM_IT_CC2, ENABLE);
    
    /* 配置NVIC */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM8_CC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    /* 设置从模式触发门模式，当通道1有效电平到来时开启计数器，通道1无效电平关闭计数器*/
    /* 降低功耗，及时关闭定时器*/
    TIM_SelectInputTrigger(TIM8, TIM_TS_TI1FP1);
    TIM_SelectSlaveMode(TIM8, TIM_SlaveMode_Gated);
    
    /* 使能定时器开始计数 */
    TIM_Cmd(TIM8, ENABLE);

 }


void HC_SR04_Init(void)
{
   
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
    
    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed  = GPIO_High_Speed;
    GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_7 ;  
    GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP;  

    GPIO_Init( GPIOD, &GPIO_InitStruct);
    
    HC_SR04_TIM8_Init();
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
    uint32_t pulse_time_us = 0;
    uint32_t distance_cm = 0;
    uint32_t timeout = 0xFFFFFF; 

    // 1. 重置标志，并确保 Trig 引脚先拉低 (Trig 脉冲是高电平)
    Dis_flag = 0;
    PDout(7) = 0;
    Delay_us(5); // 确保低电平间隔

    // 2. 发送 Trig 脉冲 >= 10us
    PDout(7) = 1;
    Delay_us(15);
    PDout(7) = 0; // **必须将 Trig 脉冲拉低**

    // 3. 等待 ISR (下降沿中断) 设置 Dis_flag
    while ((Dis_flag == 0) && (timeout--)) 
    {
        // 等待捕获完成
    }

    // 4. 超时处理
    if (Dis_flag == 0)
    {
        return 0; // 捕获失败，返回 0
    }
    
    // 5. 计算时间差 (单位是 us，因为 T_count = 1us)
    // 假设未发生溢出，直接相减
    if (Dis_down >= Dis_up)
    {
        pulse_time_us = Dis_down - Dis_up;
    }
    else
    {
        // 发生了定时器溢出，需要特殊处理，这里简化为返回 0
        return 0; 
    }

    // 6. 计算距离 (D = t / 58.3)
    // 采用除法：
    // distance_cm = pulse_time_us / 58; 
    
    // 或者更精确的整数乘法： D = (t * 10) / 583
    distance_cm = (pulse_time_us * 10) / 583;

    /* 测量周期建议60ms以上*/

    return distance_cm;
}

void TIM8_CC_IRQHandler(void)
{

    //捕获上升沿的中断
    if (TIM_GetITStatus(TIM8, TIM_IT_CC1) != RESET) 
    {
        Dis_up = TIM_GetCapture1(TIM8);//读取cc1的值
        TIM_ClearITPendingBit(TIM8, TIM_IT_CC1);
    }

    //捕获下降沿的中断
    if (TIM_GetITStatus(TIM8, TIM_IT_CC2) != RESET) 
    {
        Dis_down = TIM_GetCapture2(TIM8);//读取cc2的值
        Dis_flag = 1;//通知可以开始计算
        TIM_ClearITPendingBit(TIM8, TIM_IT_CC2);
    }
    

}


void HC_SR04_Timer_Tick(void)
{

	if(cnt>=50) //等待500ms
	{
		cnt =0;
		HC_SR04_start_flag = 1;
	}

}



