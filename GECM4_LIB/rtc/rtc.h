#ifndef __RTC_H					//定义以防止递归包含
#define __RTC_H

#include "stm32f4xx.h"
#include "my_reg.h"
#include "my_config.h"

extern void RTC_ConfigInit(void);			
extern void RTC_SetDateTime(uint8_t Year, uint8_t Month, uint8_t Date, uint8_t Hours, uint8_t Minutes, uint8_t Seconds);			
extern void RTC_Set_Alarm(void);			
extern void RTC_Alarm_IRQHandler(void);		
extern void RTC_Wkup_Config(void);
extern void  RTC_WKUP_IRQHandler(void);

#endif /* __RTC_H */
