/**
 ******************************************************************************
 * @file    esp8266.h
 * @author  ajl
 * @version V0.0.1
 * @date    30-Octorber-2025
 * @brief   0.0.2esp8266的应用函数
 * 			0.0.1完成了esp8266使用at命令模式配置连接热点
 * 			0.0.2完成了3sp8266使用连接到服务器，使用ap透传模式发送信息到服务器
 *
 ******************************************************************************
 * @attention
 *
 * 班函数是esp8266的应用层函数
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "esp8266.h" // Device header


// 全局变量定义
int8_t esp8266_transparent_transmission_sta = -1;	// 退出透传模式的状态标志位

int8_t Delay_flag = 0;
static uint8_t cnt = 0;   //静态变量函数退出后值不会变
struct esp8266_start esp8266 = {0};


/**
  * @brief  通过ESP8266模块来上网
  * @note   None
  * @param  None
  * @retval 成功：返回0
  *         失败：返回非0
  */
void Esp8266_Init(uint32_t baud_rate)
{
    
    Uart3_Init(baud_rate);

}


/**
  * @brief  通过ESP8266模块来上网
  * @note   None
  * @param  None
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t ESP8266_OnLineInit(void)
{
	int8_t ret = 0;
	
	// 1、退出透传模式，才能输入AT指令给ESP8266模块
	ret = ESP8266_ExitTransparentTransmission(); //？貌似就是返回0
	if(ret < 0)
	{
		printf("ESP8266_ExitTransparentTransmission error!\r\n");
		return -1;
	}
	printf("ESP8266_ExitTransparentTransmission success!\r\n"); //透传关闭成功
	
	Delay_ms(500);
	
	// 2、复位模块
	ret = ESP8266_Rst();
	if(ret < 0)
	{
		printf("ESP8266_Rst error!\r\n");
		return -2;
	}

	printf("ESP8266_Rst success!\r\n");
	Delay_ms(500);
	
	// 3、关闭回显 防止输入缓冲区的数据被污染
	ret = ESP8266_EnEcho(0);
	if(ret < 0)
	{
		printf("ESP8266_EnEcho error!\r\n");
		return -3;
	}
	printf("03: ESP8266_EnEcho success!\r\n");
	Delay_ms(500);
	
	// 4、连接热点  参数是账号和密码链接
	ret = ESP8266_ConnectAp(SSID, PSWD);
	if(ret < 0)
	{
		printf("ESP8266_ConnectAp error!\r\n");
		return -3;
	}
	printf("04: ESP8266_ConnectAp success!\r\n");
	Delay_ms(500);
	
	return 0;
}


/**
  * @brief  通过ESP8266模块来连接自己的ubuntu系统上部署的中转服务器
  * @note   None
  * @param  reg_msg：注册信息
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t ESP8266_ConnectMyYunServer(char* reg_msg)
{
	char buf[128] = {0};
	int8_t ret = 0;
	
	// 1、与ubuntu系统阿里云服务器建立连接
	ret = ESP8266_ConnectServer(SOCKET_MODE, SOCKET_IP, SOCKET_PORT);
	if(ret < 0)
	{
		printf("ESP8266_ConnectServer error!\r\n");
		return -1;
	}
	printf("01: ESP8266_ConnectServer success!\r\n");
	Delay_ms(500);
	
	// 2、开启透传模式
	ret = ESP8266_EnterTransparentTransmission();
	if(ret < 0)
	{
		printf("ESP8266_EnterTransparentTransmission error!\r\n");
		return -1;
	}
	printf("02: ESP8266_EnterTransparentTransmission success!\r\n");
	Delay_ms(500);
	
	// 3、向网站提出请求
	sprintf(buf, "#reg %s", reg_msg);
	ESP8266_SendMsg(buf);
	printf("02: ESP8266_SendMsg success!\r\n");
	
	
	return 0;
}


/**
  * @brief  退出透传模式
  * @note   None
  * @param  None
  * @retval 成功：返回0
  */
