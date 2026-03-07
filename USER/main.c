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
 *	V0.0.4	2-November-2025 完成了esp8266时序控制，完成了相关数据的上报
 *							修复了OLED刷新率过低的问题 
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


#define ESP8266_DEBUG 1

EventGroupHandle_t xESP8266EventGroup = NULL;

SemaphoreHandle_t xHardwareMutex = NULL;


// 任务控制块指针(相当于Linux的线程标识符)
static TaskHandle_t OLED_Task_handle;
static TaskHandle_t HC_SR04_Task_handle;
static TaskHandle_t Motor_Task_handle;
static TaskHandle_t ESP8266_Connect_Task_handle;
static TaskHandle_t ESP8266_Process_Task_handle;
static TaskHandle_t ESP8266_messagereport_Task_handle;




// 任务函数声明
static void OLED_Task( void * pvParameters );
static void HC_SR04_Task( void * pvParameters );
static void Motor_Task( void * pvParameters );
static void ESP8266_Connect_Task( void * pvParameters );
static void ESP8266_Process_Task( void * pvParameters );
static void ESP8266_messagereport_Task( void * pvParameters );



void Hardware_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置中断的分组为4
    Key_Timer5_Init();//按键定时器初始化
    key_Timer_Init();//按键初始化
	Uart_Init();//uart1主要用来打印调试信息
	BUZZER_Init();//蜂鸣器的初始化
	OLED_Init();//本地的oled的使用
	HC_SR04_Init();//超声波传感器用来接受液面的数据
    motor_init();//电机初始化
	Esp8266_Init(115200);//初始化esp8266来发送检的数据
}


