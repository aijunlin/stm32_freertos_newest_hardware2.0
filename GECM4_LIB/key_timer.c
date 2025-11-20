/**
 ******************************************************************************
 * @file    key_timer.c
 * @author  ajl
 * @version V0.0.1
 * @date    26-Octorber-2025
 * @brief
 *
 ******************************************************************************
 * @attention
 *
 * 本函数测试外部中断来控制
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "key_timer.h"

static uint8_t cnt = 0;   //静态变量函数退出后值不会变
static uint8_t prestate,curstate;

uint8_t key_area_flag = 1;
uint8_t key_hc05_page_area_flag = 1;
uint8_t key_motor_page_area_flag = 2;

uint8_t key_num;//按键的键位状态

uint8_t key_page_flag = 0;  //页面切换的标志位

/**
 * @brief  key初始化函数
 * @param  None
 * @note   初始化 PA0 - key1  PE2 - key2 PE3 - key3 PE4 - key4
 * @retval None
 */

void key_Timer_Init(void)	//按键引脚的初始化
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;

    GPIO_Init(GPIOE, &GPIO_InitStruct);

}

/**
 * @brief  key的扫描函数
 * @param  None
 * @note   PA0 - key1  PE2 - key2 PE3 - key3 PE4 - key4
 * 		   按键默认的数据为0当按钮按下的时候会填充按键的数据到数据区
 * 		   单片机的快速扫描获取刚刚按下的数据
 * @retval None
 */


uint8_t key_Timer_Callback(void)//按键调用的推荐接口
{
	uint8_t temp;
	temp = key_num;
	key_num = 0;
	return temp;
}

/**
 * @brief  key的确认函数
 * @param  None
 * @note   PA0 - key1  PE2 - key2 PE3 - key3 PE4 - key4
 * @retval None
 */

uint8_t key_Timer_Scan(void)
{
	
	if (PAin(0) == 0)
	{
		return KEY_TIMER_KEY1;
	}
	if (PEin(2) == 0)
	{
		return KEY_TIMER_KEY2;
	}
	if (PEin(3) == 0)
	{
		return KEY_TIMER_KEY3;
	}
	if (PEin(4) == 0)
	{
		return KEY_TIMER_KEY4;
	}
	return 0;

}
/**
 * @brief  key的tick回调函数
 * @param  None
 * @note   PA0 - key1  PE2 - key2 PE3 - key3 PE4 - key4
 * @retval None
 */

void key_Timer_Tick(void)   //每20次时钟调用一次
{

	cnt ++;
	if(cnt>=2) //等待20ms
	{
		cnt =0;
		prestate = curstate;
		curstate = key_Timer_Scan();
		
		if((curstate == 0)&&(prestate != 0))   //捕获按键松手
		{
			key_num = prestate;
		}
	}
	
}


/**
 * @brief  这里是菜单页面的切换函数
 * @param  None
 * @note   PA0 - key1  PE2 - key2 PE3 - key3 PE4 - key4
 * @retval None
 */
void Key_Key_Word_Pagemenu(void)
{
	
	uint8_t key_num = key_Timer_Callback();	

	switch (key_num)
	{
	case KEY_TIMER_KEY1:

		break;
	case KEY_TIMER_KEY2:

		break;
	case KEY_TIMER_KEY3:
		key_area_flag ++;   			//读取到这里的页面的选项
		break;
	case KEY_TIMER_KEY4:
		key_page_flag  = key_area_flag;   			//读取到这里的页面的选项
        OLED_Clear();
		break;
	default:
		break;
	}

}

/**
 * @brief  这里是页面的切换函数
 * @param  None
 * @note   PA0 - key1  PE2 - key2 PE3 - key3 PE4 - key4
 * @retval None
 */
void page_switch(void)
{
    switch (key_page_flag)
    {
    case 0:
        Key_Timer_page_menuchoose();
        break;
    case 1:
        motor_page_menu();
        break;
    case 2:
        hc05_page_menu();
        break;
    default:
        break;
    }
}


/**
 * @brief  这里是HC-05的设置页面函数
 * @param  None
 * @note   PA0 - key1  PE2 - key2 PE3 - key3 PE4 - key4
 * @retval None
 */
void hc05_page_menu(void)
{
    OLED_ShowChinese(0,0,"当前液位:");
    OLED_ShowNum(80,0,distance,3,OLED_8X16);

    OLED_ShowChinese(0,16,"液位上预警:");
    OLED_ShowNum(88,16,distance_up,3,OLED_8X16);

    OLED_ShowChinese(0,32,"液位下预警:");
    OLED_ShowNum(88,32,distance_down,3,OLED_8X16);

    switch (key_hc05_page_area_flag)
	{
    case 1:
		OLED_ReverseArea(88, 16, 24, 16); 
		break;
	case 2:
		OLED_ReverseArea(88, 32, 24, 16);
		break;

	default:
		key_hc05_page_area_flag = 1;
		break;
	}

    OLED_Update(); //显存刷新函数
    Key_hc05_page_Pagemenu();
}

