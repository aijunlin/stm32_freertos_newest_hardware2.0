/*
 * ws2812.c
 * 描述: WS2812B (SPI+DMA) 驱动源文件 (FreeRTOS 兼容)
 */

#include "ws2812.h"
#include <stdio.h>    // 用于 printf
#include <string.h>   // 用于 memset (虽然我们不用了)
#include <math.h>     // 用于 sinf (呼吸灯)

// --- 缓冲区和颜色定义 (来自你的代码) ---
static u8 pixelBuffer[Pixel_S1_NUM][24];

const RGBColor_TypeDef YRED      = {0,255,0};
const RGBColor_TypeDef YGREEN    = {255,0,0};
const RGBColor_TypeDef YBLUE     = {0,0,255};
const RGBColor_TypeDef YSKY      = {255,0,255};
const RGBColor_TypeDef YMAGENTA  = {0,255,255};
const RGBColor_TypeDef YYELLOW   = {255,255,0};
const RGBColor_TypeDef YORANGE   = {106,127,0}; // G,R,B
const RGBColor_TypeDef YBLACK    = {0,0,0};
const RGBColor_TypeDef YWHITE    = {255,255,255};
const RGBColor_TypeDef YPURPLE   = {105,65,225}; // G,R,B
const RGBColor_TypeDef YPINK     = {80,255,120}; // G,R,B


// *************************************************************
// *** ***
// *** 驱动核心函数 (复用你的正确代码)          ***
// *** ***
// *************************************************************

/**
 * @brief  初始化 SPI1 和 DMA2_Stream3
 * (此函数来自你上传的 ws2812.c)
 */
void WS2812b_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);  
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; // PB5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1); // PB5 复用为 SPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // 84MHz/16 = 5.25MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);

	DMA_DeInit(DMA2_Stream3);
	while (DMA_GetCmdStatus(DMA2_Stream3) != DISABLE){}
		
	DMA_InitStructure.DMA_Channel = DMA_Channel_3;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI1->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)pixelBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = Pixel_S1_NUM * 24;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream3, &DMA_InitStructure);

	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA2_Stream3, DISABLE);
	while (DMA_GetCmdStatus(DMA2_Stream3) != DISABLE){}
	DMA_SetCurrDataCounter(DMA2_Stream3,Pixel_S1_NUM * 24);
	DMA_Cmd(DMA2_Stream3, ENABLE);

	// 启动时清空一次灯带
	rgb_ClearAll();
	rgb_SendArray();
    // 需要一个短暂的 RTOS 延迟来等待 DMA 完成和灯带复位
    // vTaskDelay(pdMS_TO_TICKS(1)); 
}

/**
 * @brief  设置单个灯珠的颜色 (写入缓冲区)
 * (此函数来自你上传的 ws2812.c，它是正确的)
 */
void rgb_SetColor(u16 LedId, RGBColor_TypeDef Color){
 	u16 i;
	
    // 关键的边界检查，防止越界
    if( LedId >= Pixel_S1_NUM ){ // 应该是 >= 
        printf("Error: LedId %d Out of Range! (Max is %d)\r\n", LedId, Pixel_S1_NUM - 1);
		return;
	}
  
    // GRB 顺序
    for(i=0;i<=7;i++){
		pixelBuffer[LedId][i]= ( (Color.G & (1 << (7 -i)) )? (CODE1):CODE0 );
	}
    for(i=8;i<=15;i++){
		pixelBuffer[LedId][i]= ( (Color.R & (1 << (15-i)) )? (CODE1):CODE0 );
	}
    for(i=16;i<=23;i++){
		pixelBuffer[LedId][i]= ( (Color.B & (1 << (23-i)) )? (CODE1):CODE0 );
	}
}

/**
 * @brief  通过 DMA 发送数据
 * (此函数来自你上传的 ws2812.c)
 */
void rgb_SendArray(void)
{
    // 等待上一次 DMA 传输完成
	if(DMA_GetFlagStatus(DMA2_Stream3,DMA_FLAG_TCIF3) != RESET)
    {
		DMA_ClearFlag(DMA2_Stream3,DMA_FLAG_TCIF3);
		
		DMA_Cmd(DMA2_Stream3, DISABLE);
		while (DMA_GetCmdStatus(DMA2_Stream3) != DISABLE){}
		
        // 重新设置 DMA 传输
		DMA_SetCurrDataCounter(DMA2_Stream3,Pixel_S1_NUM * 24);
		DMA_Cmd(DMA2_Stream3, ENABLE); 
	}
    // 注意：这里没有阻塞等待，DMA 会在后台运行。
    // 如果特效切换太快，可能需要加一个短暂的 vTaskDelay
}

// *************************************************************
// *** ***
// *** 辅助函数 (新)                         ***
// *** ***
// *************************************************************

/**
 * @brief  设置所有灯珠为同一颜色 (不发送)
 */
void rgb_SetAll(RGBColor_TypeDef Color)
{
    for (uint16_t i = 0; i < Pixel_S1_NUM; i++)
    {
        rgb_SetColor(i, Color);
    }
}

/**
 * @brief  关闭所有灯珠 (不发送)
 */
void rgb_ClearAll(void)
{
    rgb_SetAll(YBLACK);
}

/**
 * @brief  颜色环 (来自你的代码)
 */
