#ifndef __W25Q64_INS_H
#define __W25Q64_INS_H

#define W25Q64_WRITE_ENABLE  					0x06
#define W25Q64_Volatile_SR Write Enable 		0x50
#define W25Q64_Write_Disable 					0x04
#define W25Q64_Release_Powerdown				0xAB
#define W25Q64_Device_ID 						0x90
#define W25Q64_JEDEC_ID							0x9F
#define W25Q64_Read_Unique_ID					0x4B
#define W25Q64_Read_Data 						0x03
#define W25Q64_Fast_Read 						0x0B
#define W25Q64_Page_Program 					0x02
#define W25Q64_Sector_Erase 					0x20  
#define W25Q64_Block_Erase						0x52
#define W25Q64_Block_Erase64                	0xD8
#define W25Q64_Chip_Erase                   	0xC7
#define W25Q64_Power_down                   	0xB9
#define W25Q64_Enable_Reset                 	0x66
#define W25Q64_Reset Device                 	0x99
#define W25Q64_Read_Status_Register         	0x05


#define W25Q64_DUMMY_BYTE                  		0xFF



#endif

