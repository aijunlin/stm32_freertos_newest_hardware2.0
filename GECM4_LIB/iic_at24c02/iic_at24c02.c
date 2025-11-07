#include "iic_at24c02.h"
#include "delay.h"

// ================================一、IIC协议=============================== //
/**
  * @brief  起始信号
  * @note   通知从机做好通信的准备
  * @param  None
  * @retval None
  */
void IIC_AT24C02_Start(void)
{
	// 1、设置SDA引脚为输出模式
	IIC_AT24C02_SelectMode(GPIO_Mode_OUT);
	
	// 2、设置SCL为高电平、SDA高电平、并延时4.7us以上
	IIC_AT24C02_SCL     = 1;
	IIC_AT24C02_SDA_OUT = 1;
	delay_us(5);
	
	// 3、设置SDA为低电平，并延时4us以上
	IIC_AT24C02_SDA_OUT = 0;
	delay_us(5);
	
	// 4、设置SCL为低电平
	IIC_AT24C02_SCL = 0;
	delay_us(5);
}

/**
  * @brief  停止信号
  * @note   通知从机通信已结束
  * @param  None
  * @retval None
  */
void IIC_AT24C02_Stop(void)
{
	// 1、设置SDA引脚为输出模式
	IIC_AT24C02_SelectMode(GPIO_Mode_OUT);
	
	// 2、设置SCL为高电平、SDA低电平、并延时4.7us以上
	IIC_AT24C02_SCL     = 1;
	IIC_AT24C02_SDA_OUT = 0;
	delay_us(5);
	
	// 3、设置SDA为高电平，并延时4.7us以上
	IIC_AT24C02_SDA_OUT = 1;
	delay_us(5);
	
	// 4、设置SDA为低电平
	IIC_AT24C02_SDA_OUT = 0;
	delay_us(5);
}


/**
  * @brief  发送一个字节(8位)数据
  * @note   主机-》从机
  * @param  byte：要发送的一个字节(8位)的数据
  * @retval None
  */
void IIC_AT24C02_SendByte(uint8_t byte)	// 0010 1100
{
	int8_t i = 0;
	
	// 1、设置SDA引脚为输出模式
	IIC_AT24C02_SelectMode(GPIO_Mode_OUT);
	
	// 2、设置SCL位低电平，SDA为低电平，并延时4us以上
	IIC_AT24C02_SCL     = 0;
	IIC_AT24C02_SDA_OUT = 0;
	delay_us(5);
	
	// 3、开始发送8位数据(MSB)
	for(i=7; i>=0; i--)
	{
		// a、设置SCL为低电平，并发送数据
		if(byte & (1<<i))				//  1010 1100 & 0000 0000
			IIC_AT24C02_SDA_OUT = 1;
		else
			IIC_AT24C02_SDA_OUT = 0;
			
		delay_us(2);					// 让其有时间将数据发送出去
		
		// b、设置SCL为高电平，并延时4us以上
		IIC_AT24C02_SCL     = 1;
		delay_us(5);
		
		// c、设置SCL为低电平，并延时4us以上
		IIC_AT24C02_SCL     = 0;
		delay_us(5);
		
	}
	
}

/**
  * @brief  接收一个字节(8位)数据
  * @note   从机-》主机
  * @param  None
  * @retval 返回接收到的数据
  */
uint8_t IIC_AT24C02_RecvByte(void)
{
	uint8_t data = 0;
	int8_t  i    = 0;
	
	// 1、设置SDA引脚为输入模式
	IIC_AT24C02_SelectMode(GPIO_Mode_IN);
	
	// 2、读取从模块发过来的数据(MSB)
	for(i=7; i>=0; i--)
	{
		// a、设置SCL引脚为高电平，并延时4us以上
		IIC_AT24C02_SCL     = 1;
		delay_us(5);
		
		// b、判断SDA引脚是否为高电平
		if(IIC_AT24C02_SDA_IN == 1)
			data = data|(1<<i);
		
		delay_us(2);					// 让其有时间将数据发送出去
		
		// c、设置SCL引脚为低电平，并延时4us以上
		IIC_AT24C02_SCL     = 0;
		delay_us(5);	
	}
	
	// 3、返回读取到的值
	return data;
	
}

