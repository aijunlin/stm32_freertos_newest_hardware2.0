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
static TaskHandle_t ESP8266_messagereport_Task_handle;




// 任务函数声明
static void OLED_Task( void * pvParameters );
static void HC_SR04_Task( void * pvParameters );
static void ESP8266_Connect_Task( void * pvParameters );
static void ESP8266_Process_Task( void * pvParameters );
static void ESP8266_messagereport_Task( void * pvParameters );




char data_buffer[10] =  {0};

u8 g_buffer[32] = {0};
char g_rxbuffer[32] = {0};


void Hardware_Init(void)
{
    Key_Timer5_Init();//按键定时器初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置中断的分组为4
    key_Timer_Init();//按键初始化
	Uart_Init();//uart1主要用来打印
	Uart2_Init();//uart2主要用来接受蓝牙的传送信号
	LED_Init();
	BUZZER_Init();//蜂鸣器的初始化
	OLED_Init();//本地的oled的使用
	HC_SR04_Init();//超声波传感器用来接受液面的数据
	Esp8266_Init(115200);//初始化esp8266来发送检的数据
    motor_init();//电机初始化
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
	// 						4,				   // 任务创建时的优先等级(注意：优先级数字小表示任务优先级低(跟stm32相反)、优先级默认上限为 (configMAX_PRIORITIES - 1)。)
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
	// 						3,				   // 任务创建时的优先等级(注意：优先级数字小表示任务优先级低(跟stm32相反)、优先级默认上限为 (configMAX_PRIORITIES - 1)。)
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

    // xReturned = xTaskCreate(
	// 						ESP8266_messagereport_Task,		// 任务接口函数
	// 						"ESP8266_messagereport_Task",	   // 任务名字
	// 						512,			   // 任务堆栈的大小(注意：这个大小指字(4字节)，而非字节)
	// 						NULL,			   // 传递给任务的参数
	// 						3,				   // 任务创建时的优先等级(注意：优先级数字小表示任务优先级低(跟stm32相反)、优先级默认上限为 (configMAX_PRIORITIES - 1)。)
	// 						&ESP8266_messagereport_Task_handle // 任务控制块指针
	// );

	// if( xReturned == pdPASS )						// 创建任务成功
    // {
	// 	printf("ESP8266_messagereport_Task create!\r\n");
    // }	
	// else
	// {
	// 	printf("ESP8266_messagereport_Task create failed!\r\n");
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
        // OLED_ShowNum(32, 16, distance, 3, OLED_8X16);
        
        page_switch();

        // OLED_UpdateArea(32, 16, 24, 16);
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
    uint32_t temp = 0;

    while (1)
    {
        temp = HC_SR04_Get_Distance();
        if (temp != -1) 
        {
            distance = temp;
            printf("distance: %d cm\r\n", distance);

        }
        vTaskDelay( pdMS_TO_TICKS(1000) ); 
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
    // static TickType_t xLastHeartbeatTime = 0;
    
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
                vTaskDelay(pdMS_TO_TICKS(1000)); // 1秒后再继续任务
                xEventGroupSetBits(xESP8266EventGroup, 
                    ESP8266_BIT_SERVER_CONNECTED | ESP8266_BIT_READY|ESP8266_BIT_SEND_REQUEST);
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
            printf("ESP8266: Connection successful. Sending first heartbeat.\r\n");
            // ESP8266_SendHeartbeat(); 
            // xLastHeartbeatTime = xTaskGetTickCount();
        }
        // 空闲状态 - 连接状态监控
       else {
            // 只有当服务器连接成功且系统处于就绪状态时，才执行心跳检查
            // if (uxBits & ESP8266_BIT_READY) 
            // {
            //     TickType_t xCurrentTime = xTaskGetTickCount();
                
            //     // 检查是否到达心跳发送时间
            //     if ((xCurrentTime - xLastHeartbeatTime) > pdMS_TO_TICKS(HEARTBEAT_ECHO_TIMEOUT_MS)) 
            //     {
            //         // 1. **检查上次心跳以来是否收到回声确认**
            //         EventBits_t echo_bit = xEventGroupGetBits(xESP8266EventGroup) & ESP8266_BIT_HEARTBEAT_ECHO;
                    
            //         if (echo_bit) {
            //             // 收到回声，连接健康
            //             printf("heart beat /ACK success\r\n");
            //             // 清除回声位，等待下一轮心跳的确认
            //             xEventGroupClearBits(xESP8266EventGroup, ESP8266_BIT_HEARTBEAT_ECHO);
            //         } else {
            //             // 在整个 HEARTBEAT_INTERVAL_MS 周期内未收到回声，连接失败
            //             printf("heart beat receive error\r\n");
            //             // 设置重连位，并清除连接状态位，强制任务回到连接阶段
            //             xEventGroupSetBits(xESP8266EventGroup, ESP8266_BIT_RECONNECT);
            //             xEventGroupClearBits(xESP8266EventGroup, 
            //                 ESP8266_BIT_SERVER_CONNECTED | ESP8266_BIT_READY);
                        
            //             // 立即跳过发送步骤，进入下一个循环处理 RECONNECT
            //             continue;
            //         }
            //         // 2. 检查通过，发送新的心跳包
            //         xLastHeartbeatTime = xCurrentTime; // 立即更新时间戳
            //         printf("send new heart beat...\r\n");
            //         ESP8266_SendHeartbeat(); 
            //     }
            // } 
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
                // 注意：这里使用局部副本 local_recv_buffer //在这里使用互斥锁保护数据发送

                if (MY_LIB_CmpArray((int8_t *)local_recv_buffer, (int8_t *)"#heartbeat", 10) == 0)
                {
                    printf("receive heart beat echo\r\n");
                    // *** 核心改动: 收到回声，设置 EventBit 通知连接任务 ***
                    xEventGroupSetBits(xESP8266EventGroup, ESP8266_BIT_HEARTBEAT_ECHO);
                }
                
                else
                {
                    printf("ESP8266: failed to get mutex, data not processed.\r\n");
                }

              
                if (MY_LIB_CmpArray((int8_t *)local_recv_buffer, (int8_t *)"Green",5) == 0)
                {
                    strcpy(g_rxbuffer, "Green");
                }


            }
        // }

        // 延时一段时间，避免过于频繁的处理
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


void ESP8266_messagereport_Task(void *pvParameters)
{

    while (1)
    {
        EventBits_t uxBits = xEventGroupGetBits(xESP8266EventGroup);

        if ((uxBits & ESP8266_BIT_READY) != 0 && (uxBits & ESP8266_BIT_SEND_COMPLETE) == 0)
        {
            printf("ESP8266: begin send message ... \r\n");

            // 调用上报函数
            
            if (esp8266_DataReport(NULL) == 0) 
            {
                printf("ESP8266: message send success!\r\n");
                xEventGroupSetBits(xESP8266EventGroup, ESP8266_BIT_SEND_COMPLETE);
            } 
            
            // 不需要 vTaskDelay，任务处理完会自动回到 WaitBits 进入阻塞状态
        }
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}



