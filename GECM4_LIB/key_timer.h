/**
  ******************************************************************************
  * @file    key_timer.h
  * @author  ajl  
  * @version V0.0.1
  * @date    26-Octorber-2025
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
#ifndef __KEY_TIMER_H
#define __KEY_TIMER_H


/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stm32f4xx.h"
#include "sys.h"
#include "OLED.h"
#include "led.h"
#include "esp8266.h"
#include "motor.h"



/* Exported types ------------------------------------------------------------*/



/* Exported constants --------------------------------------------------------*/
//按键1,2负责加减,按键3负责切换下一个,按键4负责确认


extern uint8_t key_area_flag;//按键的键位状态



enum key
{
  KEY_TIMER_KEY1 = 1, 
  KEY_TIMER_KEY2,
  KEY_TIMER_KEY3,
  KEY_TIMER_KEY4,
};

// typedef enum
// {
//     PAGE_AUTHENTIC = 0,
//     PAGE_MENU,
//     PAGE_MENU_ESP8266,
//     PAGE_MENU_DHT11,
// }PAGE_FLAG_t;


/* Exported functions --------------------------------------------------------*/

void key_Timer_Init(void);

void key_Timer_Tick(void);

uint8_t key_Timer_Callback(void);//按键调用的推荐接口

uint8_t key_Timer_Scan(void);

void Key_Timer5_Init(void);

void Key_Key_Word_Pagemenu(void);


void motor_page_menu(void);

void Key_Timer_page_menuchoose(void);

void hc05_page_menu(void);

void Key_hc05_page_Pagemenu(void);

void Key_motor_page_Pagemenu(void);


#endif /* __KEY_TIMER_H */

/*****END OF FILE****/


