/**
 ******************************************************************************
 * @file    uart.c
 * @author  ajl
 * @version V0.0.3
 * @date    30-Octorber-2025
 * @brief   0.0.1初始化了usart1来传输串口的信息
 *			0.0.2初始化了usart2来传输蓝牙的信息
 *			0.0.3初始化了usart3来传输wifi的数据
 ******************************************************************************
 * @attention
 *	//6AA60695EB62 蓝牙模块的mac地址
 * 
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "uart.h" // Device header

uint16_t uart3_count = 0;
int8_t uart3_flag = 0;
uint16_t uart3_len = 0;

uint16_t uart1_count = 0;
int8_t uart1_flag = 0;
uint16_t uart1_len = 0;


#define BEEP_OFF_LEN (sizeof(beep_off) / sizeof(uint16_t)) // 7
#define BEEP_ON_LEN  (sizeof(beep_on) / sizeof(uint16_t))  // 8


// 重定向printf函数的数据到串口1中
#pragma import(__use_no_semihosting_swi)	// 关闭半主机模式(使得程序遇到这个文件操作函数(printf、scanf)时，不停留在此断点处)

struct __FILE 
{ 
	int handle; /* Add whatever you need here */ 
};
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) 		// 重写fputc函数，以支持printf函数，使得数据定向到串口1上
{
	// 发送单个数据
	USART_SendData(USART1, ch);	
	
	// 检查串口1是否还有未发送的数据，有的话，一直死循环等待
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == 0);
		
    return ch;
}

/**
 * @brief  uart初始化函数
 * @param  None
 * @note   
 * @retval None
 */

uint16_t data[DATA_SIZE] = {0};//存放接收到的数据ascii码
uint16_t data_temp[DATA_SIZE] = {0};
uint8_t  uart3_recvbuf[RX3_BUF_SIZE];

uint8_t  uart1_recvbuf[RX3_BUF_SIZE];

//beep_on
//42 65 65 70 5f 6f 6e
uint16_t beep_on[] = {0x62, 0x65, 0x65, 0x70, 0x5f, 0x6f, 0x6e};
//在这里写命令
//beep_off
uint16_t beep_off[] = {0x62, 0x65, 0x65, 0x70, 0x5f, 0x6f, 0x66, 0x66};


void Uart_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	

	//端口复用设置
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10,GPIO_AF_USART1);
	
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=115200;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//无流控
	USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;//收发模式
	USART_InitStructure.USART_Parity=USART_Parity_No;//校验位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;//1位停止位
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;//8位数据位
	
	USART_Init(USART1,&USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//usart的接受中断使能
	
	USART_Cmd(USART1, ENABLE);//usart2使能
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;//配置为usart的中断
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	
	NVIC_Init(&NVIC_InitStruct);
	
}

/**
 * @brief  uart2初始化函数
 * @param  None
 * @note   PA2  r   PA3  t
 * @retval None
 */


void Uart2_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	

	//端口复用设置
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=9600;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//无流控
	USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;//收发模式
	USART_InitStructure.USART_Parity=USART_Parity_No;//校验位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;//1位停止位
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;//8位数据位
	
	USART_Init(USART2,&USART_InitStructure);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//usart的接受中断使能
	
	USART_Cmd(USART2, ENABLE);//usart2使能
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;//配置为usart的中断
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	
	NVIC_Init(&NVIC_InitStruct);
	
}

/**
  * @brief  初始化USART3外设
  * @note   STM32F407ZET6芯片：             
  *         PB10(USART3_TX)、PB11(USART3_RX)
  * @param  None
  * @retval None
  */
