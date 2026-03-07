/**
  ******************************************************************************
  * @file    esp8266.h
  * @author  ajl  
  * @version V0.0.1
  * @date    28-Octorber-2025
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
#ifndef __ESP8266_H
#define __ESP8266_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "delay.h"
#include "uart.h"
#include "key.h"
#include "key_timer.h"
#include "HC_SR04.h"
#include "motor.h"

/* Exported types ------------------------------------------------------------*/

#define SSID "ajl"
#define PSWD "112234556678"

// 你的ubuntu系统服务器的协议模式、ip地址、端口号
#define SOCKET_MODE "TCP"
#define SOCKET_IP   "8.138.8.218"
#define SOCKET_PORT 50001


#define HEARTBEAT_MESSAGE               "#heartbeat\r\n"
#define HEARTBEAT_ECHO_TIMEOUT_MS       10000   // 等待服务器回声的超时时间 (60秒)



/* Exported constants --------------------------------------------------------*/

extern int8_t esp8266_transparent_transmission_sta;

struct esp8266_start
{   
    int8_t Esp8266_Ready_Flag;          //esp8266的连接准备
    int8_t Esp8266_Start_Flag;          //esp8266的连接云服务器的标志
    int8_t Esp8266_Report_Flag;         //esp8266的上报的标记
    int8_t Esp8266_Report_Flag1;
    uint8_t Esp8266_Ready_percentage;   //标记初始化的百分比
};

extern struct esp8266_start esp8266;




/* Exported functions --------------------------------------------------------*/

void Esp8266_Init(uint32_t baud_rate);

int8_t ESP8266_OnLineInit(void);

int8_t ESP8266_ExitTransparentTransmission(void);

void ESP8266_SendATCmd(const char* cmd_str);

int8_t ESP8266_CheckStrInRxbuf(char* str, int32_t timeout);

int8_t ESP8266_ConnectAp(char *ssid, char *pswd);

int8_t ESP8266_EnEcho(int8_t flag);

int8_t ESP8266_Rst(void);

int8_t ESP8266_ConnectMyYunServer(char* reg_msg);

void ESP8266_SendMsg(const char* msg_str);

int8_t ESP8266_ConnectServer(char *mode, char *ip, uint16_t port);

int8_t ESP8266_EnterTransparentTransmission(void);

void esp8266_test_demo(void);

void esp8266_Timer_Tick(void);

uint8_t esp8266_DataReport(const char* msg_str);

void  ESP8266_SendHeartbeat(void);
#endif /* __ESP8266_H */



/*****END OF FILE****/


