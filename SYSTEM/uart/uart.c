#include "uart.h"
#include "delay.h"

volatile uint8_t  u1_recvbuf[RX1_BUF_SIZE];
volatile uint8_t  u1_count;
volatile int8_t   u1_flag;

volatile uint8_t  u2_recvbuf[RX2_BUF_SIZE];
volatile uint8_t  u2_count;
volatile int8_t   u2_flag;

volatile uint8_t  u3_recvbuf[RX3_BUF_SIZE];
volatile uint16_t u3_count;
volatile uint16_t u3_len;
volatile int8_t   u3_flag;


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

void _sys_exit(int return_code) 
{
	// label: goto label; /*
}


// =========================================一、UART片内外设初始化函数================================================//
/**
  * @brief  初始化USART1外设
  * @note   STM32F407ZET6芯片：             
  *         PA9(USART1_TX)、PA10(USART1_RX)
  * @param  None
  * @retval None
  */
void UART1_Init( uint32_t baud_rate)
{
	// 0、片内外设信息初始化结构体
	GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	// 1、使能USART1的硬件时钟，相应的GPIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);																
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//	2、将GPIO引脚PA9(TX1),PA10(RX1)连接到所需外设(USART1)的复用模式下
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	//	3、配置GPIO片内外设的引脚(PA9(TX1),PA10(RX1))
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_9|GPIO_Pin_10;	// 引脚：第9、10根引脚
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;				// 模式：复用模式
	GPIO_InitStructure.GPIO_Speed   = GPIO_High_Speed;			// 速度：高速(100MHz)
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;			// 上下拉：不拉
	GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;			// 输出类型：推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);						// 使用该函数，将配置的信息写入到相应的寄存器中
	
	//	4、USART1的外设信息配置
	USART_InitStructure.USART_BaudRate              = baud_rate;						// 波特率：有线;115200,无线：9600
	USART_InitStructure.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;	// 硬件流控制：没有
	USART_InitStructure.USART_Mode          		= USART_Mode_Tx|USART_Mode_Rx;		// 模式：收发模式
	USART_InitStructure.USART_Parity                = USART_Parity_No;					// 校验位：8位
	USART_InitStructure.USART_StopBits				= USART_StopBits_1;					// 停止位：1		
	USART_InitStructure.USART_WordLength			= USART_WordLength_8b;				// 数据位(字长)：8位
	USART_Init(USART1, &USART_InitStructure);											// 使用该函数，将配置的信息写入到相应的寄存器中
	
	// 5、给USART1外设在NVIC上进行配置
	NVIC_InitStructure.NVIC_IRQChannel                    = USART1_IRQn;				// 中断请求通道：USART1通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0;							// 抢占式优先级：0x00
	NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0;							// 响应式优先级：0x00
	NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;						// 使能NVIC相应的中断请求通道
	NVIC_Init(&NVIC_InitStructure);														// 使用该函数，将配置的信息写入到相应的寄存器中
	
	// 6、使能USART1中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	// 7、使能USART1外设
	USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  初始化USART2外设
  * @note   STM32F407ZET6芯片：             
  *         PA2(USART2_TX)、PA3(USART2_RX)
  * @param  None
  * @retval None
  */
void UART2_Init( uint32_t baud_rate)
{
	// 0、片内外设信息初始化结构体
	GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	// 1、使能USART2的硬件时钟，相应的GPIO时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);																
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//	2、将GPIO引脚PA2(TX2),PA3(RX2)连接到所需外设(USART2)的复用模式下
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	//	3、配置GPIO片内外设的引脚(PA2(TX2),PA3(RX2))
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2|GPIO_Pin_3;	// 引脚：第2、3根引脚
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;				// 模式：复用模式
	GPIO_InitStructure.GPIO_Speed   = GPIO_High_Speed;			// 速度：高速(100MHz)
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;			// 上下拉：不拉
	GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;			// 输出类型：推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);						// 使用该函数，将配置的信息写入到相应的寄存器中
	
	//	4、USART2的外设信息配置
	USART_InitStructure.USART_BaudRate              = baud_rate;						// 波特率：有线;115200,无线：9600
	USART_InitStructure.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;	// 硬件流控制：没有
	USART_InitStructure.USART_Mode          		= USART_Mode_Tx|USART_Mode_Rx;		// 模式：收发模式
	USART_InitStructure.USART_Parity                = USART_Parity_No;					// 校验位：8位
	USART_InitStructure.USART_StopBits				= USART_StopBits_1;					// 停止位：1		
	USART_InitStructure.USART_WordLength			= USART_WordLength_8b;				// 数据位(字长)：8位
	USART_Init(USART2, &USART_InitStructure);											// 使用该函数，将配置的信息写入到相应的寄存器中
	
	// 5、给USART2外设在NVIC上进行配置
	NVIC_InitStructure.NVIC_IRQChannel                    = USART2_IRQn;				// 中断请求通道：USART2通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0;							// 抢占式优先级：0x00
	NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0;							// 响应式优先级：0x00
	NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;						// 使能NVIC相应的中断请求通道
	NVIC_Init(&NVIC_InitStructure);														// 使用该函数，将配置的信息写入到相应的寄存器中
	
	// 6、使能USART2中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	// 7、使能USART2外设
	USART_Cmd(USART2, ENABLE);
}


/**
  * @brief  初始化USART3外设
  * @note   STM32F407ZET6芯片：             
  *         PB10(USART3_TX)、PB11(USART3_RX)
  * @param  None
  * @retval None
  */
void UART3_Init( uint32_t baud_rate)
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



// =========================================二、UART片内外设发送数据函数================================================//
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


// =========================================三、UART片内外设接收数据函数================================================//
/**
  * @brief  UART1的接收数据中断服务函数(上位机-》下位机)
  * @note   接收来自上位机(电脑的串口助手、手机蓝牙APP、手机wifi的app)的信息，并进行处理
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)		// "qinggenihao#"
{	
	uint8_t r_data = 0;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET )			// 判断是否有数据发送到uart1中了(串口接收中断)
	{
		r_data = USART_ReceiveData(USART1);							// 接收来自电脑(手机)的信息(上位机-》下位机)
		u1_recvbuf[u1_count] = r_data;								// 将接收到的数据，一个个存到数组u1_recvbuf中

		if(r_data == '#')
			u1_flag = 1;											// 接收串口数据结束标志
		else
			u1_count++;
		
		
		USART_SendData(USART1, r_data);								// 向电脑(手机)回发数据(下位机-》上位机)，让上位机知道自己发了什么数据
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == 0);	// 等待字符发送结束
	
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);				// 清除串口接收中断标志
	}		
}


/**
  * @brief  UART2的接收数据中断服务函数(上位机-》下位机)
  * @note   接收来自上位机(电脑的串口助手、手机蓝牙APP、手机wifi的app)的信息，并进行处理
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)		
{	
	uint8_t r_data = 0;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET )			
	{
		r_data = USART_ReceiveData(USART2);							
		u2_recvbuf[u2_count] = r_data;								

		if(r_data == '#')
			u2_flag = 1;											
		else
			u2_count++;
		
#if   UART2_DEBUG													// 将uart2返回的信息发送到uart1(一般是串口A-A口线上)
		USART_SendData(USART1, r_data);								
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == 0);	
#elif UART2_RUN														// 会正常的将数据回发到uart2上
		USART_SendData(USART2, r_data);								
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == 0);	
#endif		
	
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);				
	}		
}


/**
  * @brief  UART3的中断服务函数(电脑(手机)->单片机)
  * @note   接收来自上位机(电脑的串口助手、手机蓝牙app、wifi的app)的信息，并进行处理
  * @param  None 
  * @retval None
  */
void USART3_IRQHandler(void)								
{
	uint8_t r_data = 0;
	
	if( USART_GetITStatus(USART3, USART_IT_RXNE) == SET )		// 判断是否有数据发送到uart3中了	
	{
		
		r_data =  USART_ReceiveData(USART3);	
		if(u3_count < RX3_BUF_SIZE)								// 只要接收缓冲区可以存放，就一直存放下去，直到没有数据为止
		{
			u3_recvbuf[u3_count] = r_data;	
			u3_count++;
		}
		
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);		
	}
	
	if(USART_GetITStatus(USART3, USART_IT_IDLE) == SET )		// 判断uart3是否为空闲状态
	{
		uint32_t temp;
		
		// 设置接收完毕标志
		u3_flag  = 1;
		u3_len   = u3_count;
		
#if ESP8266_DEBUG		
		USART1_SendStr((const char*)u3_recvbuf);
#endif	
		
		// 重置索引，准备接收下一帧数据
		u3_count = 0;
		
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



// =========================================四、UART片内外设使用的模块的信息配置和获取================================================//


/**
  * @brief  获取蓝牙模块的信息
  * @note   None
  * @param  None
  * @retval None
  */
void USART_BlueTooth_GetMsg(void)	
{
	USART1_SendStr("获取蓝牙模块信息中\r\n");	// 输出到UART1中(通常是A-A口线)
	
	// 1、发送测试指令
	USART2_SendStr("AT\r\n");
	delay_ms(1000);
	
	// 2、获取蓝牙模块的名字
	USART2_SendStr("AT+NAME?\r\n");
	delay_ms(1000);
	
	// 3、获取蓝牙模块的地址
	USART2_SendStr("AT+ADDR?\r\n");
	delay_ms(1000);
	
	// 4、获取蓝牙模块的配对码
	USART2_SendStr("AT+PSWD?\r\n");
	delay_ms(1000);
	
	// 5、获取蓝牙模块的波特率
	USART2_SendStr("AT+UART?\r\n");
	delay_ms(1000);
	
	USART1_SendStr("获取蓝牙模块信息完毕\r\n");	
	
}

/**
  * @brief  设置蓝牙模块的信息
  * @note   None
  * @param  name：要设置的蓝牙的名字
  *			pswd：要设置的蓝牙的配对码
  *			uart：要设置的蓝牙的波特率, 停止位，校验位(例子："9600,0,0")
  * @retval None
  */
void USART_BlueTooth_SetMsg(const char* name, const char* pswd, const char* uart)
{
	
	char buf[128] = {0};
	
	USART1_SendStr("设置蓝牙模块信息中\r\n");	// 输出到UART1中(通常是A-A口线)

	// 1、发送测试指令
	USART2_SendStr("AT\r\n");
	delay_ms(1000);
	
	// 2、设置蓝牙模块的名字
	MY_LIB_ClearArray((uint8_t *)buf, 128);
	sprintf(buf, "AT+NAME=%s\r\n", name);
	USART2_SendStr(buf);
	delay_ms(1000);

	
	// 3、设置蓝牙模块的配对码
	MY_LIB_ClearArray((uint8_t *)buf, 128);
	sprintf(buf, "AT+PSWD=%s\r\n", pswd);
	USART2_SendStr(buf);
	delay_ms(1000);
	
	// 4、设置蓝牙模块的波特率
	MY_LIB_ClearArray((uint8_t *)buf, 128);
	sprintf(buf, "AT+UART=%s\r\n", uart);
	USART2_SendStr(buf);
	delay_ms(1000);
	
	USART1_SendStr("设置蓝牙模块信息完毕\r\n");	
}