void Key_hc05_page_Pagemenu(void)
{

    uint8_t key_num = key_Timer_Callback();

    switch (key_num)
    {
    case KEY_TIMER_KEY1:
        if (key_hc05_page_area_flag == 1)
        {
            distance_up += 5;
        }
        if (key_hc05_page_area_flag == 2)
        {
            distance_down += 5;
        }
        break;
    case KEY_TIMER_KEY2:
        if (key_hc05_page_area_flag == 1)
        {
            distance_up -= 5;
        }
        if (key_hc05_page_area_flag == 2)
        {
            distance_down -= 5;
        }
        break;
    case KEY_TIMER_KEY3:
        key_hc05_page_area_flag++; // 读取到这里的页面的选项
        break;
    case KEY_TIMER_KEY4:
        key_page_flag = 0; // 直接返回第一页面
        OLED_Clear();
        break;
    default:
        break;
    }
}

/**
 * @brief  这里是电机的手动操作页面函数
 * @param  None
 * @note   PA0 - key1  PE2 - key2 PE3 - key3 PE4 - key4
 * @retval None
 */
void motor_page_menu(void)
{
    OLED_ShowChinese(0,0,"电机手动操作");

    OLED_ShowChinese(32,16,"开");
    if (motor_falg == 1)
    {
        OLED_ShowNum(80,16,1,1,OLED_8X16);  //显示开
    }
    OLED_ShowChinese(32,32,"关");
    if (motor_falg == 0)
    {
        OLED_ShowNum(80,32,1,1,OLED_8X16);  //显示关
    }

    OLED_Update(); //显存刷新函数
    Key_motor_page_Pagemenu();
}

void Key_motor_page_Pagemenu(void)
{

    uint8_t key_num = key_Timer_Callback();

    switch (key_num)
    {
    case KEY_TIMER_KEY1:
        motor_falg = 1;
        OLED_ClearArea(80, 8, 8, 40);
        break;
    case KEY_TIMER_KEY2:
        motor_falg = 0;
        OLED_ClearArea(80, 16, 8, 40);
        break;
    case KEY_TIMER_KEY3:
        break;
    case KEY_TIMER_KEY4:
        key_page_flag = 0;  // 直接返回第一页面
        motor_falg = 0;     //退出直接关闭电机
        OLED_Clear();
        break;
    default:
        break;
    }
}

/*
函数:定时器5使用
参数:无
时间片扫描定时器在用
*/
void Key_Timer5_Init(void)
{
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;   //默认为0
	TIM_TimeBaseStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStruct.TIM_Period=100-1;    //定时1s    0~65536    72m/   /   1ms  
	TIM_TimeBaseStruct.TIM_Prescaler=8400-1;					
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStruct);//配置时基单元


	TIM_ITConfig(TIM5, TIM_IT_Update,ENABLE);//中断输出控制开   cc1捕获通道的


	TIM_ClearFlag(TIM5,TIM_IT_Update);//防止配置完成的标志位影响中断
		
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=TIM5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=5;//抢占
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;//响应
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM5,ENABLE);

}

/**
 * @brief  菜单页面函数
 * @param  None
 * @note   本函数是菜单页面函数，包含需要用的菜单供选择
 * @retval None
 */


void Key_Timer_page_menuchoose(void)
{
    OLED_ShowChinese(0,32,"电机手动");     //手动页面可以手动开电机
	OLED_ShowString(0,48,"HC-05",OLED_8X16);    //超声波页面可以手动切换触发的最高液位和最低液位


	switch (key_area_flag)
	{
    case 1:
		OLED_ReverseArea(0, 32, 64, 16); 
		break;
	case 2:
		OLED_ReverseArea(0, 48, (sizeof("HC-05")-1)*8, 16);
		break;

	default:
		key_area_flag = 1;
		break;
	}

	OLED_Update(); //显存刷新函数
	Key_Key_Word_Pagemenu();	//按键扫描0-4切换函数
	//在这里做数据上传
}




 void TIM5_IRQHandler(void)   //轮循检测   10ms/tick
 {
 	if(TIM_GetITStatus(TIM5,TIM_IT_Update) == SET)
 	{
		
 		key_Timer_Tick();	//按键的扫描

 		TIM_ClearITPendingBit(TIM5,TIM_IT_Update);//定时器中断清除
 	}

 }


