/**
 ******************************************************************************
 * @file    main.c
 * @author  ajl
 * @version V0.1.2
 * @date    9-November-2025
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
 * 	V0.1.0  7-November-2025 完成了freertos的代码框架移植，开始进行任务的调度测试
 *
 * 	V0.1.1  8-November-2025 完成了任务的调度测试，完成了oled dht11 hc-sr04 模块的引入和测试
 * 
 * 	V0.1.2  9-November-2025 完成了任务的调度测试，完成了esp8266模块的引入和测试 完成了hs0038模块的引入和测试
 * 
 * 	V0.1.2  11-November-2025 开始裁剪原项目
 * 
 * 
 ******************************************************************************
 */

 /* Includes ------------------------------------------------------------------*/

#include "main.h"


EventGroupHandle_t xESP8266EventGroup = NULL;


// 任务控制块指针(相当于Linux的线程标识符)
static TaskHandle_t OLED_Task_handle;
static TaskHandle_t HC_SR04_Task_handle;
static TaskHandle_t ESP8266_Connect_Task_handle;
static TaskHandle_t ESP8266_Process_Task_handle;



// 任务函数声明
static void OLED_Task( void * pvParameters );
static void HC_SR04_Task( void * pvParameters );
static void ESP8266_Connect_Task( void * pvParameters );
static void ESP8266_Process_Task( void * pvParameters );



uint32_t distance;



u8 g_buffer[32] = {0};
char g_rxbuffer[32] = {0};


void Hardware_Init(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置中断的分组为4

	Uart_Init();//uart1主要用来打印
	Uart2_Init();//uart2主要用来接受蓝牙的传送信号
	LED_Init();
	KEY_Init();
	BUZZER_Init();//蜂鸣器的初始化
	OLED_Init();//本地的oled的使用
	HC_SR04_Init();//超声波传感器用来接受液面的数据
	Esp8266_Init(115200);//初始化esp8266来发送检的数据
}


// 主函数
int main(void)
{

	Hardware_Init();
	// (1)、相关变量区域
	BaseType_t xReturned;	// 任务创建函数的返回值
	

	// 创建ESP8266事件组（必须添加）
	xESP8266EventGroup = xEventGroupCreate();
	if (xESP8266EventGroup == NULL) {
		printf("ESP8266 Event Group create failed!\r\n");
		while(1); // 事件组创建失败，系统无法继续
	} else {
		printf("ESP8266 Event Group create success!\r\n");
	}

	
	// (3)、RTOS任务的创建并开启调度				 
    //oled的任务
    xReturned = xTaskCreate(
								OLED_Task,       	// 任务接口函数
								"OLED_Task",        // 任务名字
								512,      		 	// 任务堆栈的大小(注意：这个大小指字(4字节)，而非字节)
								NULL,    		 	// 传递给任务的参数
								3,					// 任务创建时的优先等级(注意：优先级数字小表示任务优先级低(跟stm32相反)、优先级默认上限为 (configMAX_PRIORITIES - 1)。)
								&OLED_Task_handle 	// 任务控制块指针
						    );  

    if( xReturned == pdPASS )						// 创建任务成功
    {
		printf("OLED_Task create!\r\n");
	}
	else
	{
		printf("OLED_Task create failed!\r\n");
		// 可以在这里处理创建失败的情况
	}

	// 3、创建任务，并存储其标识符
	xReturned = xTaskCreate(
							HC_SR04_Task,		// 任务接口函数
							"HC_SR04_Task",	   // 任务名字
							512,			   // 任务堆栈的大小(注意：这个大小指字(4字节)，而非字节)
							NULL,			   // 传递给任务的参数
							1,				   // 任务创建时的优先等级(注意：优先级数字小表示任务优先级低(跟stm32相反)、优先级默认上限为 (configMAX_PRIORITIES - 1)。)
							&HC_SR04_Task_handle // 任务控制块指针
	);

	if( xReturned == pdPASS )						// 创建任务成功
    {
		printf("HC_SR04_Task create!\r\n");
    }	
	else
	{
		printf("HC_SR04_Task create failed!\r\n");
		// 可以在这里处理创建失败的情况
	}
			
	// //esp8266的连接任务
	// xReturned = xTaskCreate(
	// 						ESP8266_Connect_Task,		// 任务接口函数
	// 						"ESP8266_Connect_Task",	   // 任务名字
	// 						512,			   // 任务堆栈的大小(注意：这个大小指字(4字节)，而非字节)
	// 						NULL,			   // 传递给任务的参数
	// 						3,				   // 任务创建时的优先等级(注意：优先级数字小表示任务优先级低(跟stm32相反)、优先级默认上限为 (configMAX_PRIORITIES - 1)。)
	// 						&ESP8266_Connect_Task_handle // 任务控制块指针
	// );

	// if( xReturned == pdPASS )						// 创建任务成功
    // {
	// 	printf("ESP8266_Connect_Task create!\r\n");
    // }	
	// else
	// {
	// 	printf("ESP8266_Connect_Task create failed!\r\n");
	// 	// 可以在这里处理创建失败的情况
	// }

    // // 3、创建任务，并存储其标识符
	// xReturned = xTaskCreate(
	// 						ESP8266_Process_Task,		// 任务接口函数
	// 						"ESP8266_Process_Task",	   // 任务名字
	// 						512,			   // 任务堆栈的大小(注意：这个大小指字(4字节)，而非字节)
	// 						NULL,			   // 传递给任务的参数
	// 						4,				   // 任务创建时的优先等级(注意：优先级数字小表示任务优先级低(跟stm32相反)、优先级默认上限为 (configMAX_PRIORITIES - 1)。)
	// 						&ESP8266_Process_Task_handle // 任务控制块指针
	// );

	// if( xReturned == pdPASS )						// 创建任务成功
    // {
	// 	printf("ESP8266_Process_Task create!\r\n");
    // }	
	// else
	// {
	// 	printf("ESP8266_Process_Task create failed!\r\n");
	// 	// 可以在这里处理创建失败的情况
	// }

	// (4)、开启任务调度(堵塞于此，让任务可以运行起来)
	vTaskStartScheduler();			

}
 
