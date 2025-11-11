/*
 * ws2812.h
 * 描述: WS2812B (SPI+DMA) 驱动头文件 (FreeRTOS 兼容)
 */

#ifndef __WS2812_H
#define __WS2812_H

#include "stm32f4xx.h"
#include "FreeRTOS.h"   // 必须包含，用于 RTOS 延迟
#include "task.h"       // 必须包含，用于 pdMS_TO_TICKS 宏

// ***********************************************
// *** 用户必须配置的参数            ***
// ***********************************************
//
//  !!! 修改这里为你灯带的实际灯珠总数 !!!
//  你之前的代码中使用了60，这里保持一致
//
#define Pixel_S1_NUM 60 
// ***********************************************


// WS2812 0/1 码的SPI表示 (基于你 84MHz APB2 -> 5.25MHz SPI 的配置)
#define CODE0   0xC0  // 0.38us 高电平 (T0H)
#define CODE1   0xF8  // 0.95us 高电平 (T1H)

// 颜色结构体 (注意：WS2812 的顺序是 GRB)
typedef struct {
    uint8_t G;
    uint8_t R;
    uint8_t B;
} RGBColor_TypeDef;

// --- 预定义颜色 (来自你的代码) ---
extern const RGBColor_TypeDef YRED;
extern const RGBColor_TypeDef YGREEN;
extern const RGBColor_TypeDef YBLUE;
extern const RGBColor_TypeDef YSKY;
extern const RGBColor_TypeDef YMAGENTA;
extern const RGBColor_TypeDef YYELLOW;
extern const RGBColor_TypeDef YORANGE;
extern const RGBColor_TypeDef YBLACK;
extern const RGBColor_TypeDef YWHITE;
extern const RGBColor_TypeDef YPURPLE;
extern const RGBColor_TypeDef YPINK;


// --- 驱动核心函数 (来自你的代码) ---
/**
 * @brief  初始化 SPI 和 DMA
 */
void WS2812b_Configuration(void);

/**
 * @brief  设置单个灯珠的颜色 (写入缓冲区，不发送)
 * @param  LedId: 灯珠编号 (0 到 Pixel_S1_NUM - 1)
 * @param  Color: 颜色
 */
void rgb_SetColor(uint16_t LedId, RGBColor_TypeDef Color);

/**
 * @brief  通过 DMA 将缓冲区的数据发送到灯带
 */
void rgb_SendArray(void);


// --- 辅助函数 ---
/**
 * @brief  设置所有灯珠为同一颜色 (不发送)
 * @param  Color: 颜色
 */
void rgb_SetAll(RGBColor_TypeDef Color);

/**
 * @brief  关闭所有灯珠 (不发送)
 */
void rgb_ClearAll(void);

/**
 * @brief  颜色环，用于彩虹效果
 * @param  WheelPos: 0-255
 * @return 对应的颜色
 */
RGBColor_TypeDef Colourful_Wheel(uint8_t WheelPos);


// --- 非阻塞灯效函数 (RTOS 安全) ---
// 注意：这些函数会“霸占”当前任务直到特效完成，但会通过 vTaskDelay() 允许其他任务运行。

/**
 * @brief  彩虹循环
 * @param  cycles: 循环次数
 * @param  delay_ms: 每一帧的延迟 (毫秒)
 */
void ws_effect_rainbow_cycle(uint16_t cycles, uint16_t delay_ms);

/**
 * @brief  颜色擦除 (流水灯)
 * @param  color: 目标颜色
 * @param  delay_ms: 每个灯点亮的延迟 (毫秒)
 */
void ws_effect_color_wipe(RGBColor_TypeDef color, uint16_t delay_ms);

/**
 * @brief  剧院追逐 (跑马灯)
 * @param  color: 目标颜色
 * @param  delay_ms: 每一帧的延迟 (毫秒)
 */
void ws_effect_theater_chase(RGBColor_TypeDef color, uint16_t delay_ms);

/**
 * @brief  扫描 (从中间向两边)
 * @param  color: 目标颜色
 * @param  delay_ms: 每一帧的延迟 (毫秒)
 */
void ws_effect_scan(RGBColor_TypeDef color, uint16_t delay_ms);

/**
 * @brief  呼吸灯
 * @param  color: 目标颜色
 * @param  delay_ms: 每次亮度变化的延迟 (毫秒)
 */
void ws_effect_breathe(RGBColor_TypeDef color, uint16_t delay_ms);

/**
 * @brief  修复版的 "tiaodong" 特效 (安全)
 * @param  delay_ms: 基础延迟速度 (毫秒)
 */
void ws_effect_tiaodong_fixed(uint16_t delay_ms);


#endif /* __WS2812_H */