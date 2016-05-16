/*

===========================================================================
                       ##### 如何使用这个驱动 #####
===========================================================================
(1)在.h文件中添加好需要的头文件。
(2)按照注释编写好“移植部分”的代码。
(3)挂载 BSP_SIM900_RecvIRQ() 和 BSP_SIM900_RecvTimeOutIRQ() 函数到相应的中断。
(4)在.h文件中配置好相应的参数。
(5)调用 BSP_SIM900_StartingUp() 函数来使能SIM900的电源及与SIM900有关的单片机外设。
(6)调用 BSP_SIM900_InitCheck() 函数来检测SIM900模块是否初始化完成。（检测SIM卡，检测GSM网络注册）
(7)通过以上几步后，便可对SIM900进行正常使用，可调用 BSP_SIM900_SendRecCmd("AT\r\n","OK",2); 进行测试。
(8)调用 BSP_SIM900_ShutDown() 来关闭SIM900电源及与SIM900相关的单片机外设。

*/
#include "bsp_sim900.h"

SIM900_Type sim900;



/*
需要移植的代码：
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//SIM900调试输出串口函数,输出接收到的SIM900数据到另外一个串口(只在开启调试模式下有效)
#if SIM900_DEBUG_Enable == 1
	#define DEBUG_RECV_PRINTF(data)		HAL_UART4_SendByte(data)	//输出一个Byte
#endif

//SIM900A电源IO控制
#define SIM900_P_EN				BSP_POWER_GSM(1)	//使能SIM900电源
#define SIM900_P_DN				BSP_POWER_GSM(0)	//关闭SIM900电源

//SIM900状态指示IO读取
#if SIM900_STA_Enable == 1
//	#define SIM900_STA_R	GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0)		//SIM900开关机状态引脚读取
#endif


//接口函数-10毫秒级延时函数
#define BSP_SIM900_Delay10ms_Port(ms)		CoTickDelay(ms*10)

//接口函数-发送命令函数(串口发送一串字符串)
#define BSP_SIM900_SendCmd_Port(p_str)		HAL_UART5_SendStr(p_str)

//接口函数-发送数据函数(串口发送指定长度的数据)
#define BSP_SIM900_SendData_Port(p_data,len)		HAL_UART5_SendData(p_data,len)

//接口函数-串口超时计数器开启/关闭函数
#define BSP_SIM900_TimeOutStart_Port()		HAL_TIM2_Start()		//开启计数器并归零计数器
#define BSP_SIM900_TimeOutStop_Port()			HAL_TIM2_Stop()			//关闭计数器

/*
 ************************************************************
 *  名称：	BSP_SIM900_IO_Init_Port()
 *  功能：	接口函数-SIM900A连接引脚初始化(如果开启调试模式,建议波特率小于调试输出串口波特率)
 *	输入：  rate-串口波特率Hz  timeout-串口一帧数据超时时间ms
 *	输出：  无
 ************************************************************
*/
void BSP_SIM900_IO_Init_Port(u32 rate,u16 timeout)
{
	//-----------------串口接收超时定时器初始化-----------------
	HAL_TIM2_Init(timeout);
	BSP_SIM900_TimeOutStop_Port();
	
	//-----------------USART IO初始化-----------------
	HAL_UART5_Init(rate);
	
	//-----------------SIM900电源IO初始化-----------------
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	
//	GPIO_InitTypeDef	GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);

//	SIM900_P_DN;
	
	//-----------------SIM900状态指示IO初始化-----------------
#if SIM900_STA_Enable == 1
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
#endif
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_IO_Default_Port()
 *  功能：	接口函数-SIM900A连接引脚失能（除电源控制引脚外）
 *	输入：  无
 *	输出：  无
 ************************************************************
*/
void BSP_SIM900_IO_Default_Port(void)
{
	//-----------------串口接收超时定时器失能-----------------
	HAL_TIM2_Default();
	
	//-----------------USART IO失能-----------------
	HAL_UART5_Default();
	
	//-----------------SIM900状态指示IO失能-----------------
#if SIM900_STA_Enable == 1	
//	GPIO_InitTypeDef	GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
#endif
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_RecvFinished_Hook()
 *  功能：	SIM900接收数据包完成后调用的钩子函数，可在此函数内对数据进行处理，由于是在中断中进行处理，所以函数内不可调用延时函数！
 			(接收到的数据sim900.recv_data[x]  接收到的字节数sim900.recv_data_len)
 *	输入：  无
 *	输出：  无
 ************************************************************
*/
void BSP_SIM900_RecvDataFinished_Hook(void)
{
	__NOP();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 ************************************************************
 *  名称：	BSP_SIM900_StartingUp()
 *  功能：	SIM900开机
 *	输入：  rate-串口波特率Hz  timeout-串口一帧数据超时时间
 *	输出：  true-开机成功  false-开机失败
 ************************************************************
*/
bool BSP_SIM900_StartingUp(u32 rate,u16 timeout)
{
	sim900.flag_timer_sta = 0;
  
	//SIM900 IO初始化
	BSP_SIM900_IO_Init_Port(rate,timeout);		
	
	//SIM900电源打开
	SIM900_P_EN;		

	//等待SIM900开机完成
#if SIM900_STA_Enable == 1
	u8 i=5;
	while(1)
	{
		BSP_SIM900_Delay10ms_Port(100);
		if(SIM900_STA_R == 1)	break;
		i--;
		if(i == 0) return false;
	}
#else
	BSP_SIM900_Delay10ms_Port(100);		//延时1秒
#endif
	
	sim900.recv_point = 0;
	sim900.flag_handle_cmd = 0;
	sim900.recv_data_len = 0;
	
	sim900.flag_data_recv_ok = 0;
	sim900.flag_data_send_ok = 0;
	
	return true;
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_ShutDown()
 *  功能：	SIM900关机
 *	输入：  无
 *	输出：  无
 ************************************************************
*/
void BSP_SIM900_ShutDown(void)
{
	//SIM900电源关闭
	SIM900_P_DN;
	
	//SIM900 IO失能
	BSP_SIM900_IO_Default_Port();

	//等待SIM900关闭 等待1S
//	BSP_SIM900_Delay10ms_Port(100);
	
	sim900.flag_timer_sta = 0;
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_SendRecCmd()
 *  功能：	SIM900 发送接收命令
 *	输入：  *cmd-发送的命令  *ack-要接收的命令  times-等待接收命令时间,单位s
 *	输出：  false-失败  true-成功
 ************************************************************
*/
bool BSP_SIM900_SendRecCmd(char *cmd, char *ack, u8 times)
{
	if(times == 0) return false;
	
	while(sim900.flag_timer_sta == 1)	//等待接收完成一帧	
	{
		BSP_SIM900_Delay10ms_Port(2);	//20ms
	}
	
	sim900.recv_point = 0;
	sim900.flag_handle_cmd = 0;
	sim900.recv_buf[0] = 0;
	
	//发送命令
	BSP_SIM900_SendCmd_Port(cmd);
	
	//等待检测回应命令
	while(1)
	{
		for(u8 i=0;i<10;i++)
		{
			BSP_SIM900_Delay10ms_Port(10);	//100ms
		
			if(sim900.flag_handle_cmd == 1)	//判断是否接收完成一帧
			{
				if(strstr((char*)sim900.recv_buf,ack))	return true;
				else return false;
			}
		}
		
		times--;
		if(times == 0) return false;
	}
}

/////////////////////////////////////////////////////////////////AT指令部分/////////////////////////////////////////////////////////////////

/*
 ************************************************************
 *  名称：	BSP_SIM900_ImeiGet()
 *  功能：	SIM900 获取IMEI编号
 *	输入：  IMEI指针数组(15位ASCKII码形式返回)  timeout-超时时间(1)
 *	输出：  false-获取失败  true-获取成功
 ************************************************************
*/
bool BSP_SIM900_ImeiGet(char* imei_asckii,u8 timeout)
{
	char *p;
	if(true == BSP_SIM900_SendRecCmd("AT+GSN\r\n","OK",timeout))
	{
		p = strstr((char*)sim900.recv_buf, "OK");
		p = p-19;

		for(u8 i=0;i<15;i++)
		{
			imei_asckii[i] = p[i];
		}
		return true;
	}
	else
	{
		return false;
	}
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_IccidGet()
 *  功能：	SIM900 获取ICCID
 *	输入：  ICCID指针数组(20位ASCKII码形式返回)  timeout-超时时间(1)
 *	输出：  false-获取失败  true-获取成功
 ************************************************************
*/
bool BSP_SIM900_IccidGet(char* icid_asckii,u8 timeout)
{
	char *p;
	if(true == BSP_SIM900_SendRecCmd("AT+CCID\r\n","OK",timeout))
	{
		p = strstr((char*)sim900.recv_buf, "OK");
    p = p-24;
  
    for(u8 i=0;i<20;i++)
    {
      icid_asckii[i] = p[i];
    }
    return true;
	}
	else
	{
		return false;
	}
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_EchoSet()
 *  功能：	SIM900 回显设定
 *	输入：  0-关闭回显  1-开启回显  timeout-超时时间(1)
 *	输出：  false-设定失败  true-设定成功
 ************************************************************
*/
bool BSP_SIM900_EchoSet(u8 sta,u8 timeout)
{
	if(sta == 0)
	{
		return BSP_SIM900_SendRecCmd("ATE0\r\n","OK",timeout);
	}
	else if(sta == 1)
	{
		return BSP_SIM900_SendRecCmd("ATE1\r\n","OK",timeout);
	}
	else
	{
		return false;
	}
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_SimCheck()
 *  功能：	SIM900 检测SIM卡
 *	输入：  timeout-超时时间(1)
 *	输出：  false-检测不到SIM卡  true-成功
 ************************************************************
*/
bool BSP_SIM900_SimCheck(u8 timeout)
{
	return BSP_SIM900_SendRecCmd("AT+CPIN?\r\n", "READY", timeout);
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_NetCheck()
 *  功能：	SIM900 检测网络状态
 *	输入：  timeout-超时时间(1)
 *	输出：  false-网络未注册  true-网络已注册
 ************************************************************
*/
 bool BSP_SIM900_NetCheck(u8 timeout)
 {
 	char *p;
 	if(true == BSP_SIM900_SendRecCmd("AT+CREG?\r\n","OK",timeout))
 	{
 		p = strstr((char*)sim900.recv_buf,"+CREG: ");
 		if(*(p+9) == '1' || *(p+9) == '5') return true;
		else return false;
 	}
 	else
 	{
 		return false;
 	}
}


/*
 ************************************************************
 *  名称：	BSP_SIM900_InitCheck()
 *  功能：	SIM900 初始化检测
 			(检测SIM卡  检测GSM网络注册)
 *	输入：  无
 *	输出：  0-初始化完成
 			1-发送"AT"命令不回应
			2-关闭回显失败
 			3-检测不到SIM卡
 			4-注册网络失败
 ************************************************************
*/
u8 BSP_SIM900_InitCheck(void)
{
	u8 i;

	i=6;
	while(1)
	{
		if(true == BSP_SIM900_SendRecCmd("AT\r\n","OK",1)) break;
		
		BSP_SIM900_Delay10ms_Port(200);
		i--;
		if(i == 0) return 1;
	}
	
	//关闭回显
	i=3;
	while(1)
	{
		if(true == BSP_SIM900_EchoSet(0,1)) break;
	
		BSP_SIM900_Delay10ms_Port(200);
		i--;
		if(i == 0) return 2;
	}

	
	i=5;
	while(1)
	{
		if(true == BSP_SIM900_SimCheck(1)) break;
		
		BSP_SIM900_Delay10ms_Port(200);
		i--;
		if(i == 0) return 3;
	}

	i=30;
	while(1)
	{
		if(true == BSP_SIM900_NetCheck(2)) break;

		BSP_SIM900_Delay10ms_Port(200);
		i--;
		if(i == 0) return 4;
	}

	return 0;
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_RssiGet()
 *  功能：	SIM900 获取网络信号强度
 *	输入：  信号指针0-31 单位asu  timeout-超时时间(1)
 *	输出：  false-获取失败  true-获取成功
 ************************************************************
*/
bool BSP_SIM900_RssiGet(u8 *rssi,u8 timeout)
{
	char *p;
	if(true == BSP_SIM900_SendRecCmd("AT+CSQ\r\n","OK",timeout))
	{
		p = strstr((char*)sim900.recv_buf, "+CSQ: ");
		*rssi = atoi(p+6);
		return true;
	}
	else
	{
		return false;
	}
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_GprsServiceSet()
 *  功能：	SIM900 附着/关闭GPRS业务
 *	输入：  0-关闭  1-附着  timeout-超时时间(3)
 *	输出：  false-失败  true-成功
 ************************************************************
*/
bool BSP_SIM900_GprsServiceSet(u8 sta,u8 timeout)
{
	if(sta == 0)
	{
		return BSP_SIM900_SendRecCmd("AT+CGATT=0\r\n","OK",timeout);
	}
	else if(sta == 1)
	{
		return BSP_SIM900_SendRecCmd("AT+CGATT=1\r\n","OK",timeout);
	}
	else
	{
		return false;
	}
 	
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_IpHeadSet()
 *  功能：	SIM900 设置接收数据是否显示IP头
 *	输入：  0-关闭  1-开启  timeout-超时时间(2)
 *	输出：  false-失败  true-成功
 ************************************************************
*/
bool BSP_SIM900_IpHeadSet(u8 sta,u8 timeout)
{
	if(sta == 0)
	{
		return BSP_SIM900_SendRecCmd("AT+CIPHEAD=0\r\n","OK",timeout);
	}
	else if(sta == 1)
	{
		return BSP_SIM900_SendRecCmd("AT+CIPHEAD=1\r\n","OK",timeout);
	}
	else
	{
		return false;
	}
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_TcpConnect()
 *  功能：	SIM900 建立TCP连接
 *	输入：  ipx-IP地址  port-端口号  timeout-超时时间(20)
 *	输出：  false-失败  true-成功
 ************************************************************
*/
bool BSP_SIM900_TcpConnect(u8 ip1,u8 ip2,u8 ip3,u8 ip4,u16 port,u8 timeout)
{
	char buf[60];
	
	sprintf(buf,"AT+CIPSTART=\"TCP\",\"%1d.%1d.%1d.%1d\",\"%1d\"\r\n",
          ip1,ip2,ip3,ip4,port);

	if(true == BSP_SIM900_SendRecCmd(buf,"OK",2))
	{
		return BSP_SIM900_SendRecCmd("","CONNECT OK",timeout);
	}
	else
	{
		return false;
	}
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_TcpInit()
 *  功能：	SIM900 TCP连接初始化
 *	输入：  ipx-IP地址  port-端口号  str_apn-APN接入点
 *	输出：  0-初始化完成  
			      1-设置接收数据显示IP头失败
 			      2-附着GPRS业务失败
 			      3-发起TCP连接失败
            4-APN接入点设定失败
 ************************************************************
*/
u8 BSP_SIM900_TcpInit(u8 ip1,u8 ip2,u8 ip3,u8 ip4,u16 port,char* str_apn)
{
	u8 i;
	
	//是否设定APN接入点
	if(*str_apn != 0)
	{
		char buf[64+22];	//APN名称最多64个字节
		
		i = 3;
		while(1)
		{
			sprintf(buf,"AT+CGDCONT=1,\"IP\",\"%s\"\r\n",str_apn);
			if(true == BSP_SIM900_SendRecCmd(buf,"OK",2)) break;
			BSP_SIM900_Delay10ms_Port(200);
			i--;
			if(i == 0) return 4;
		}
		
		i = 3;
		while(1)
		{
			sprintf(buf,"AT+CIPCSGP=1,\"%s\"\r\n",str_apn);
			if(true == BSP_SIM900_SendRecCmd(buf,"OK",2))	break;
			
			BSP_SIM900_Delay10ms_Port(200);
			i--;
			if(i == 0) return 4;
		}
	}
	
	i=3;
	while(1)
	{
		if(true == BSP_SIM900_IpHeadSet(1,2)) break;		//设置接收数据显示IP头
		
		BSP_SIM900_Delay10ms_Port(200);
		i--;
		if(i == 0) return 1;
	}
	
	i=2;
	while(1)
	{
		if(true == BSP_SIM900_GprsServiceSet(1,10)) break;		//附着GPRS业务
		
		BSP_SIM900_Delay10ms_Port(10);
		i--;
		if(i == 0) return 2;
	}

	if(true == BSP_SIM900_TcpConnect(ip1,ip2,ip3,ip4,port,20)) return 0;
	else return 3;
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_TcpDefault()
 *  功能：	SIM900 关闭TCP连接
 *	输入：  无
 *	输出：  false-失败  true-成功
 ************************************************************
*/
bool BSP_SIM900_TcpDefault(void)
{
	return	BSP_SIM900_SendRecCmd("AT+CIPCLOSE=1\r\n","CLOSE OK",2);
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_TcpSendData()
 *  功能：	SIM900 通过TCP发送数据 (在此之前必须关闭模块回显！)
 *	输入：  *data-数据指针  len-数据长度  timeout-超时时间(5)
 *	输出：  false-失败  true-成功
 ************************************************************
*/
bool BSP_SIM900_TcpSendData(u8 *data,u16 len,u8 timeout)
{
	char *p;
	char buf[20];
	
	sim900.flag_data_send_ok = 0;
	
	if(true == BSP_SIM900_SendRecCmd("AT+CIPSEND?\r\n","OK",3))		//查询最大发送字节数
	{
		p = strstr((char*)sim900.recv_buf, "+CIPSEND: ");
		if(len <= atoi(p+10))		//是否超过最大发送字节数
		{
			sprintf(buf,"AT+CIPSEND=%1d\r\n",len);
			if(true == BSP_SIM900_SendRecCmd(buf,">",3))		//等待SIM900回应">"
			{
				
				BSP_SIM900_SendData_Port(data,len);		//发送数据
				
				//等待SIM900回应"SEND OK"
				u16 times = timeout*10;
				while(1)
				{
					BSP_SIM900_Delay10ms_Port(10);	//100ms
					
					if(sim900.flag_data_send_ok == 1)
					{
						sim900.flag_data_send_ok = 0;
						return true;
					}
					
					times--;
					if(times == 0)
					{
						return false;
					}
				}
			}
			else return false;
		}
		else return false;
	}
	else return false;
}

#if SIM900_TCP_RECVDATA_IRQ_Enable == 1
/*
 ************************************************************
 *  名称：	BSP_SIM900_TcpRecvData_Quick()
 *  功能：	SIM900 通过TCP接受数据 (在此之前必须关闭模块回显！)（通过中断的方式接受数据）
 *	输入：  *data-接收到的数据  *len-数据长度  timeout-超时时间s
 *	输出：  false-失败  true-成功
 ************************************************************
*/
bool BSP_SIM900_TcpRecvData_Quick(u8 *data,u16 *len,u8 timeout)
{
	//同步接收模式
	if(timeout != 0)
	{
		u16 time = (u16)timeout*100;
		
		while(1)
		{
			if(sim900.flag_data_recv_ok == 1)
			{
				*len = sim900.recv_data_len;
				
				for(u16 i =0;i<(*len);i++)
				{
					data[i] = sim900.recv_data[i];
				}
				
				sim900.flag_data_recv_ok = 0;
				return true;
			}
			BSP_SIM900_Delay10ms_Port(1);
			
			time--;
			if(time == 0)
			{
				return false;
			}
		}
	}
	//异步接收模式
	else	
	{
		if(sim900.flag_data_recv_ok == 1)
		{
			*len = sim900.recv_data_len;
			
			for(u16 i =0;i<(*len);i++)
			{
				data[i] = sim900.recv_data[i];
			}
			
			sim900.flag_data_recv_ok = 0;
			return true;
		}
		else
		{
			return false;
		}
	}
}
#else
bool BSP_SIM900_TcpRecvData_Stab(u8 *data,u16 *len,u8 timeout)
{
	if(timeout == 0) return false;
	
	while(sim900.flag_timer_sta == 1)	//等待接收完成一帧	
	{
		BSP_SIM900_Delay10ms_Port(2);	//20ms
	}
	
	sim900.recv_point = 0;
	sim900.flag_handle_cmd = 0;
	sim900.recv_buf[0] = 0;
	
	while(1)
	{
		for(u8 i=0;i<10;i++)
		{
			BSP_SIM900_Delay10ms_Port(10);	//100ms
		
			if(sim900.flag_handle_cmd == 1)	//判断是否接收完成一帧
			{
				char *p = strstr((char*)sim900.recv_buf,"+IPD,");
				if(p)
				{
					u16 temp;
					u16 i=0;

					(*len) = atoi(p+5);		//获取接收字节数

					temp = (*len);		//计算sim900.recv_data_len的位数	
					while(temp)
					{
						temp = temp/10;
						i++;
					}
							
					p = p+i+6;		//p指向数据的首地址

					for(i=0;i<(*len);i++)		//取出接收到的数据
					{
						data[i] = p[i];
					}
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		
		timeout--;
		if(timeout == 0) return false;
	}
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 ************************************************************
 *  名称：	BSP_SIM900_RecvIRQ()
 *  功能：  SIM900串口接收函数（需要挂在到串口接收中断函数里）
 *	输入：  data-串口接收到的数据
 *	输出：  无
 ************************************************************
*/
void BSP_SIM900_RecvIRQ(u8 data)
{
	BSP_SIM900_TimeOutStart_Port();	//定时器清零
	sim900.flag_timer_sta = 1;	
	
	if(sim900.recv_point < (SIM900_RECVBUF_NUM-1))
	{
		sim900.recv_buf[sim900.recv_point++] = data;	//接收数据到缓冲区
	}
	else
	{
		sim900.recv_point = 0;
	}
#if SIM900_DEBUG_Enable == 1	
	DEBUG_RECV_PRINTF(data);
#endif
}

/*
 ************************************************************
 *  名称：	BSP_SIM900_RecvTimeOutIRQ()
 *  功能：  SIM900串口接收超时函数（需要挂在到定时器溢出中断函数里）
 *	输入：  无
 *	输出：  无
 ************************************************************
*/
void BSP_SIM900_RecvTimeOutIRQ(void)
{
	BSP_SIM900_TimeOutStop_Port();	//定时器关闭
	sim900.flag_timer_sta = 0;
	
	sim900.flag_handle_cmd = 1;
	
	sim900.recv_buf[sim900.recv_point] = 0;		//最后一个字符已0结尾 方便strstr查找
	
	
	//----------------判断"SEND OK"字符----------------
	if(strstr((char*)sim900.recv_buf,"SEND OK"))
	{
		sim900.flag_data_send_ok = 1;
	}
	
#if SIM900_TCP_RECVDATA_IRQ_Enable == 1
	//----------------判断并接收TCP数据包----------------
	char *p = strstr((char*)sim900.recv_buf,"+IPD,");
	if(p)
	{
		u16 temp;
		u16 i=0;

		sim900.recv_data_len = atoi(p+5);		//获取接收字节数

		temp = sim900.recv_data_len;		//计算sim900.recv_data_len的位数	
		while(temp)
		{
			temp = temp/10;
			i++;
		}
        
		p = p+i+6;		//p指向数据的首地址

		for(i=0;i<sim900.recv_data_len;i++)		//取出接收到的数据
		{
			sim900.recv_data[i] = p[i];
		}
		
		sim900.recv_point = 0;
		sim900.flag_handle_cmd = 0;
		sim900.recv_buf[0] = 0;
		
		sim900.flag_data_recv_ok = 1;
		
		BSP_SIM900_RecvDataFinished_Hook();		//接收完成钩子函数
	}
#endif
}