/**
  * @brief  任务1函数实现
  * @note   实现led灯亮灭
  * @param  None
  * @retval None
  */
static void OLED_Task(void *pvParameters)
{
    while (1)
    {   
        OLED_ShowNum(32, 16, distance, 3, OLED_8X16);
        OLED_UpdateArea(32, 16, 24, 16);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

/**
  * @brief  任务3函数实现
  * @note   实现超声波测距 采集的速率1s一次
  * @param  None
  * @retval None
  */
static void HC_SR04_Task(void *pvParameters)
{

	while (1)
	{
		distance = HC_SR04_Get_Distance();
		printf("distance = %dmm\r\n", distance);

		vTaskDelay( pdMS_TO_TICKS(1000)); 

	}
}

/**
  * @brief  ESP8266连接任务
  * @note   负责WiFi连接和服务器连接，预留消息上报接口
  * @param  pvParameters: 任务参数
  * @retval None
  */
static void ESP8266_Connect_Task(void *pvParameters)
{ 
    uint8_t retry_count = 0;
    const uint8_t max_retry = 3;
    
    printf("ESP8266 connect task start ... \r\n");
    
    while (1) {
        EventBits_t uxBits = xEventGroupGetBits(xESP8266EventGroup);
        
        // 错误处理
        if (uxBits & ESP8266_BIT_ERROR) {
            printf("ESP8266: error status, retry after 5s ... \r\n");
            vTaskDelay(pdMS_TO_TICKS(5000));
            xEventGroupClearBits(xESP8266EventGroup, ESP8266_BIT_ERROR);
            xEventGroupSetBits(xESP8266EventGroup, ESP8266_BIT_RECONNECT);
            retry_count = 0;
            continue;
        }
        
        // 重新连接请求
        if (uxBits & ESP8266_BIT_RECONNECT) {
            printf("ESP8266: begin retry ... \r\n");
            xEventGroupClearBits(xESP8266EventGroup, 
                ESP8266_BIT_WIFI_CONNECTED | ESP8266_BIT_SERVER_CONNECTED | ESP8266_BIT_READY);
            // 继续执行连接流程
        }
        
        // WiFi连接阶段
        if (!(uxBits & ESP8266_BIT_WIFI_CONNECTED)) {
            printf("ESP8266: begin link WiFi ... \r\n");
            
            // 这里调用您的WiFi连接函数
            if (ESP8266_OnLineInit() == 0) {
                printf("ESP8266: WiFi连接成功! \r\n");
                xEventGroupSetBits(xESP8266EventGroup, ESP8266_BIT_WIFI_CONNECTED);
                retry_count = 0;
            } else {
                retry_count++;
                printf("ESP8266: link error %d/%d\r\n", retry_count, max_retry);
                if (retry_count >= max_retry) {
                    xEventGroupSetBits(xESP8266EventGroup, ESP8266_BIT_ERROR);
                }
                vTaskDelay(pdMS_TO_TICKS(3000)); // 3秒后重试
            }
        }
        // 服务器连接阶段
        else if (!(uxBits & ESP8266_BIT_SERVER_CONNECTED)) {
            printf("ESP8266: begin link server ... \r\n");
            
            // 这里调用您的服务器连接函数
            if (ESP8266_ConnectMyYunServer("stm32") == 0) {
                printf("ESP8266: server link success! \r\n");
                xEventGroupSetBits(xESP8266EventGroup, 
                    ESP8266_BIT_SERVER_CONNECTED | ESP8266_BIT_READY);
                retry_count = 0;
                printf("ESP8266: system ready, can send message! \r\n");
            } else {
                retry_count++;
                printf("ESP8266: server link error %d/%d\r\n", retry_count, max_retry);
                if (retry_count >= max_retry) {
                    xEventGroupSetBits(xESP8266EventGroup, ESP8266_BIT_ERROR);
                }
                vTaskDelay(pdMS_TO_TICKS(5000)); // 5秒后重试
            }
        }
        // 消息上报处理（为您预留的接口）
        else if (uxBits & ESP8266_BIT_SEND_REQUEST) {
            printf("ESP8266: begin send message ... \r\n");
            
            // ============================================
            // 这里是消息上报接口 - 您可以在这里编写上报逻辑
            // ============================================
            
            // 示例：调用您的消息上报函数
            // if (ESP8266_SendMessage(data_buffer, data_length) == 0) {
            //     printf("ESP8266: 消息上报成功！\r\n");
            //     xEventGroupSetBits(xESP8266EventGroup, ESP8266_BIT_SEND_COMPLETE);
            //     xEventGroupClearBits(xESP8266EventGroup, ESP8266_BIT_SEND_REQUEST);
            // } else {
            //     printf("ESP8266: 消息上报失败！\r\n");
            //     xEventGroupSetBits(xESP8266EventGroup, ESP8266_BIT_ERROR);
            // }
            
            // 临时模拟成功上报（供您测试）
            printf("ESP8266: message send interface ready, wait for your implementation ... \r\n");
            vTaskDelay(pdMS_TO_TICKS(1000)); // 模拟上报过程
            xEventGroupSetBits(xESP8266EventGroup, ESP8266_BIT_SEND_COMPLETE);
            xEventGroupClearBits(xESP8266EventGroup, ESP8266_BIT_SEND_REQUEST);
            printf("ESP8266: 消息上报完成！\r\n");
        }
        // 空闲状态 - 连接状态监控
        else {
            // 定期检查连接状态（可选）
            static uint32_t last_check_time = 0;
            if (xTaskGetTickCount() - last_check_time > pdMS_TO_TICKS(30000)) { // 30秒检查一次
                // 这里可以添加连接状态检查逻辑
                // if (ESP8266_CheckConnection() != 0) {
                //     printf("ESP8266: 连接断开，需要重新连接\r\n");
                //     xEventGroupSetBits(xESP8266EventGroup, ESP8266_BIT_RECONNECT);
                // }
                last_check_time = xTaskGetTickCount();
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); // 让出CPU
    }
}

/**
  * @brief  ESP8266运行任务
  * @note   负责处理ESP8266的接收数据，做对应的任务操作
  * @param  pvParameters: 任务参数
  * @retval None
  */
void ESP8266_Process_Task(void *pvParameters)
{

    char local_recv_buffer[128]; // 用于本地处理的安全副本
    while (1)
    {
        // 1. 等待 UART3 接收完成信号
        // 使用较长的超时时间，或 portMAX_DELAY
        // if (xSemaphoreTake(xUART3_RxSemaphore, portMAX_DELAY) == pdTRUE)
        // {
            // 2. 检查是否处于透传模式，并且 flag 被设置
            // 注意：在 FreeRTOS 中，我们应依赖信号量，但这里为了移植兼容您的裸机 flag 逻辑：
            if ((esp8266_transparent_transmission_sta == 1) && (uart3_flag == 1))
            {

                taskENTER_CRITICAL(); // 使用临界区保护全局共享数据的读/写和状态清除

                printf("u3_recvbuf == %s\r\n", uart3_recvbuf);
                strcpy(local_recv_buffer, (const char *)uart3_recvbuf);

                // 清空相关数据，方便下次接收
                Uart_DataArray_clear((uint8_t *)uart3_recvbuf, 128);
                // uart3_count = 0; // 如果是全局变量也需要保护
                uart3_flag = 0;
                // uart3_len = 0;   // 如果是全局变量也需要保护

                taskEXIT_CRITICAL();

                // 4. 解析数据并控制外设
                // 注意：这里使用局部副本 local_recv_buffer
                if (MY_LIB_CmpArray((int8_t *)local_recv_buffer, (int8_t *)"BUZZER_ON", 9) == 0)
                {
                    strcpy(g_rxbuffer, "HCL41");
                }
                else if (MY_LIB_CmpArray((int8_t *)local_recv_buffer, (int8_t *)"BUZZER_OFF", 10) == 0)
                {
                    strcpy(g_rxbuffer, "HCL21");
                }
            }
        // }

        // 延时一段时间，避免过于频繁的处理
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


