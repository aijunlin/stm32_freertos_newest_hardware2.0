#include "spi_w25q128.h"
#include "delay.h"


// =======================一、SPI协议================================//
/**
  * @brief  模拟SPI协议的模式0通信
  * @note   None
  * @param  byte：你要发送的8位(1个字节)的数据
  * @retval 返回读取到8位(1个字节)的数据
  */
uint8_t SPI_W25Q28_MODE0_SendRecvByte(uint8_t byte)	
{
	uint8_t data = 0;
	int8_t  i    = 0;
		
	for(i=7; i>=0; i--)		// SPI通常是MSB传输的
	{
		// 1、主机->从机(写)
		// 写入
		if(byte & (1<<i))			// 0101 1100 & 1000 0000 == 0
			SPI_W25Q128_MOSI = 1;
		else
			SPI_W25Q128_MOSI = 0;
		
		// 设置时钟引脚为高电平(让其处在工作中)
		SPI_W25Q128_SCLK = 1;
		delay_us(2);				// 让其有时间操作
		
		// 2、从机->主机(读)
		if(SPI_W25Q128_MISO == 1)
			data = data|(1<<i);
		
		// 设置时钟引脚为低电平(让其有电平跳变)
		SPI_W25Q128_SCLK = 0;
		delay_us(2);					
	}
	return data;	
}


/**
  * @brief  模拟SPI协议的模式3通信
  * @note   None
  * @param  byte：你要发送的8位(1个字节)的数据
  * @retval 返回读取到8位(1个字节)的数据
  */
uint8_t SPI_W25Q28_MODE3_SendRecvByte(uint8_t byte)	
{
	uint8_t data = 0;
	int8_t  i    = 0;
		
	for(i=7; i>=0; i--)		// SPI通常是MSB传输的
	{
		// 1、主机->从机(写)
		// 写入
		if(byte & (1<<i))			// 0101 1100 & 1000 0000 == 0
			SPI_W25Q128_MOSI = 1;
		else
			SPI_W25Q128_MOSI = 0;
		
		// 设置时钟引脚为低电平(让其处在工作中)
		SPI_W25Q128_SCLK = 0;
		delay_us(2);				// 让其有时间操作
		
		// 2、从机->主机(读)
		if(SPI_W25Q128_MISO == 1)
			data = data|(1<<i);
		
		// 设置时钟引脚为高电平(让其有电平跳变)
		SPI_W25Q128_SCLK = 1;
		delay_us(2);					
	}
	return data;	
}

// =======================二、W25Q128模块通信================================//

/**
  * @brief  初始化W28Q128模块
  * @note   U3-》F_CS-》     PB14(输出模式)
  *         U3-》SPI1_MISO-》PB4(输入模式)
  *         U3-》SPI1_MOSI-》PB5(输出模式)
  *         U3-》SPI1_SCK -》PB3(输出模式)
  *
  * @param  None
  * @retval None
  */
void SPI_W25Q28_Init(void)	
{
	// 0、GPIO片内外设信息初始化结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 1、使能GPIO片内外设的硬件时钟					
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	// 2、配置GPIO片内外设的引脚						
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_14;	// 引脚：第3、5、14根引脚
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;						// 模式：输出模式
	GPIO_InitStructure.GPIO_Speed   = GPIO_High_Speed;						// 速度：高速(100MHz)
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;						// 上下拉：不拉
	GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;						// 输出类型：推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);									// 使用该函数，将配置的信息写入到相应的寄存器中

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_4;							// 引脚：第4根引脚
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;							// 模式：输出模式
	GPIO_Init(GPIOB, &GPIO_InitStructure);									// 使用该函数，将配置的信息写入到相应的寄存器中
	
	// 3、设置一开始的引脚的状态
	SPI_W25Q128_CS = 1;
}




/**
  * @brief  读取厂商和设备的ID
  * @note   None
  * @param  None
  * @retval 返回读取到的厂商和设备的ID值
  */
uint16_t SPI_W25Q28_ReadID(void)	
{
	uint16_t man_id = 0;
	uint16_t dev_id = 0;
	uint16_t id     = 0;
	
	// 1、拉低片选引脚(使得模块开始工作)
	SPI_W25Q128_CS = 0;
	
	// 2、单片机-》W25Q128模块
	// a、发送引导码(90h)
	SPI_W25Q28_MODE0_SendRecvByte(0x90);
	
	// b、发送地址码(000000h)
	SPI_W25Q28_MODE0_SendRecvByte(0x00);
	SPI_W25Q28_MODE0_SendRecvByte(0x00);
	SPI_W25Q28_MODE0_SendRecvByte(0x00);
	
	// 3、W25Q128模块-》单片机
	// a、读取厂商ID值(可以任意的发送数据(M->S)(没用)，目的只是为了获取数据(S-M)(有用))
	man_id = SPI_W25Q28_MODE0_SendRecvByte(0xFF);
	
	// b、读取设备ID值(可以任意的发送数据(M->S)(没用)，目的只是为了获取数据(S-M)(有用))
	dev_id = SPI_W25Q28_MODE0_SendRecvByte(0xFF);
	
	// 4、组合两个ID值
	id = ((man_id<<8)|dev_id);
	
	// 5、拉高片选引脚，使得模块处在休闲状态下
	SPI_W25Q128_CS = 1;
	
	return id;

}