/**
  * @brief  发送的应答信号
  * @note   主机-》从机
  * @param  ack：要发送的应答信号(应答：0， 不应答：1)
  * @retval None
  */
void IIC_AT24C02_SendAck(uint8_t ack)	
{
	// 1、设置SDA引脚为输出模式
	IIC_AT24C02_SelectMode(GPIO_Mode_OUT);
	
	// 2、设置SCL位低电平，发送电平信号，并延时4us以上
	IIC_AT24C02_SCL     = 0;
	IIC_AT24C02_SDA_OUT = ack;
	delay_us(2);					
	
	// 3、设置SCL为高电平
	IIC_AT24C02_SCL     = 1;
	delay_us(5);	
	
	// 4、设置SCL为低电平
	IIC_AT24C02_SCL     = 0;
	delay_us(5);
}


/**
  * @brief  接收的应答信号
  * @note   从机-》主机
  * @param  None
  * @retval 返回应答信号(应答：0， 不应答：1)
  */
uint8_t IIC_AT24C02_RecvAck(void)	
{
	uint8_t ack = 0;
	
	// 1、设置SDA引脚为输入模式
	IIC_AT24C02_SelectMode(GPIO_Mode_IN);
	
	// 2、设置SCL引脚为高电平，并延时4us以上
	IIC_AT24C02_SCL     = 1;
	delay_us(5);

	// 3、判断SDA引脚的电平值
	if(IIC_AT24C02_SDA_IN == 1)
		ack = 1;
	else
		ack = 0;
	
	delay_us(5);		// 让其有时间将数据接收

	
	// 4、设置SCL引脚为低电平
	IIC_AT24C02_SCL     = 0;
	delay_us(2);
	
	// 5、返回ack值
	return ack;
}



// ============================二、AT24C02模块通信=============================//
/**
  * @brief  初始化AT24C02模块
  * @note   U4-》IIC_SCL-》PB8(纯输出)	 
  *         U4-》IIC_SDA-》PB9(既有输出，又有输入)	 
  * @param  None  
  * @retval None
  */
void IIC_AT24C02_Init(void)
{
	// 0、GPIO片内外设信息初始化结构体
	GPIO_InitTypeDef  GPIO_InitStructure;

	// 1、使能GPIO片内外设的硬件时钟					
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	// 2、配置GPIO片内外设的引脚						
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_8|GPIO_Pin_9;	// 引脚：第8、9根引脚
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;			// 模式：输出模式
	GPIO_InitStructure.GPIO_Speed   = GPIO_High_Speed;			// 速度：高速(100MHz)
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_UP;				// 上下拉：上拉
	GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;			// 输出类型：推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);						// 使用该函数，将配置的信息写入到相应的寄存器中
}


