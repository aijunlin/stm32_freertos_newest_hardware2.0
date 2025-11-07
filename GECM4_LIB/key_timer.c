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



uint8_t key_area_flag = 0;//选择的密码位
uint8_t key_num;//按键的键位状态
uint8_t key[4] = {0}; //存放密码的空间
uint8_t key_user[4] = {1,1,4,5};
uint8_t dis_range = 10;//反射的最小距离
uint8_t key_page_flag = 0;  //页面切换的标志位
uint8_t key_area_esp8266 = 0;
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
 * @brief  密码的操作函数
 * @param  None
 * @note   PA0 - key1  PE2 - key2 PE3 - key3 PE4 - key4
 * @retval None
 */

void Key_Key_Word_Page1(void)
{
	
	uint8_t key_num = key_Timer_Callback();	

	switch (key_num)
	{
	case KEY_TIMER_KEY1:
		key[key_area_flag] ++;
		break;
	case KEY_TIMER_KEY2:
		key[key_area_flag] --;
		break;
	case KEY_TIMER_KEY3:
		key_area_flag ++;
		break;
	case KEY_TIMER_KEY4:
		Key_User_Cmp();//密码确认函数
		break;
	default:
		break;
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
		key_page_flag = key_area_flag + 2; 		//确认按键  在这里切换页面
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
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;//抢占
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;//响应
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM5,ENABLE);

}
/**
 * @brief  这里是菜单页面的切换函数
 * @param  None
 * @note   PA0 - key1  PE2 - key2 PE3 - key3 PE4 - key4
 * @retval None
 */
void Key_Page_Switch(void)
{

	switch (key_page_flag)
	{
	case 0:
		Key_Timer_Page1();
		break;
	case 1 :
		Key_Timer_page_menuchoose();
		key_area_flag = 0; //清除翻转区域的标志位
		break;
	case 2 :
						  //在这里显示温湿度的页面
		break;
	case 3 :
		Key_Timer_page_esp8266();	
		OLED_Update();
		key_area_flag = 0;				
		break;
	case 4 :

		break;



	default:
		break;
	}
}

/**
 * @brief  密码页面的输入函数
 * @param  None
 * @note   本页面是密码输入的页面
 * @retval None
 */

void Key_Timer_Page1(void)
{
		OLED_ShowChinese(0,0,"请输入密码");	//注意汉字的编码要和文件的编码相同
		
		OLED_ShowNum(16,16,key[0],1,OLED_8X16);
		OLED_ShowNum(32,16,key[1],1,OLED_8X16);
		OLED_ShowNum(48,16,key[2],1,OLED_8X16);
		OLED_ShowNum(64,16,key[3],1,OLED_8X16);

		switch (key_area_flag)
		{
		case 0:
			OLED_ReverseArea(16,16,8,16);
			break;
		case 1:
			OLED_ReverseArea(32,16,8,16);
			break;
		case 2:
			OLED_ReverseArea(48,16,8,16);
			break;
		case 3:
			OLED_ReverseArea(64,16,8,16);
			break;
		default:
			if (key_area_flag >= 4)
			{
				key_area_flag = 0;
			}
			break;
		}

		//确认切换页面的操作



		OLED_Update();
		Key_Key_Word_Page1();	//按键扫描0-4切换函数
}

void Key_Key_Dis(void)
{
	uint8_t key_num = key_Timer_Callback();

	switch (key_num)
	{
	case KEY_TIMER_KEY1:
		dis_range += 5;
		break;
	case KEY_TIMER_KEY2:
		dis_range -= 5;
		break;
	case KEY_TIMER_KEY3:
		

		break;
	case KEY_TIMER_KEY4:
		

		break;
	default:
		break;
	}

}
/**
 * @brief  菜单页面函数
 * @param  None
 * @note   本函数是菜单页面函数，包含需要用的菜单供选择
 * @retval None
 */