void Uart3_Init(uint32_t baud_rate)
{
	// 0、片内外设信息初始化结构体
	GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	// 1、使能USART3的硬件时钟，相应的GPIO时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);																
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//	2、将GPIO引脚PB10(TX3),PB11(RX3)连接到所需外设(USART3)的复用模式下
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	//	3、配置GPIO片内外设的引脚(PB10(TX3),PB11(RX3))
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_10|GPIO_Pin_11;	// 引脚：第10、11根引脚
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;				// 模式：复用模式
	GPIO_InitStructure.GPIO_Speed   = GPIO_High_Speed;			// 速度：高速(100MHz)
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;			// 上下拉：不拉
	GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;			// 输出类型：推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);						// 使用该函数，将配置的信息写入到相应的寄存器中
	
	//	4、USART3的外设信息配置
	USART_InitStructure.USART_BaudRate              = baud_rate;						// 波特率：有线;115200,无线：9600
	USART_InitStructure.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;	// 硬件流控制：没有
	USART_InitStructure.USART_Mode          		= USART_Mode_Tx|USART_Mode_Rx;		// 模式：收发模式
	USART_InitStructure.USART_Parity                = USART_Parity_No;					// 校验位：8位
	USART_InitStructure.USART_StopBits				= USART_StopBits_1;					// 停止位：1		
	USART_InitStructure.USART_WordLength			= USART_WordLength_8b;				// 数据位(字长)：8位
	USART_Init(USART3, &USART_InitStructure);											// 使用该函数，将配置的信息写入到相应的寄存器中
	
	// 5、给USART3外设在NVIC上进行配置
	NVIC_InitStructure.NVIC_IRQChannel                    = USART3_IRQn;				// 中断请求通道：USART3通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0;							// 抢占式优先级：0x00
	NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0;							// 响应式优先级：0x00
	NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;						// 使能NVIC相应的中断请求通道
	NVIC_Init(&NVIC_InitStructure);														// 使用该函数，将配置的信息写入到相应的寄存器中
	
	// 6、使能USART3中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	
	// 7、使能USART3外设
	USART_Cmd(USART3, ENABLE);
}


/**
 * @brief  uart1中断函数
 * @param  None
 * @note   该中断函数主要用来接收usart发送的数据
 * 			注意使用串口助手发送数据的时候发送新行，保证数据的正确截断
 * @retval None
 */

// void USART1_IRQHandler(void) 
// {
// 	static uint8_t count = 0;
//     if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) 
// 	{    //如果收到了信息

// 		data_temp[count] = USART_ReceiveData(USART1);//将接收到的数据放入缓冲区
					
// 		USART_SendData(USART1,data_temp[count]);//将接受到的数据发送回去

// 		if (data_temp[count] == 0x0A)
// 		{
// 			data_temp[count] = 0;
// 			Uart_Data_Cpy(data,data_temp);
// 			Uart_Data_Cmp();
// 			Uart_Data_clear();
// 			count = 0;
// 			USART_ClearITPendingBit(USART1, USART_IT_RXNE);  // 清除中断标志		
// 			return;
// 		}
//         count++;
		
//         USART_ClearITPendingBit(USART1, USART_IT_RXNE);  // 清除中断标志
//     }
// }

void USART1_IRQHandler(void)		// "qinggenihao#"
{	
	uint8_t r_data = 0;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET )			// 判断是否有数据发送到uart1中了(串口接收中断)
	{
		r_data = USART_ReceiveData(USART1);						// 接收来自电脑(手机)的信息(上位机-》下位机)
		uart1_recvbuf[uart1_count] = r_data;								// 将接收到的数据，一个个存到数组u1_recvbuf中

		if(r_data == '#')
			uart1_flag = 1;											// 接收串口数据结束标志
		else
			uart1_count++;
		
		
		USART_SendData(USART1, r_data);	
		USART_SendData(USART3, r_data);	
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == 0);	// 等待字符发送结束
	
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);				// 清除串口接收中断标志
	}		
}


/**
 * @brief  uar2中断函数
 * @param  None
 * @note   该中断函数主要用来接收usart发送的数据
 * 			注意使用串口助手发送数据的时候发送新行，保证数据的正确截断
 * @retval None
 */

void USART2_IRQHandler(void) 
{
	static uint8_t count = 0;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET) 
	{    //如果收到了信息

		data_temp[count] = USART_ReceiveData(USART2);//将接收到的数据放入缓冲区
					
		USART_SendData(USART2,data_temp[count]);//将接受到的数据发送回去

		if (data_temp[count] == '#')	//设置关闭位
		{
			data_temp[count] = 0;
			Uart_Data_Cpy(data,data_temp);
			Uart_Data_Cmp();
			Uart_Data_clear();
			count = 0;
			USART_ClearITPendingBit(USART2, USART_IT_RXNE);  // 清除中断标志		
			return;
		}
        count++;
		
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);  // 清除中断标志
    }
}


/**
 * @brief  uart3中断函数
 * @param  None
 * @note   该中断函数主要用来接收usart发送的数据
 * 			注意使用串口助手发送数据的时候发送新行，保证数据的正确截断
 * @retval None
 */