/**
  * @brief  选择IIC_AT24C02模块引脚的输入输出模式
  * @note   None
  * @param  GPIO_Mode: GPIO_Mode_OUT(输出模式)， GPIO_Mode_IN(输入模式)
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t IIC_AT24C02_SelectMode(GPIOMode_TypeDef GPIO_Mode)
{
	// 0、GPIO片内外设信息初始化结构体
	GPIO_InitTypeDef  GPIO_InitStructure;

	// 1、选择模式并进行配置
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_9;				// 引脚：第9根引脚
	
	if(GPIO_Mode == GPIO_Mode_OUT)
	{
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;		// 模式：输出模式
		GPIO_InitStructure.GPIO_Speed   = GPIO_High_Speed;		// 速度：高速(100MHz)
		GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_UP;			// 上下拉：上拉
		GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;		// 输出类型：推挽输出
	}
	else if(GPIO_Mode == GPIO_Mode_IN)
	{
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;			// 模式：输入模式
		GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_UP;			// 上下拉：上拉
	}
	else
	{
		return -1;
	}
	GPIO_Init(GPIOB, &GPIO_InitStructure);						// 使用该函数，将配置的信息写入到相应的寄存器中
	
	return 0;
}



/**
  * @brief  写数据到at24c02模块上
  * @note   主机-》从机(仿照Linux的write函数的写法)
  * @param  addr：模块的工作地址
  *         pbuf：要写入的数据的内存
  *         len： 要写入的数据的长度
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t IIC_AT24C02_PageWrite(uint8_t addr, uint8_t *pbuf, uint8_t len)
{
	uint8_t ack = 0;
	
	// 1、发送开始信号
	IIC_AT24C02_Start();
	
	// 2、发送寻找设备地址信号(0xA0代表模块地址、并且是写操作：1010 0000)
	IIC_AT24C02_SendByte(0xA0);
	ack = IIC_AT24C02_RecvAck();	// 等待模块是否有应答(有应答：0， 无应答：1)
	if(ack == 1)
	{
		printf("Device Address(write) Error!\r\n");
		return -1;
	}
	
	// 3、发送工作地址信号
	IIC_AT24C02_SendByte(addr);
	ack = IIC_AT24C02_RecvAck();	
	if(ack == 1)
	{
		printf("Word Address(write) Error!\r\n");
		return -2;
	}
	
	
	// 4、向该地址写入数据
	while(len--)
	{
	
		// a、写入一个字节的数据
		IIC_AT24C02_SendByte(*pbuf);
		pbuf++;
		
		// b、判断发来的应答信号
		ack = IIC_AT24C02_RecvAck();	
		if(ack == 1)
		{
			printf("Date Error!\r\n");
			return -3;
		}
		
	}
	
	// 5、停止信号
	IIC_AT24C02_Stop();
	
	return 0;

}



/**
  * @brief  从at24c02模块上读取数据
  * @note   从机-》主机(仿照Linux的read函数的写法)
  * @param  addr：模块的工作地址
  *         pbuf：读取数据后存放的内存
  *         len： 要读取的数据的长度
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t IIC_AT24C02_SequentialRead(uint8_t addr, uint8_t *pbuf, uint8_t len)
{
	uint8_t ack = 0;
	
	if(len == 0)
		return -1;
	
	// 一、此处的写入，是为了让主机找到从机，建立联系
	// 1、发送开始信号
	IIC_AT24C02_Start();
	
	// 2、发送寻找设备地址信号(0xA0代表模块地址、并且是写操作：1010 0000)
	IIC_AT24C02_SendByte(0xA0);
	ack = IIC_AT24C02_RecvAck();	// 等待模块是否有应答(有应答：0， 无应答：1)
	if(ack == 1)
	{
		printf("Device Address(read) Error!\r\n");
		return -1;
	}
	
	// 3、发送工作地址信号
	IIC_AT24C02_SendByte(addr);
	ack = IIC_AT24C02_RecvAck();	
	if(ack == 1)
	{
		printf("Word Address(read) Error!\r\n");
		return -2;
	}
	
	
	// 二、建立联系后，就进行读取操作
	// 1、发送开始信号
	IIC_AT24C02_Start();
	
	// 2、发送寻找设备地址信号(0xA1代表模块地址、并且是读操作：1010 0001)
	IIC_AT24C02_SendByte(0xA1);
	ack = IIC_AT24C02_RecvAck();	
	if(ack == 1)
	{
		printf("Device Address(read) Error!\r\n");
		return -3;
	}
	
	// 3、从该地址读取数据
	len = len-1;
	while(len--)
	{
		// a、不断地接收来自模块的数据
		*pbuf = IIC_AT24C02_RecvByte();
		pbuf++;
		
		// b、作为主机，收到数据后需要发送应答信号给从机
		IIC_AT24C02_SendAck(0);

	}
	
	// 4、模块发送最后一个数据，主机发送非应答信号给从机
	*pbuf = IIC_AT24C02_RecvByte();
	IIC_AT24C02_SendAck(1);
	
	// 5、停止信号
	IIC_AT24C02_Stop();
	
	return 0;
}



