/**
  * @brief  读取指定W25Q128模块的FLASH地址的数据
  * @note   仿照Linux上read函数的写法
  * @param  addr：要读取的W25Q128模块FlASH内存的24位的地址
  *         pbuf：读取数据后存放的缓冲区
  *         len： 要读取的数据的长度
  * @retval 成功：返回0
  */
int8_t SPI_W25Q28_ReadData(uint32_t addr, uint8_t* pbuf, uint16_t len)	
{
	// 1、拉低片选引脚(使得模块开始工作)
	SPI_W25Q128_CS = 0;
	
	
	// 2、单片机-》W25Q128模块
	// a、发送引导码(03h)
	SPI_W25Q28_MODE0_SendRecvByte(0x03);
	
	// b、发送地址码
	SPI_W25Q28_MODE0_SendRecvByte((addr>>16)&0xff);		// 0x00123456>>16 ==> 0x00000012 ==>0x00000012 & 0x000000ff == 0x00000012(高8位)
	SPI_W25Q28_MODE0_SendRecvByte((addr>>8) &0xff);     // 0x00123456>>8  ==> 0x00001234 ==>0x00001234 & 0x000000ff == 0x00000034(中8位)
	SPI_W25Q28_MODE0_SendRecvByte((addr>>0) &0xff);		// 0x00123456>>0  ==> 0x00123456 ==>0x00123456 & 0x000000ff == 0x00000056(低8位)
	
	// 3、W25Q128模块-》单片机
	while(len--)
	{
		// 可以任意的发送数据(M->S)(没用)，目的只是为了获取数据(S-M)(有用)
		*pbuf = SPI_W25Q28_MODE0_SendRecvByte(0xFF);
		pbuf++;
		delay_us(20);
		
	}
	
	// 4、拉高片选引脚，使得模块处在休闲状态下
	SPI_W25Q128_CS = 1;
	
	return 0;
	
}



/**
  * @brief  读取状态寄存器
  * @note   仿照Linux上read函数的写法
  * @param  statu_addrr：状态急寄存器1：05h， 状态急寄存器2：35h
  * @retval 成功：返回0
  */
int8_t SPI_W25Q28_ReadStatus(uint8_t statu_addr)	
{
	uint8_t statu = 0;
	
	// 1、拉低片选引脚(使得模块开始工作)
	SPI_W25Q128_CS = 0;
	
	// 2、单片机-》W25Q128模块
	// a、发送引导码(05h或35h)
	SPI_W25Q28_MODE0_SendRecvByte(statu_addr);
	
	// 3、W25Q128模块-》单片机
	// 可以任意的发送数据(M->S)(没用)，目的只是为了获取数据(S-M)(有用)
	statu = SPI_W25Q28_MODE0_SendRecvByte(0xFF);
	
	// 4、拉高片选引脚，使得模块处在休闲状态下
	SPI_W25Q128_CS = 1;
	
	return statu;
	
}



/**
  * @brief  写使能
  * @note   None
  * @param  None
  * @retval None
  */
void SPI_W25Q28_WriteEnable(void)	
{
	// 1、拉低片选引脚(使得模块开始工作)
	SPI_W25Q128_CS = 0;
	
	// 2、单片机-》W25Q128模块
	// a、发送引导码(06h)
	SPI_W25Q28_MODE0_SendRecvByte(0x06);
		
	// 3、拉高片选引脚，使得模块处在休闲状态下
	SPI_W25Q128_CS = 1;
}


/**
  * @brief  写失能
  * @note   None
  * @param  None
  * @retval None
  */
void SPI_W25Q28_WriteDisable(void)	
{
	// 1、拉低片选引脚(使得模块开始工作)
	SPI_W25Q128_CS = 0;
	
	// 2、单片机-》W25Q128模块
	// a、发送引导码(04h)
	SPI_W25Q28_MODE0_SendRecvByte(0x04);
		
	// 3、拉高片选引脚，使得模块处在休闲状态下
	SPI_W25Q128_CS = 1;
}



