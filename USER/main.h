/**
  ******************************************************************************
  * @file    main.h
  * @author  ajl  
  * @version V0.0.1
  * @date    20-Octorber-2025
  * @brief   
  *          
  ******************************************************************************
  * @attention
  *
  * 
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H



// 一、其它头文件
#include <stdio.h>

#include "my_config.h"
#include "my_lib.h"
#include "my_reg.h"
#include "sys.h"
#include "led.h"
#include "key.h"
#include "buzzer.h"
#include "uart.h"
#include "delay.h"
#include "OLED.h"
#include "dht11.h"
#include "HC_SR04.h"
#include "esp8266.h"
#include "FreeRTOS.h"	// 选编译配置文件，用于汇总所有源文件的编译选择控制。
#include "task.h"		// 任务相关函数
#include "semphr.h" 	// 信号量相关函数
#include "event_groups.h"
#include "HS0038.h"
#include "ADC.h"
#include "ws2812.h"



// 二、宏定义(函数、变量、常量)
// ESP8266状态标志位定义
#define ESP8266_BIT_WIFI_CONNECTED    (1 << 0)  // WiFi已连接
#define ESP8266_BIT_SERVER_CONNECTED  (1 << 1)  // 服务器已连接
#define ESP8266_BIT_READY             (1 << 2)  // 系统就绪
#define ESP8266_BIT_SEND_REQUEST      (1 << 3)  // 发送数据请求
#define ESP8266_BIT_SEND_COMPLETE     (1 << 4)  // 数据发送完成
#define ESP8266_BIT_ERROR             (1 << 5)  // 错误状态
#define ESP8266_BIT_RECONNECT         (1 << 6)  // 需要重新连接
#define ESP8266_BIT_HEARTBEAT_ECHO    (1 << 7) // 新增：心跳回声标志



// 三、自定义的数据类型(结构体、联合体、枚举等)

// 四、全局变量声明

extern EventGroupHandle_t xESP8266EventGroup;



// 五、函数声明


// 六、静态变量、函数定义

#endif /* __MAIN_H */

