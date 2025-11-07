#ifndef __ADC_H					//定义以防止递归包含
#define __ADC_H

#include "stm32f4xx.h"
#include "my_reg.h"
#include "my_config.h"

extern void ADC3_IN5_LightInit(void);				// 1、ADC3_IN5光敏传感器初始化(PF7引脚)
extern uint16_t ADC3_IN5_GetLightVal(void);			// 2、获取ADC值
extern void ADC1_IN5_PotentiometerInit(void);		// 3、ADC12_IN5电位器器初始化(PA5引脚)
extern uint16_t ADC1_IN5_GetPotentiometerVal(void);	// 4、获取电位器ADC值

#endif /* __ADC_H */
