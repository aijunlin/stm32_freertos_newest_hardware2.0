/**
 ******************************************************************************
 * @file    ADC.c
 * @author  ajl
 * @version V0.0.1
 * @date    5-November-2025
 * @brief
 *
 ******************************************************************************
 * @attention
 *
 * 本文件主要包含了ADC1和ADC3两个初始化函数
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


uint16_t ADC1_buf[5] = {0};
uint16_t ADC3_buf[5] = {0};

static uint8_t ADC1_count; // 1s扫描一次0.1s容易跑飞
static uint8_t ADC3_count; // 1s扫描一次0.1s容易跑飞


/**
 * @brief  ADC的数据初始化函数
 * @param  None
 * @note   本函数初始化了硬件的ADC1的四个通道
 * @retval None
 */


void ADC1_Init(void){

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
		GPIO_InitTypeDef GPIO_InitStructure;
		
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;   //模拟
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_5;
		GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
	
		ADC_DeInit();//ADC复位
		ADC_CommonInitTypeDef ADC_CommonInitStruct;
		ADC_CommonInitStruct.ADC_DMAAccessMode		=ADC_DMAAccessMode_Disabled;
		ADC_CommonInitStruct.ADC_Mode				=ADC_Mode_Independent;
		ADC_CommonInitStruct.ADC_Prescaler			=ADC_Prescaler_Div2;
		ADC_CommonInitStruct.ADC_TwoSamplingDelay	=ADC_TwoSamplingDelay_5Cycles;

		ADC_CommonInit(&ADC_CommonInitStruct);

		ADC_InitTypeDef ADC_InitStructure;
		ADC_InitStructure.ADC_ContinuousConvMode	=ENABLE;    				 		//连续转换，单次转换
		ADC_InitStructure.ADC_DataAlign				=ADC_DataAlign_Right;  				//对齐方式右对齐
		ADC_InitStructure.ADC_ExternalTrigConv		=ADC_ExternalTrigConvEdge_None;		//外部打断关
		ADC_InitStructure.ADC_NbrOfConversion		=1;							 		//一个通道
		ADC_InitStructure.ADC_ScanConvMode			=DISABLE;							//adc扫描模式
		ADC_InitStructure.ADC_Resolution			=ADC_Resolution_12b;
	
		ADC_Init(ADC1,&ADC_InitStructure);
		
		
		
		ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);
		
	
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	
		ADC_Cmd(ADC1,ENABLE);  
	
}


uint16_t ADC1_Convert(uint8_t ADC_Channel)
{
	ADC_RegularChannelConfig(ADC1,ADC_Channel,1,ADC_SampleTime_480Cycles);//配置规则组的输入通道
	ADC_SoftwareStartConv(ADC1);
	while(RESET==ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
	return ADC_GetConversionValue(ADC1);
}



void ADC3_Init(void)
{
	/* 0、外设信息配置结构体*/
									//ADC外设基础信息配置结构体
							//ADC通用信息配置结构体
	//GPIO基础信息配置结构体

	/* 1、使能ADC3和GPIOF组时钟 */ 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);							//84MHZ	

	GPIO_InitTypeDef      GPIO_InitStructure;	/* 2、配置ADC3通道5作为模拟输入 */										
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_7;									//引脚：			第7个引脚
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AN;									//模式：			模拟模式
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;								//是否上下拉：   不拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);		

	//将配置的外设信息，写入到相对应的寄存器中

	/* 3、ADC通用配置 */
	ADC_CommonInitTypeDef ADC_CommonInitStructure;		
	ADC_CommonInitStructure.ADC_Mode 				= ADC_Mode_Independent;			//独立工作模式：1个ADC硬件独立工作
	ADC_CommonInitStructure.ADC_Prescaler 			= ADC_Prescaler_Div2;			//ADC的分频系数：84MHZ/2 = 42MHZ 	
	ADC_CommonInitStructure.ADC_DMAAccessMode 		= ADC_DMAAccessMode_Disabled;	//DMA模式：类似内存映射，不需要
	ADC_CommonInitStructure.ADC_TwoSamplingDelay 	= ADC_TwoSamplingDelay_5Cycles;	//两个采样点的时间间隔：5个时钟间隔
	ADC_CommonInit(&ADC_CommonInitStructure);										//将配置的外设信息，写入到相对应的寄存器中

	ADC_InitTypeDef       ADC_InitStructure;	/* 4、ADC3外设信息配置 */	
	ADC_InitStructure.ADC_Resolution 				= ADC_Resolution_12b;			//采样精度为12位
	ADC_InitStructure.ADC_ScanConvMode 				= DISABLE;						//是否多通道检测(单通道)			
	ADC_InitStructure.ADC_ContinuousConvMode 		= ENABLE;						//连续转换，单次转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge 		= ADC_ExternalTrigConvEdge_None;//禁用外部脉冲来让ADC工作
	ADC_InitStructure.ADC_ExternalTrigConv 			= ADC_ExternalTrigConv_T1_CC1;	//不需要外部触发
	ADC_InitStructure.ADC_DataAlign 				= ADC_DataAlign_Right;			//默认右对齐方式
	ADC_InitStructure.ADC_NbrOfConversion 			= 1;							//转换的通道数
	ADC_Init(ADC3, &ADC_InitStructure);

	/* 5、ADC3通道5配置(为所选的ADC配置相应的常规通道()及其采用时间) */
	/*
		ADC3： 					哪个ADC外设
		ADC_Channel_5：			ADC通道
		1：						表示转换序号，如果ADC需要对多通道进行转换的话，指定转换通道的顺序，实现排队转换
		ADC_SampleTime_3Cycles;	表示转换该采样点的时间间隔，3个时钟周期(3/42MHZ)
	*/
	
	ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);


	/* 6、清除ADC标志位 */
	ADC_ClearFlag(ADC3, ADC_FLAG_EOC);
	
	/* 7、使能ADC */
	ADC_Cmd(ADC3, ENABLE);
}


uint16_t ADC3_IN5_GetLightVal(void)
{
	uint16_t ret_val = 0;
	
	/* 1、启动ADC软件转换 */ 
	ADC_SoftwareStartConv(ADC3);
	
	/* 2、等待转换结果(没有转换完毕，就让其一直堵塞于此) */
	while(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC) == RESET);
	
	/* 3、得到转换后的值(这个不是电压值) */
	ret_val = ADC_GetConversionValue(ADC3);

	return ret_val;
}


uint16_t ADC3_Convert(uint8_t ADC_Channel)
{
	ADC_RegularChannelConfig(ADC3,ADC_Channel,1,ADC_SampleTime_480Cycles);//配置规则组的输入通道
	ADC_SoftwareStartConv(ADC3);
	while(RESET==ADC_GetFlagStatus(ADC3,ADC_FLAG_EOC));
	return ADC_GetConversionValue(ADC3);
}




void ADC1_Tick(void)
{
	ADC1_count++;
	if (ADC1_count >= 100) // 每1s更新一次
	{
		ADC1_count = 0;
		ADC1_buf[4] = ADC1_Convert(ADC_Channel_5);
	
	}
}


void ADC3_Tick(void)
{


	ADC3_count++;
	if (ADC3_count >= 100) // 每1s更新一次
	{
		ADC3_count = 0;
		ADC1_buf[4] = ADC3_Convert(ADC_Channel_5);
	
	}

}