int8_t ESP8266_ExitTransparentTransmission(void)
{
	// 1、退出透传模式，发送下一条指令要间隔1S
	ESP8266_SendATCmd("+++");   //退出at的命令
	
	// 2、记录当前ESP8266模块工作在非透传模式下
	esp8266_transparent_transmission_sta = 0;
	
	return 0;
}

/**
  * @brief  发送ESP8266模块的AT指令
  * @note   None
  * @param  None
  * @retval None
  */
void ESP8266_SendATCmd(const char* cmd_str)
{
	// 1、清空相关数据 方便后面的send传输
	Uart_DataArray_clear((uint8_t*)uart3_recvbuf, 128);
	uart3_count = 0;
	uart3_flag  = 0;
	uart3_len   = 0;
	
	// 2、串口3发送字符串数据
	USART3_SendStr(cmd_str);
}

void ESP8266_SendMsg(const char* msg_str)
{
	// 1、清空相关数据
	Uart_DataArray_clear((uint8_t*)uart3_recvbuf, 128);
	uart3_count = 0;
	uart3_flag  = 0;
	uart3_len   = 0;
	
	// 2、串口3发送字符串数据
	USART3_SendStr(msg_str);
}


/**
  * @brief  模块复位
  * @note   None
  * @param  None
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t ESP8266_Rst(void)
{
	ESP8266_SendATCmd("AT+RST\r\n");    //发送AT的复位指令
	if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)  
		return -1;
	return 0 ;	
}


/**
  * @brief  回显打开或关闭
  * @note   None
  * @param  flag：开关模块的回显信息
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t ESP8266_EnEcho(int8_t flag)
{
	
	if(flag == 1)
		ESP8266_SendATCmd("ATE1\r\n");			// 开启回显
	else
		ESP8266_SendATCmd("ATE0\r\n");			// 关闭回显
	
	if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
		return -1;
	return 0 ;	 
}


/**
  * @brief  检查接收的数据包里面的字符串
  * @note   None
  * @param  str：    接收字符串里面的特殊字符串标志(wifi模块返回的数据)
  *         timeout：超时时间
  * @retval 成功：返回0
  *         失败：返回-1
  */
int8_t ESP8266_CheckStrInRxbuf(char* str, int32_t timeout)
{
	char *dest = str;
	char *src  = (char*)uart3_recvbuf; // OK 0,CONNECT
	
	// 等待串口接收完毕或超时
	while( (strstr(src, dest) == NULL) && (timeout>0)) //当查找失败和超时的时候退出
	{
		Delay_ms(1);//超时的等待函数
		timeout--;
	}
	
	if(timeout>0)   //正常发回返回0
		return 0;

	return -1;  //超时错误都会返回-1
}

/**
  * @brief  连接热点
  * @note   None
  * @param  ssid：热点的名字
  *         pswd：热点的密码
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t ESP8266_ConnectAp(char *ssid, char *pswd)
{
	// 1、设置为客户端模式可以station模式可以联机不当服务器
	ESP8266_SendATCmd("AT+CWMODE=1\r\n");
	if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)//检查OK超时时间设置为5s
		return -1;	
	
	Delay_ms(500); 
	
	// 2、连接目标热点
    char at_cmd_buf[128]; // 确保缓冲区足够大

    // 拼接完整的 AT 命令到缓冲区  \"的目的是进行转义确保"这个符号正确表达
    int len = sprintf(at_cmd_buf, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pswd);
	
	if(ESP8266_CheckStrInRxbuf("WIFI GOT IP", 5000) == -1)
	{
		if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
		{
			return -1;	
		}
	}
	
	return 0;   //链接成功
		
}


/**
  * @brief  连接服务器
  * @note   可以指定协议进行连接(TCP/UDP)
  * @param  mode：协议类型(TCP/UDP)
  *         ip：  目标服务器的IP地址
  *         port：目标服务器的端口号(49151-65535)   // 推荐, 平常做实验可用的端口号范围
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t ESP8266_ConnectServer(char *mode, char *ip, uint16_t port)
{
	char port_buf[16] = {0};
	// 连接服务器
	ESP8266_SendATCmd("AT+CIPSTART=");

	ESP8266_SendATCmd("\""); ESP8266_SendATCmd(mode); 	ESP8266_SendATCmd("\"");	// mode
	ESP8266_SendATCmd(","); 
	ESP8266_SendATCmd("\""); ESP8266_SendATCmd(ip); 	ESP8266_SendATCmd("\"");	// ip
	ESP8266_SendATCmd(",");
	sprintf(port_buf,"%d", port);													// port
	ESP8266_SendATCmd(port_buf);
	ESP8266_SendATCmd("\r\n");  
	
	if(ESP8266_CheckStrInRxbuf("CONNECT", 5000) == -1)
	{
		if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
		{
			return -1;	
		}
	}
	
	return 0;		
}


/**
  * @brief  断开连接服务器
  * @note   None
  * @param  None
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t ESP8266_DisConnectServer(void)
{
	// 断开服务器
	ESP8266_SendATCmd("+++");					//先关闭透传模式再关闭wifi
	ESP8266_SendATCmd("AT+CIPCLOSE\r\n");
	if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
		return -1;	
	return 0;		
}


/**
  * @brief  进入透传模式
  * @note   None
  * @param  None
  * @retval 成功：返回0
  *         失败：返回非0
  */