// 主函数
int main(void)
{

	Hardware_Init();

    Load_Distance_From_Flash();
	// (1)、相关变量区域
	BaseType_t xReturned;	// 任务创建函数的返回值
	
	// 创建硬件互斥信号量（必须添加）
	xHardwareMutex = xSemaphoreCreateMutex();
	if (xHardwareMutex == NULL) {
		printf("Hardware Mutex create failed!\r\n");
		while(1); // 互斥信号量创建失败，系统无法继续
	} else {
		printf("Hardware Mutex create success!\r\n");
	}



	// 创建ESP8266事件组（必须添加）
	xESP8266EventGroup = xEventGroupCreate();
	if (xESP8266EventGroup == NULL) {
		printf("ESP8266 Event Group create failed!\r\n");
		while(1); // 事件组创建失败，系统无法继续
	} 
    else 
    {
		printf("ESP8266 Event Group create success!\r\n");
	}



    // (3)、RTOS任务的创建并开启调度				 
    //oled的任务
    xReturned = xTaskCreate(
								OLED_Task,       	// 任务接口函数
								"OLED_Task",        // 任务名字
								256,      		 	// 任务堆栈的大小(注意：这个大小指字(4字节)，而非字节)
								NULL,    		 	// 传递给任务的参数
								4,					// 任务创建时的优先等级(注意：优先级数字小表示任务优先级低(跟stm32相反)、优先级默认上限为 (configMAX_PRIORITIES - 1)。)
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
							256,			   // 任务堆栈的大小(注意：这个大小指字(4字节)，而非字节)
							NULL,			   // 传递给任务的参数
							4,				   // 任务创建时的优先等级(注意：优先级数字小表示任务优先级低(跟stm32相反)、优先级默认上限为 (configMAX_PRIORITIES - 1)。)
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

    // 2、创建任务，并存储其标识符
	xReturned = xTaskCreate(
							Motor_Task,		// 任务接口函数
							"Motor_Task",	   // 任务名字
							128,			   // 任务堆栈的大小(注意：这个大小指字(4字节)，而非字节)
							NULL,			   // 传递给任务的参数
							4,				   // 任务创建时的优先等级(注意：优先级数字小表示任务优先级低(跟stm32相反)、优先级默认上限为 (configMAX_PRIORITIES - 1)。)
							&Motor_Task_handle // 任务控制块指针
	);

	if( xReturned == pdPASS )				    // 创建任务成功
    {
		printf("Motor_Task create!\r\n");
    }	
	else
	{
		printf("Motor_Task create failed!\r\n");
		// 可以在这里处理创建失败的情况
	}




	#if ESP8266_DEBUG	
	//esp8266的连接任务
	xReturned = xTaskCreate(
							ESP8266_Connect_Task,		// 任务接口函数
							"ESP8266_Connect_Task",	   // 任务名字
							256,			   // 任务堆栈的大小(注意：这个大小指字(4字节)，而非字节)
							NULL,			   // 传递给任务的参数
							5,				   // 任务创建时的优先等级(注意：优先级数字小表示任务优先级低(跟stm32相反)、优先级默认上限为 (configMAX_PRIORITIES - 1)。)
							&ESP8266_Connect_Task_handle // 任务控制块指针
	);

	if( xReturned == pdPASS )						// 创建任务成功
    {
		printf("ESP8266_Connect_Task create!\r\n");
    }	
	else
	{
		printf("ESP8266_Connect_Task create failed!\r\n");
		// 可以在这里处理创建失败的情况
	}

    // 3、创建任务，并存储其标识符
	xReturned = xTaskCreate(
							ESP8266_Process_Task,		// 任务接口函数
							"ESP8266_Process_Task",	    // 任务名字
							256,			            // 任务堆栈的大小(注意：这个大小指字(4字节)，而非字节)
							NULL,			            // 传递给任务的参数
						    5,				            // 任务创建时的优先等级(注意：优先级数字小表示任务优先级低(跟stm32相反)、优先级默认上限为 (configMAX_PRIORITIES - 1)。)
							&ESP8266_Process_Task_handle // 任务控制块指针
	);

	if( xReturned == pdPASS )						// 创建任务成功
    {
		printf("ESP8266_Process_Task create!\r\n");
    }	
	else
	{
		printf("ESP8266_Process_Task create failed!\r\n");
		// 可以在这里处理创建失败的情况
	}

    xReturned = xTaskCreate(
							ESP8266_messagereport_Task,		// 任务接口函数
							"ESP8266_messagereport_Task",	   // 任务名字
							512,			   // 任务堆栈的大小(注意：这个大小指字(4字节)，而非字节)
							NULL,			   // 传递给任务的参数
							5,				   // 任务创建时的优先等级(注意：优先级数字小表示任务优先级低(跟stm32相反)、优先级默认上限为 (configMAX_PRIORITIES - 1)。)
							&ESP8266_messagereport_Task_handle // 任务控制块指针
	);

	if( xReturned == pdPASS )						// 创建任务成功
    {
		printf("ESP8266_messagereport_Task create!\r\n");
    }	
	else
	{
		printf("ESP8266_messagereport_Task create failed!\r\n");
		// 可以在这里处理创建失败的情况
	}
    #endif

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
        page_switch();
        vTaskDelay(pdMS_TO_TICKS(20));
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
    int32_t temp = 0;

    while (1)
    {
        // 尝试获取互斥锁，最多等待 50ms
        // 如果 50ms 还拿不到（说明 Wi-Fi 卡住了），就放弃这次测距，等下一个周期
        if (xSemaphoreTake(xHardwareMutex, pdMS_TO_TICKS(50)) == pdTRUE)
        {
            // 拿到锁，安全环境，开始测距！
            temp = HC_SR04_Get_Distance();
            
            // 测距完成，立刻释放互斥锁
            xSemaphoreGive(xHardwareMutex);
            
            if (temp != -1) 
            {

                uint32_t filtered_temp = Distance_Filter(temp);

                distance = (100 - filtered_temp);

                // 自动运行的区域水位控制逻辑
                if (distance >= 100) distance = 100;

                // 逻辑判断：控制电机和蜂鸣器
                if (distance <= distance_down) motor_flag = 1;
                else if (distance >= distance_up) motor_flag = 0;

                if ((distance >= distance_up + 5) || (distance <= distance_down - 5)) BUZZER(ON);
                else BUZZER(OFF);

                printf("distance: %d cm\r\n", temp);
            }
        }
        else
        {
            // 没有拿到锁，打印一条调试信息
            printf("HC_SR04: wait for Wi-Fi... skip this time.\r\n");
        }
        
        //同样改用 211ms 这种非整数倍节拍
        vTaskDelay(pdMS_TO_TICKS(211)); 
    }
}
/**
  * @brief  任务2函数实现
  * @note   实现电机控制
  * @param  None
  * @retval None
  */
static void Motor_Task(void *pvParameters)
{
    while (1)
    {
        if (motor_flag == 1)
        {
            GPIO_SetBits(GPIOA, GPIO_Pin_4);
        }
        else
        {
            GPIO_ResetBits(GPIOA, GPIO_Pin_4);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
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
        
        // 1. 错误处理阶段
        if (uxBits & ESP8266_BIT_ERROR) {
            printf("ESP8266: error status, retry after 5s ... \r\n");
            vTaskDelay(pdMS_TO_TICKS(3000));
            xEventGroupClearBits(xESP8266EventGroup, ESP8266_BIT_ERROR);
            xEventGroupSetBits(xESP8266EventGroup, ESP8266_BIT_RECONNECT);
            retry_count = 0;
            continue;
        }
        
        // 2. 重新连接请求阶段
        if (uxBits & ESP8266_BIT_RECONNECT)
        {
            printf("ESP8266: begin retry ... \r\n");
            // 【必须把 ESP8266_BIT_RECONNECT 也加进清除列表里】
            xEventGroupClearBits(xESP8266EventGroup,
                                 ESP8266_BIT_WIFI_CONNECTED | ESP8266_BIT_SERVER_CONNECTED | ESP8266_BIT_READY | ESP8266_BIT_RECONNECT);
        }

        // 3. WiFi 连接阶段
        if (!(uxBits & ESP8266_BIT_WIFI_CONNECTED)) {
            printf("ESP8266: begin link WiFi ... \r\n");
            
            // 调用 WiFi 连接函数
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
                vTaskDelay(pdMS_TO_TICKS(2000)); // 失败后 2秒重试
            }
        }
        // 4. 服务器连接阶段 (前提是 WiFi 已经连上)
        else if (!(uxBits & ESP8266_BIT_SERVER_CONNECTED)) {
            printf("ESP8266: begin link server ... \r\n");
            
            // 调用服务器连接函数
            if (ESP8266_ConnectMyYunServer("stm32") == 0) {
                printf("ESP8266: server link success! \r\n");
                vTaskDelay(pdMS_TO_TICKS(1000)); // 给底层一点缓冲时间
                
                // 设置全部就绪标志位
                xEventGroupSetBits(xESP8266EventGroup, 
                    ESP8266_BIT_SERVER_CONNECTED | ESP8266_BIT_READY | ESP8266_BIT_SEND_REQUEST);
                retry_count = 0;
                printf("ESP8266: system ready, can send message! \r\n");
            } else {
                retry_count++;
                printf("ESP8266: server link error %d/%d\r\n", retry_count, max_retry);
                if (retry_count >= max_retry) {
                    xEventGroupSetBits(xESP8266EventGroup, ESP8266_BIT_ERROR);
                }
                vTaskDelay(pdMS_TO_TICKS(3000)); // 失败后 3秒重试
            }
        }
        // 5. 空闲保持阶段 (网络和服务器都已连接)
        else {
            // 目前已移除心跳包逻辑。
            // 只要连接正常，任务会在这里不断循环并执行底部的 vTaskDelay，处于极低功耗的空转状态。
        }
        
        // 让出 CPU 资源给其他任务 (如电机控制、超声波测距)
        vTaskDelay(pdMS_TO_TICKS(100)); 
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

               vTaskSuspendAll(); // 使用临界区保护全局共享数据的读/写和状态清除

                printf("u3_recvbuf == %s\r\n", uart3_recvbuf);
                strcpy(local_recv_buffer, (const char *)uart3_recvbuf);

                // 清空相关数据，方便下次接收
                Uart_DataArray_clear((uint8_t *)uart3_recvbuf, 128);
                uart3_flag = 0;

               xTaskResumeAll();

                // 4. 解析数据并控制外设
                // 注意：这里使用局部副本 local_recv_buffer //在这里使用互斥锁保护数据发送

                if (MY_LIB_CmpArray((int8_t *)local_recv_buffer, (int8_t *)"#heartbeat", 10) == 0)
                {
                    printf("receive heart beat echo\r\n");
                    // *** 核心改动: 收到回声，设置 EventBit 通知连接任务 ***
                    xEventGroupSetBits(xESP8266EventGroup, ESP8266_BIT_HEARTBEAT_ECHO);
                }
                //接收电机的远程控制命令
                if (strstr(local_recv_buffer, "on") != NULL)
                {
                    motor_flag = 1;
                }
                if (strstr(local_recv_buffer, "off") != NULL)
                {
                    motor_flag = 0;
                }
                if (strstr(local_recv_buffer, "up") && strstr(local_recv_buffer, "down"))
                {
                    uint32_t up_value = 0;
                    uint32_t down_value = 0;

                    // 直接在这里使用sscanf提取数据
                    if(sscanf(local_recv_buffer, "up%ddown%d", &up_value, &down_value) == 2)
                    {
                        distance_up = up_value;
                        distance_down = down_value;

                        Save_Distance_To_Flash();
                    }
                    else
                    {
                        printf("数据格式不正确，无法提取数值\n");
                    }
                }
            }
            // }

            // 延时一段时间，避免过于频繁的处理
            vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
  * @brief  ESP8266消息上报任务
  * @note   负责上报本地的状态
  * @param  pvParameters: 任务参数
  * @retval None
  */
void ESP8266_messagereport_Task(void *pvParameters)
{

while (1)
    {
        EventBits_t uxBits = xEventGroupGetBits(xESP8266EventGroup);

        if ((uxBits & ESP8266_BIT_READY) != 0 && (uxBits & ESP8266_BIT_SEND_COMPLETE) == 0)
        {
            // 获取互斥锁，最多等待 1000ms。拿到锁才允许操作硬件外设
            if (xSemaphoreTake(xHardwareMutex, pdMS_TO_TICKS(1000)) == pdTRUE)
            {
                printf("ESP8266: begin send message ... \r\n");

                // 调用上报函数
                esp8266_DataReport(NULL);
                
                // 【核心细节】：加一点点延时。ESP8266 收到串口指令到射频发送完毕需要时间
                // 这 50ms 能确保底层射频真正安静下来，不再产生干扰
                vTaskDelay(pdMS_TO_TICKS(50)); 
                
                // 释放互斥锁，允许超声波开始工作
                xSemaphoreGive(xHardwareMutex);
            }
        }
        
        // 【防周期重合优化】：从 2000 错开，使用如 2011 这样的素数
        vTaskDelay(pdMS_TO_TICKS(2011)); 
    }
}



