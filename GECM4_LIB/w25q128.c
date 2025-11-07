/**
 ******************************************************************************
 * @file    w25q128.c
 * @author  ajl
 * @version V0.0.1
 * @date    4-November-2025
 * @brief
 *
 ******************************************************************************
 * @attention
 *
 * 
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "w25q128.h"



//厂家id参数未组合
uint8_t MID;
uint16_t DID;
uint8_t arraywrite[8];	//写入缓冲区
uint8_t arrayread[8];	//读取缓冲区


void Myspi_w_ss(uint8_t BitValue)//芯片使能位
{
	PBout(BitValue);
}

void W25q128_Init(void)//硬件spi初始化
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 关键：复用功能模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
	
	
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;//模式0
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;//0模式低电平起手
	SPI_InitStructure.SPI_CRCPolynomial=7;
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;//全双工双线
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;//高位先行
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;

	
	SPI_Init(SPI1,&SPI_InitStructure);
	SPI_Cmd(SPI1,ENABLE);
	
	Myspi_w_ss(1);
	
}

void Myspi_Start(void)
{
		Myspi_w_ss(0);
}
void myspi_stop(void)
{
		Myspi_w_ss(1);
}
/*
数据交换函数
parameter  bytesend    需要传递的参数   8位
return 返回的函数值 交换寄存器得到的数据

*/
uint8_t Myspi_Swapbyte(uint8_t bytesend)
{
	while(SET != SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1,bytesend);//发送数据  写入自动清除   写入完成的标志位
	while(SET != SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));//读取数据完成
	return SPI_I2S_ReceiveData(SPI1);//返回读取到的数据   读取完成后自动清除读取完成的标志位

}
void W25q64_ReadID(uint8_t *MID, uint16_t *DID)
{
		Myspi_Start();
		Myspi_Swapbyte(W25Q64_JEDEC_ID);
		*MID= Myspi_Swapbyte(W25Q64_DUMMY_BYTE);//返回厂家id
		*DID= Myspi_Swapbyte(W25Q64_DUMMY_BYTE);//设备id的高8位
		*DID<<=8;
		*DID|= Myspi_Swapbyte(W25Q64_DUMMY_BYTE);//设备id的高8位
		myspi_stop();
}

void w25q64_write(void)
{
	Myspi_Start();     
	Myspi_Swapbyte(W25Q64_WRITE_ENABLE);  //写使能
	myspi_stop();
}

void w25q64_waite(void)//写入占用检测
{
	uint32_t  timeout ;
	Myspi_Start();
	Myspi_Swapbyte(W25Q64_Read_Status_Register);
	timeout = 10000;
	while((Myspi_Swapbyte(W25Q64_DUMMY_BYTE) & 0x01)==0x01)   //连续读出状态寄存器
		{
			timeout--;
			if (timeout == 0)
			{
				break;
			}
		}
		myspi_stop();
}

void w25q64_pageprogram(uint32_t Address,uint8_t *DataArray,uint16_t Count)//页编程函数
{	
	uint16_t i;
	w25q64_write();   //写入的函数必须写使能 时序结束后会自动写失能
	Myspi_Start();
	Myspi_Swapbyte(W25Q64_Page_Program);
	Myspi_Swapbyte(Address>>16);   //12   不带括号的发送
	Myspi_Swapbyte(Address>>8);		 //34
	Myspi_Swapbyte(Address);       //56
	for(i=0;i<Count;i++)
	{
		Myspi_Swapbyte(DataArray[i]);
	}
	myspi_stop();
	w25q64_waite();
}



void w25q64_sectorerasse(uint32_t Address)//擦除扇
{
	w25q64_write();
	Myspi_Start();
	Myspi_Swapbyte(W25Q64_Sector_Erase);
	Myspi_Swapbyte(Address>>16);   //12
	Myspi_Swapbyte(Address>>8);		 //34
	Myspi_Swapbyte(Address);       //56
	myspi_stop();
	w25q64_waite();
}

void w25q64_ReadData(uint32_t Address,uint8_t *DataArray,uint32_t Count)
{
	uint32_t i;
	Myspi_Start();
	Myspi_Swapbyte(W25Q64_Read_Data);
	Myspi_Swapbyte(Address>>16);   //12   不带括号的发送
	Myspi_Swapbyte(Address>>8);		 //34
	Myspi_Swapbyte(Address);       //56
	for(i=0;i<Count;i++)
	{
		DataArray[i] = Myspi_Swapbyte(W25Q64_DUMMY_BYTE);
	}
	myspi_stop();
}


