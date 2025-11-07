#include "flash.h"

/**
  * @brief  片内FLASH写入数据
  * @note   None
  * @param  FLASH_Sector: 要擦除的扇区
            Address：     扇区0-23的地址(建议从扇区4开始，扇区0-3编译程序的空间用的)
            Data_buf：    要写入的数据(每个数据1个字节)
			len：         要写入数据的长度
  * @retval None
  */ 
void FLASH_WriteData(uint32_t FLASH_Sector, uint32_t Address, uint8_t *Data_buf, uint16_t len)
{
	/* 0、函数功能变量区域 */
	int16_t i = 0;
	
	/* 1、启用flash控制寄存器访问(解锁Flash) */
	FLASH_Unlock();

	/* 2、清除挂起的标志(如果有) */  
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 

	/* 3、擦除扇区() */
	if (FLASH_EraseSector(FLASH_Sector, VoltageRange_3) != FLASH_COMPLETE)
	{ 
		printf("FLASH_EraseSector error!\r\n");
		while(1);
	}

	//4、写入数据(字写入， 32位)
	/*
		FLASH_ProgramDoubleWord(uint32_t Address, uint64_t Data);		// 双字写入，8个字节，64位
	    FLASH_ProgramWord(uint32_t Address, uint32_t Data);				// 单字写入，4个字节，32位
	    FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);			// 半字写入，2个字节，16位
	    FLASH_ProgramByte(uint32_t Address, uint8_t Data);				// 字节写入，1个字节，8位
	*/
	for(i=0; i<len; i++)
	{                     
		if (FLASH_ProgramByte(Address+i, *Data_buf) != FLASH_COMPLETE)	// 0x08010000扇区4的地址 
		{
			printf("FLASH_ProgramByte error!\r\n");
			while(1);
		}
		Data_buf++;
	}

	// 5、锁定Flash，确保数据内容不会被任意更改
	FLASH_Lock(); 
}


/**
  * @brief  片内FLASH读取数据
  * @note   None
  * @param  Address：  扇区0-23的地址(建议从扇区4开始，扇区0-3编译程序的空间用的)
            Data_buf： 要写入的数据(每个数据1个字节)
            len：      要写入数据的长度
  * @retval None
  */ 
void FLASH_ReadData(uint32_t Address, uint8_t *Data_buf, uint16_t len)
{
	volatile uint32_t * pdata = (volatile uint32_t *)Address;	// 此处强转目的是为了让其从值变为地址
	int16_t i = 0;
	for(i=0; i<len; i++)
	{
		Data_buf[i] = *((volatile uint8_t * )pdata+i);			// 此处强转目的是为了能够获取单字节的数据，并赋值给字符串数组
	}
}