int8_t ESP8266_EnterTransparentTransmission(void)
{
	// 1、进入透传模式
	ESP8266_SendATCmd("AT+CIPMODE=1\r\n");
	if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
		return -1;
		
	// // 2、复位
	// ESP8266_SendATCmd("AT+RST\r\n");
	// if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
	// 	return -2;	
	
	// 3、开启发送模式
	ESP8266_SendATCmd("AT+CIPSEND\r\n");
	if(ESP8266_CheckStrInRxbuf("OK", 5000) == -1)
		return -3;
	
	// 4、记录当前ESP8266模块工作在透传模式下
	esp8266_transparent_transmission_sta = 1;

	return 0;
}

void esp8266_test_demo(void)
{

	if (KEY1 == 0) // 退出服务器
	{
		printf("exit");
		// 消抖并只让按一次
		Delay_ms(20);
		while (KEY1 == 0)
			;
		Delay_ms(20);

		// 退出服务器
		ESP8266_DisConnectServer();
		Esp8266_Init(115200);
	}

	if (KEY2 == 0) // 和机器人聊天
	{

		printf("robot");
		// 消抖并只让按一次
		Delay_ms(20);
		while (KEY2 == 0)
			;
		Delay_ms(20);

		Delay_ms(200);
		ESP8266_SendMsg("#robot nihao");
	}

	// ESP模块处在透传模式下，并且接收esp8266模块数据中断完毕
	if ((esp8266_transparent_transmission_sta == 1) && (uart3_flag == 1))
	{

		printf("u3_recvbuf == %s\r\n", uart3_recvbuf);
		// 判断
		if (MY_LIB_CmpArray((int8_t *)uart3_recvbuf, (int8_t *)"BUZZER_ON", 9) == 0)
		{
			// Beep_Flag = 1;
		}
		if (MY_LIB_CmpArray((int8_t *)uart3_recvbuf, (int8_t *)"BUZZER_OFF", 10) == 0)
		{
			// Beep_Flag = 0;
		}

		// 清空相关数据，方便下次接收
		Uart_DataArray_clear((uint8_t *)uart3_recvbuf, 128);
		uart3_count = 0;
		uart3_flag = 0;
		uart3_len = 0;
	}

}

/**
  * @brief  esp8266数据上报函数
  * @note   该函数调用后每1s向云服务器发送DHT11采集的数据
  * @param  
  * @retval 
  *         
  */
void esp8266_DataReport(void)
{
	char data[64];
	// sprintf(data,"#chat stm32 huminity%d,temporture%d",DHT11_Data[0],DHT11_Data[2]);	//拼接温湿度数据进行发送  可以改成json发送

	ESP8266_SendMsg(data);

}

void esp8266_Timer_Tick(void)
{
	cnt++;
    if (cnt > 100)  //计时1s
    {
        esp8266.Esp8266_Report_Flag1 = 1;
        cnt = 0;
    }
}