RGBColor_TypeDef Colourful_Wheel(u8 WheelPos){
	RGBColor_TypeDef color;
    WheelPos = 255 - WheelPos;
  
    if(WheelPos < 85){
        color.R = 255 - WheelPos * 3;
        color.G = 0;
        color.B = WheelPos * 3;
    } else if(WheelPos < 170){
        WheelPos -= 85;
        color.R = 0;
        color.G = WheelPos * 3;
        color.B = 255 - WheelPos * 3;
    } else {
        WheelPos -= 170;
        color.R = WheelPos * 3; 
        color.G = 255 - WheelPos * 3;
        color.B = 0;
    }
    return color;  
}


// *************************************************************
// *** ***
// *** RTOS 安全的灯效函数 (新)               ***
// *** ***
// *************************************************************

/**
 * @brief  彩虹循环 (修复自你的代码)
 */
void ws_effect_rainbow_cycle(uint16_t cycles, uint16_t delay_ms) 
{
    uint16_t i, j;
    // 循环 256 * cycles 次，确保彩虹完整地滚动
    for(j = 0; j < 256 * cycles; j++){ 
        for(i = 0; i < Pixel_S1_NUM; i++)
        {  
            // 计算每个灯珠的颜色
            rgb_SetColor(i, Colourful_Wheel(((i * 256 / Pixel_S1_NUM) + j) & 255));
        } 
        rgb_SendArray();
        // !! 关键：使用 RTOS 延迟 !!
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

/**
 * @brief  颜色擦除 (流水灯)
 */
void ws_effect_color_wipe(RGBColor_TypeDef color, uint16_t delay_ms) 
{
    for (uint16_t i = 0; i < Pixel_S1_NUM; i++) 
    {
        rgb_SetColor(i, color);
        rgb_SendArray();
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

/**
 * @brief  剧院追逐 (跑马灯)
 */
void ws_effect_theater_chase(RGBColor_TypeDef color, uint16_t delay_ms) 
{
    // 循环10次
    for (int j = 0; j < 10; j++) { 
        // 3 帧一个循环 (亮-灭-灭)
        for (int q = 0; q < 3; q++) {
            rgb_ClearAll(); // 先清空
            
            // 每隔3个灯亮一个
            for (int i = q; i < Pixel_S1_NUM; i = i + 3) {
                rgb_SetColor(i, color);
            }
            rgb_SendArray();
            vTaskDelay(pdMS_TO_TICKS(delay_ms));
        }
    }
}

/**
 * @brief  扫描 (从中间向两边)
 */
void ws_effect_scan(RGBColor_TypeDef color, uint16_t delay_ms)
{
    int center = Pixel_S1_NUM / 2;
    // 从中间向两边
    for (int i = 0; i < center; i++) 
    {
        rgb_ClearAll();
        
        // 边界检查 (虽然在 center 内是安全的，但这是个好习惯)
        if (center + i < Pixel_S1_NUM)
            rgb_SetColor(center + i, color);
        if (center - i >= 0)
            rgb_SetColor(center - i, color);
            
        rgb_SendArray();
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

/**
 * @brief  呼吸灯
 */
void ws_effect_breathe(RGBColor_TypeDef color, uint16_t delay_ms)
{
    float brightness;
    // 循环 2*PI (一个完整的 sin 周期)
    for (float i = 0; i < 6.283; i += 0.05)
    {
        // sin(i) 范围是 -1 到 1。
        // (sin(i) + 1) / 2 范围是 0 到 1。
        brightness = (sinf(i) + 1.0) / 2.0;
        
        // 根据亮度计算新的颜色
        RGBColor_TypeDef newColor;
        newColor.G = (uint8_t)(color.G * brightness);
        newColor.R = (uint8_t)(color.R * brightness);
        newColor.B = (uint8_t)(color.B * brightness);
        
        rgb_SetAll(newColor);
        rgb_SendArray();
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}


/**
 * @brief  修复版的 "tiaodong" 特效 (安全)
 * (此特效逻辑基于你 ws2812.c 中的 tiaodong 函数)
 */
void ws_effect_tiaodong_fixed(uint16_t delay_ms)
{
    const RGBColor_TypeDef colors[] = {YPINK, YRED, YBLUE, YORANGE, YBLUE};
    const int sizes[] = {10, 20, 30, 15, 30}; 
    int center = Pixel_S1_NUM / 2; // 中心点 (例如 30)

    for (int j = 0; j < 5; j++) // 5 种颜色循环
    {
        rgb_ClearAll(); // 确保开始前是黑的
        
        int size = sizes[j];
        // 边界保护：确保 size 不会超过中心点到边缘的距离
        if (size > center) size = center; 

        // 1. 点亮
        for (int i = 0; i < size; i++) 
        {
            // 向右点亮 (e.g., 30 + i)
            // 边界检查：确保 (center + i) < 60
            if (center + i < Pixel_S1_NUM) {
                rgb_SetColor(center + i, colors[j]);
            }
            
            // 向左点亮 (e.g., 30 - i)
            // 边界检查：确保 (center - i) >= 0
            if (center - i >= 0) {
                rgb_SetColor(center - i, colors[j]);
            }
        }
        rgb_SendArray();
        // 保持显示一段时间 (原代码是 delay_ms * 5)
        vTaskDelay(pdMS_TO_TICKS(delay_ms * 5));

        // 2. 熄灭 (从中心开始向外)
        for (int x = 0; x < size; x++)
        {
            if (center + x < Pixel_S1_NUM) {
                rgb_SetColor(center + x, YBLACK);
            }
            if (center - x >= 0) {
                rgb_SetColor(center - x, YBLACK);
            }
            rgb_SendArray();
            vTaskDelay(pdMS_TO_TICKS(delay_ms));
        }
        
        // 确保完全熄灭
        rgb_ClearAll();
        rgb_SendArray();
        vTaskDelay(pdMS_TO_TICKS(50)); // 短暂间隔
    }
}