void Key_Timer_page_menuchoose(void)
{

	OLED_ShowChinese(0,0,"温湿度数据"); //一个字宽度16
	// OLED_ShowNum(64,16,dis_range,1,OLED_8X16);	//设置超声波的响应距离的函数
	OLED_ShowString(0,16,"esp8266",OLED_8X16);
	OLED_ShowString(0,32,"DHT11",OLED_8X16);
	OLED_ShowString(0,48,"HC-05",OLED_8X16);


	switch (key_area_flag)
	{
	case 0:
		OLED_ReverseArea(0, 0, 80, 16);
		break;
	case 1:
		OLED_ReverseArea(0, 16, (sizeof("esp8266")-1)*8, 16); //选择进入然后开始连接网络
		break;
	case 2:		
		OLED_ReverseArea(0, 32, (sizeof("DHT11")-1)*8, 16);
		break;
	case 3:
		OLED_ReverseArea(0, 48, (sizeof("HC-05")-1)*8, 16);
		break;

	default:
		key_area_flag = 0;
		break;
	}

	OLED_Update(); //显存刷新函数
	Key_Key_Word_Pagemenu();	//按键扫描0-4切换函数




	//在这里做数据上传
}

void Key_Timer_page_esp8266(void)
{
	
#if 1
	uint8_t count = 1;
	if (count == 1)
	{
		OLED_Update();
		count = 0;
	}
	
#endif

	OLED_ShowString(0,0,"esp8266",OLED_8X16); //一个字宽度16
	if (esp8266.Esp8266_Start_Flag == 1)
	{

		// OLED_ClearArea(64,0,18,18);
		OLED_ShowChinese(64,0,"开");
	}
	if (esp8266.Esp8266_Start_Flag == 0)
	{
		// OLED_ClearArea(64,0,18,18);
		OLED_ShowChinese(64,0,"关");
	}

	OLED_ShowChinese(64,48,"退出");


	switch (esp8266.Esp8266_Ready_percentage)
	{
		
	case 1:
			OLED_ShowChinese(0,32,"正在初始化请等待中");
			OLED_ShowString(0,48,"30%",OLED_8X16);//在这里检测状态来控制进度条，增加标志位
		break;
	case 2:
			OLED_ShowChinese(0,32,"正在初始化请等待中");
			OLED_ShowString(0,48,"60%",OLED_8X16);//在这里检测状态来控制进度条，增加标志位
	case 3:
			OLED_ShowChinese(0,32,"正在初始化请等待中");
			OLED_ShowString(0,48,"100%",OLED_8X16);//在这里检测状态来控制进度条，增加标志位
		break;
	default:
		break;
	}

	switch (key_area_esp8266)
	{
	case 0:
		OLED_ReverseArea(64,0,16,16);
		OLED_UpdateArea(64,0,16,16); //显存刷新函数
		break;
	case 1:
		OLED_ReverseArea(64,48,32,16);
		OLED_UpdateArea(64,48,32,16); //显存刷新函数
		break;
	
	default:
		key_area_esp8266 = 0;
		break;
	}

	OLED_UpdateArea(64,0,18,18); //显存刷新函数
	Key_esp8266_choose();	//按键扫描0-4切换函数
	//在这里做数据上传

}



/**
 * @brief  这里是菜单页面的切换函数
 * @param  None
 * @note   PA0 - key1  PE2 - key2 PE3 - key3 PE4 - key4
 * @retval None
 */
void Key_esp8266_choose(void)
{
	
	uint8_t key_num = key_Timer_Callback();	

	switch (key_num)
	{
	case KEY_TIMER_KEY3:
		key_area_esp8266 ++;   			//读取到这里的页面的选项
		break;
	case KEY_TIMER_KEY4:
		if (key_area_esp8266 == 0)    		//在这里做esp8266的标志位处理
		{
			esp8266.Esp8266_Ready_Flag = 1;			//开始esp8266的初始化   //在这里再叠加一个快关的标志位，然后把开关的功能完善成功
			esp8266.Esp8266_Ready_percentage = 1;
		}

		if (key_area_esp8266 == 1)
		{
			key_page_flag = 1;			//选择了退出键
		}
		
		break;
	default:
		break;
	}

}

/**
 * @brief  密码验证的函数
 * @param  None
 * @note   本函数用来验证输入的密码是否正确
 * @retval None
 */

void Key_User_Cmp(void)
{
	if (memcmp(key, key_user, sizeof(key_user)) == 0) 	//密码正确
	{
		PFout(9)  = 1;
    	PFout(10) = 1;
    	PEout(13) = 1;
    	PEout(14) = 1;

		OLED_Clear();//清除上一页的数据
		key_page_flag = 1;

		//Key_Timer_Page2();//切换到倒车雷达的配置页面

	}
	else	//密码错误  //将密码清除
	{
		memset(key,0,sizeof(key));
	}
	
}