/**
  * @brief  擦除指定的扇区
  * @note   None
  * @param  addr：指定要擦除的W25Q128模块的flash内存的扇区
  * @retval None
  */
void SPI_W25Q28_SectorErase(uint32_t addr)	
{
	uint8_t status = 0;
	
	// 1、在擦除扇区之前，需要写使能
	SPI_W25Q28_WriteEnable();
	
	delay_us(10);	// 让其有时间执行功能
	
	
	// 2、拉低片选引脚(使得模块开始工作)
	SPI_W25Q128_CS = 0;
	
	// 3、单片机-》W25Q128模块
	// a、发送引导码(20h)
	SPI_W25Q28_MODE0_SendRecvByte(0x20);
	
	// b、发送地址码
	SPI_W25Q28_MODE0_SendRecvByte((addr>>16)&0xff);		// 0x00123456>>16 ==> 0x00000012 ==>0x00000012 & 0x000000ff == 0x00000012(高8位)
	SPI_W25Q28_MODE0_SendRecvByte((addr>>8) &0xff);     // 0x00123456>>8  ==> 0x00001234 ==>0x00001234 & 0x000000ff == 0x00000034(中8位)
	SPI_W25Q28_MODE0_SendRecvByte((addr>>0) &0xff);		// 0x00123456>>0  ==> 0x00123456 ==>0x00123456 & 0x000000ff == 0x00000056(低8位)
	
	// 4、拉高片选引脚，使得模块处在休闲状态下
	SPI_W25Q128_CS = 1;
	delay_us(10);	// 让其有时间执行功能
	
	// 5、检查一下当前是否擦除完毕(检查状态寄存器)
	while(1)
	{
		status = SPI_W25Q28_ReadStatus(0x05);			// 读取状态寄存器1
		if((status & 0x01) == 0)						// status: xxxx xxxx & 0000 0001 == 0000 000x
			break;
		else
			delay_us(1);
	}
	
	// 6、在擦除扇区之后，需要写失能
	SPI_W25Q28_WriteDisable();
}


/**
  * @brief  向指定的W25Q128模块的FLASH内存的地址写入数据
  * @note   仿照Linux上write函数的写法，一个页是256个字节。
  *         注意：写入数据之前，需要擦除扇区
  * @param  addr：要写入的W25Q128模块FlASH内存的24位的地址
  *         pbuf：要写入数据的存储的缓冲区
  *         len： 要写入的数据的长度(不要超过256)
  * @retval None
  */
void SPI_W25Q28_WritePageData(uint32_t addr, uint8_t* pbuf, uint16_t len)	
{
	uint8_t status = 0;
	
	// 1、在擦除扇区之前，需要写使能
	SPI_W25Q28_WriteEnable();
	
	delay_us(10);	// 让其有时间执行功能
	
	
	// 2、拉低片选引脚(使得模块开始工作)
	SPI_W25Q128_CS = 0;
	
	// 3、单片机-》W25Q128模块
	// a、发送引导码(02h)
	SPI_W25Q28_MODE0_SendRecvByte(0x02);
	
	// b、发送地址码
	SPI_W25Q28_MODE0_SendRecvByte((addr>>16)&0xff);		// 0x00123456>>16 ==> 0x00000012 ==>0x00000012 & 0x000000ff == 0x00000012(高8位)
	SPI_W25Q28_MODE0_SendRecvByte((addr>>8) &0xff);     // 0x00123456>>8  ==> 0x00001234 ==>0x00001234 & 0x000000ff == 0x00000034(中8位)
	SPI_W25Q28_MODE0_SendRecvByte((addr>>0) &0xff);		// 0x00123456>>0  ==> 0x00123456 ==>0x00123456 & 0x000000ff == 0x00000056(低8位)

	
	// 4、将要发送的数据一个个发送出去(限定256个字节的数据)
	while(len--)
	{
		SPI_W25Q28_MODE0_SendRecvByte(*pbuf);
		pbuf++;
		delay_us(2);
	}
	

	// 5、拉高片选引脚，使得模块处在休闲状态下
	SPI_W25Q128_CS = 1;
	delay_us(10);	// 让其有时间执行功能
	
	// 6、检查一下当前是否擦除完毕(检查状态寄存器)
	while(1)
	{
		status = SPI_W25Q28_ReadStatus(0x05);			// 读取状态寄存器1
		if((status & 0x01) == 0)						// status: xxxx xxxx & 0000 0001 == 0000 000x
			break;
		else
			delay_us(1);
	}
	
	// 7、在擦除扇区之后，需要写失能
	SPI_W25Q28_WriteDisable();
	
	delay_us(10);	// 让其有时间执行功能
}




