void USART3_IRQHandler(void)								
{
	uint8_t r_data = 0;


	if( USART_GetITStatus(USART3, USART_IT_RXNE) == SET )		// 判断是否有数据发送到uart3中了	
	{
		
		r_data =  USART_ReceiveData(USART3);	
		if(uart3_count < RX3_BUF_SIZE)								// 只要接收缓冲区可以存放，就一直存放下去，直到没有数据为止
		{
			uart3_recvbuf[uart3_count] = r_data;	
			uart3_count++;
		}
		
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);		
	}
	
	if(USART_GetITStatus(USART3, USART_IT_IDLE) == SET )		// 判断uart3是否为空闲状态
	{
		uint32_t temp;

		// 设置接收完毕标志
		uart3_flag  = 1;
		uart3_len   = uart3_count;
		
#if ESP8266_DEBUG		
		USART1_SendStr((const char*)uart3_recvbuf);
#endif	
		
		// 重置索引，准备接收下一帧数据
		uart3_count = 0;
		
		/*
			清除空闲中断标志的特殊方法：
			必须先读SR寄存器，再读DR寄存器
			这是STM32的特殊要求
		*/
		temp = USART3->SR;
		temp = USART3->DR;
		(void)temp;			// 避免编译警告
		
	}		
}


/**
 * @brief  uart数据拷贝函数
 * @param  data:存储用来处理的数据
 * @param  data_temp:存储用来缓冲的数据
 * @note   该函数主要用来拷贝uart接收到缓冲区的数据拷贝到保存区
 * 		   函数的主要目的确保数据的正确缓冲方便后期的对比显示
 * @retval None
 */

void Uart_Data_Cpy(uint16_t *data , uint16_t *data_temp)
{
	for (uint16_t i = 0; i < DATA_SIZE; i++)
	{
		data[i] = data_temp[i];
	}
	
}
/**
 * @brief  uart数据拷贝函数
 * @param  none
 * @note   该函数主要用来拷贝uart接收到缓冲区的数据拷贝到保存区
 * 		   函数的主要目的确保数据的正确缓冲方便后期的对比显示
 * @retval None
 */

void Uart_Data_Cmp(void)
{
	
	if (memcmp(data, beep_on, sizeof(beep_on)) == 0)
	{
		Beep_Flag = UART_BEEP_ON;
	}
	if (memcmp(data, beep_off, sizeof(beep_off)) == 0)
	{
		Beep_Flag = UART_BEEP_OFF;
	}
	
}
/**
* @brief  uart数据缓冲区清理函数
 * @param  none
 * @note   该函数主要用来拷贝uart接收到缓冲区的数据拷贝到保存区
 * 		   函数的主要目的确保数据的正确缓冲方便后期的对比显示
 * @retval None
 */

void Uart_Data_clear(void)
{

	for (uint16_t i = 0; i < DATA_SIZE; i++)
	{
		// data[i] = 0;
		data_temp[i] = 0;
	}
}


/**
* @brief  uart数据缓冲区清理函数
 * @param  none
 * @note   该函数主要用来拷贝uart接收到缓冲区的数据拷贝到保存区
 * 		   函数的主要目的确保数据的正确缓冲方便后期的对比显示
 * @retval None
 */

void Uart_DataArray_clear(uint8_t *data,uint16_t len)
{

	for (uint16_t i = 0; i < len; i++)
	{
		data[i] = 0;
	}
}



/**
  * @brief  UART1的发送数据(下位机-》上位机)
  * @note   发送的是字符串数据
  * @param  None
  * @retval None
  */
void USART1_SendStr(const char *str)	
{	
	while(*str != '\0')
	{
		USART_SendData(USART1, *str++);								// 发送一个字符
		
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == 0);	// 等待字符发送结束
	}
}

/**
  * @brief  UART2的发送数据(下位机-》上位机)
  * @note   发送的是字符串数据
  * @param  None
  * @retval None
  */
void USART2_SendStr(const char *str)	
{	
	while(*str != '\0')
	{
		USART_SendData(USART2, *str++);								// 发送一个字符
		
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == 0);	// 等待字符发送结束
	}
}

/**
  * @brief  UART3的发送数据(下位机-》上位机)
  * @note   发送的是字符串数据
  * @param  None
  * @retval None
  */
void USART3_SendStr(const char *str)	
{	
	while(*str != '\0')
	{
		USART_SendData(USART3, *str++);								// 发送一个字符
		
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == 0);	// 等待字符发送结束
	}
}


/**
  * @brief  比较两个数组前面的n个字符是否一致
  * @note   None
  * @param  p1_buf：要比较的数组1
  *         p2_buf：要比较的数组2
  *         len：   要比较的长度(前面len个字符)
  * @retval 返回0：  代表两个数组前面的len个数据，是一致的
  *         返回非0：代表两个数组前面的len个数据，是不一致的
  */
int8_t MY_LIB_CmpArray(int8_t *p1_buf, int8_t *p2_buf, uint16_t len)
{
	int16_t i = 0;
	
	for(i=0; i<len; i++)
	{
		if(p1_buf[i] != p2_buf[i])
			return -1;
	}
	return 0;
}

