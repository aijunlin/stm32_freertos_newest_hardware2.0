/**
 ******************************************************************************
 * @file    main.c
 * @author  ajl
 * @version V0.0.5
 * @date    5-November-2025
 * @brief
 ******************************************************************************
 * @attention
 * @email aj149415@outlook.com
 * @Warehouse https://gitee.com/ajjjjjjjjjjj/stm32f407.git
 *
 *	V0.0.1	完成了基本的代码框架
 *
 *
 *	V0.0.4	2-November-2025 完成了esp8266时序控制，完成了相关数据的上报 完成了AT24C02代码的测试，
 *							修复了OLED刷新率过低的问题 
 *
 *  V0.0.5	5-November-2025 完成了红外中断的数据转换bug的修复 完成了AT24C02代码的测试 完成了w25q128
 *                          代码的测试，完成了adc代码的移植，完成了rtc的代码的移植	
 *           
 *			                待完成：adc代码的测试 rtc两份代码的测试 bkp内容的整理 flash代码的移植和测试
 * 			
 ******************************************************************************
 */

 /* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "FreeRTOS.h"	// 选编译配置文件，用于汇总所有源文件的编译选择控制。
#include "task.h"		// 任务相关函数


// 任务控制块指针(相当于Linux的线程标识符)
static TaskHandle_t OLED_Task_handle;
static TaskHandle_t DHT11_Task_handle;

// 任务函数声明
static void OLED_Task( void * pvParameters );
static void DHT11_Task( void * pvParameters );




void Hardware_Init(void)
{

	Uart_Init();
	Uart2_Init();
	LED_Init();
	KEY_Init();
	BUZZER_Init();

	OLED_Init();
	DHT11_Init();


}


// 主函数
int main(void)
{

	Hardware_Init();
	// (1)、相关变量区域
	BaseType_t xReturned;	// 任务创建函数的返回值
	

	
	// (3)、RTOS任务的创建并开启调度				 
	// 1、创建任务，并存储其标识符
    xReturned = xTaskCreate(
								OLED_Task,       	// 任务接口函数
								"OLED_Task",        // 任务名字
								512,      		 	// 任务堆栈的大小(注意：这个大小指字(4字节)，而非字节)
								NULL,    		 	// 传递给任务的参数
								5,					// 任务创建时的优先等级(注意：优先级数字小表示任务优先级低(跟stm32相反)、优先级默认上限为 (configMAX_PRIORITIES - 1)。)
								&OLED_Task_handle 	// 任务控制块指针
						    );  

    if( xReturned == pdPASS )						// 创建任务成功
    {
		printf("OLED_Task create!\r\n");
    }
	
	// 2、创建任务，并存储其标识符
    xReturned = xTaskCreate(
								DHT11_Task,       	// 任务接口函数
								"DHT11_Task",        // 任务名字
								512,      		 	// 任务堆栈的大小(注意：这个大小指字(4字节)，而非字节)
								NULL,    		 	// 传递给任务的参数
								6,					// 任务创建时的优先等级(注意：优先级数字小表示任务优先级低(跟stm32相反)、优先级默认上限为 (configMAX_PRIORITIES - 1)。)
								&DHT11_Task_handle 	// 任务控制块指针
						    );  

    if( xReturned == pdPASS )						// 创建任务成功
    {
		printf("DHT11_Task create!\r\n");
    }
	

	
	// (4)、开启任务调度(堵塞于此，让任务可以运行起来)
	vTaskStartScheduler();			
	
	
}
 
/**
  * @brief  任务1函数实现
  * @note   实现led灯亮灭
  * @param  None
  * @retval None
  */
static void OLED_Task( void * pvParameters)
{
	while (1)
	{

		
		printf("OLED_Task run!\r\n");
		// OLED_ShowNum(0, 16, esp8266.Esp8266_Report_Flag1, 5, OLED_8X16); // 使用memcmp接口对比函数
		OLED_ShowNum(32, 16, DHT11_Data[0], 2, OLED_8X16);
		OLED_ShowNum(64, 16, DHT11_Data[2], 2, OLED_8X16);
		// OLED_ShowString_ascii(0,32,data,OLED_8X16);//显示接受到的数据
		OLED_Update();
		vTaskDelay(1);

	}
}


/**
  * @brief  任务2函数实现
  * @note   实现蜂鸣器的响和不响
  * @param  None
  * @retval None
  */
static void DHT11_Task(void *pvParameters)
{
	while (1)
	{
		DHT11_Data_Get();
		printf("DHT11_Task run!\r\n");
		printf("huminity %d\r\n",DHT11_Data[0]);
		printf("temperature %d\r\n",DHT11_Data[2]);
		vTaskDelay( pdMS_TO_TICKS( 6000 ) ); // 延时6000ms

	}
}




