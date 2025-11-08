
/**
  ******************************************************************************
  * @file    uart.h
  * @author  ajl  
  * @version V0.0.2
  * @date    23-Octorber-2025
  * @brief   
  *          
  ******************************************************************************
  * @attention
  *
  * 
  *
  * 
  * 
  * 
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_H
#define __UART_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "key.h"
#include "OLED.h"



/* Exported types ------------------------------------------------------------*/


#define DATA_SIZE 10
#define RX3_BUF_SIZE 128
#define ESP8266_DEBUG 1



/* Exported constants --------------------------------------------------------*/


extern uint16_t data[DATA_SIZE];
extern uint16_t data_temp[DATA_SIZE];


//后期打包成一个结构体
extern uint16_t uart3_count;
extern int8_t   uart3_flag;
extern uint16_t uart3_len;
extern uint8_t  uart3_recvbuf[RX3_BUF_SIZE];



/* Exported functions --------------------------------------------------------*/


void Uart_Init(void);

void Uart2_Init(void);

void Uart3_Init(uint32_t baud_rate);

void Uart_Data_Cpy(uint16_t *data , uint16_t *data_temp);

void Uart_Data_Cmp(void);

void Uart_Data_clear(void);

void USART1_SendStr(const char *str);

void USART2_SendStr(const char *str);

void USART3_SendStr(const char *str);

void Uart_DataArray_clear(uint8_t *data,uint16_t len);


int8_t MY_LIB_CmpArray(int8_t *p1_buf, int8_t *p2_buf, uint16_t len);

#endif /* __DELAY_H */



/*****END OF FILE****/


