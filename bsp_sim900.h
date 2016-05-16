#ifndef __BSP_SIM900_H__
#define __BSP_SIM900_H__

/*----添加需要包含的头文件----*/
//////////////////////////////////////////////////////////////////
#include "hal_uart5.h"
#include "hal_tim2.h"

#include "bsp_power.h"
#include "hal_uart4.h"
//////////////////////////////////////////////////////////////////


/*----需要挂载的函数（非常重要）----*/
////////////////////////////////////////////////////////////////////////////////////////////////
void BSP_SIM900_RecvIRQ(u8 data);		//需要挂载到串口接收中断函数里 data-接收到的数据
void BSP_SIM900_RecvTimeOutIRQ(void);		//需要挂载到定时器溢出中断函数里
////////////////////////////////////////////////////////////////////////////////////////////////


/*----参数配置----*/
////////////////////////////////////////////////////////////////////////
/*!<
	是否使用SIM900 STA引脚 检测开关机状态
	0-不使用(推荐)
	1-使用
*/
#define SIM900_STA_Enable 0

/*!<
	是否使用SIM900调试模式
	0-不使用(推荐)
	1-使用
	(如果开启调试模式,建议SIM900A串口通讯波特率小于调试串口波特率)
*/
#define SIM900_DEBUG_Enable 1

/*!<
	是否使用中断方式接收TCP数据包
	0-不使用
	1-使用(推荐)
*/
#define SIM900_TCP_RECVDATA_IRQ_Enable	1

/*!<
	SIM900串口接收缓冲区大小
*/
#define SIM900_RECVBUF_NUM 2048


/*!<
	SIM900 TCP接收数据缓冲区大小
*/
#define SIM900_RECVDATA_NUM 1536

////////////////////////////////////////////////////////////////////////


typedef struct
{
  volatile u8 recv_buf[SIM900_RECVBUF_NUM];
#if SIM900_TCP_RECVDATA_IRQ_Enable == 1
  volatile u8 recv_data[SIM900_RECVDATA_NUM];
#endif
  volatile u16 recv_point;
  volatile u16 recv_data_len;
	
	volatile u8 flag_timer_sta;		//溢出中断定时器状态标志位 0-关闭 1-计时中
	volatile u8 flag_handle_cmd;	//接收完成标志位 0-未完成  1-接收完成需要处理
	
	volatile u8 flag_data_recv_ok;		//TCP接收完成标志位
	volatile u8 flag_data_send_ok;		//TCP发送完成标志位
}SIM900_Type;
extern SIM900_Type sim900;

bool BSP_SIM900_SendRecCmd(char *cmd, char *ack, u8 times);

bool BSP_SIM900_StartingUp(u32 rate,u16 timeout);		//SIM900 开机
void BSP_SIM900_ShutDown(void);							//SIM900 关机

bool BSP_SIM900_ImeiGet(char* imei_asckii,u8 timeout);		//SIM900 获取IMEI码
bool BSP_SIM900_IccidGet(char* icid_asckii,u8 timeout);		//SIM900 获取ICCID码

bool BSP_SIM900_EchoSet(u8 sta,u8 timeout);
	

u8 BSP_SIM900_InitCheck(void);									//SIM900 初始化检测(检测SIM卡  检测GSM网络注册)
bool BSP_SIM900_RssiGet(u8 *rssi,u8 timeout);					//SIM900 获取网络信号强度
u8 BSP_SIM900_TcpInit(u8 ip1,u8 ip2,u8 ip3,u8 ip4,u16 port,char* str_apn);	//SIM900 TCP连接初始化
bool BSP_SIM900_TcpDefault(void);								//SIM900 关闭TCP连接
bool BSP_SIM900_TcpSendData(u8 *data,u16 len,u8 timeout);		//SIM900 通过TCP发送数据

#if SIM900_TCP_RECVDATA_IRQ_Enable == 1
	bool BSP_SIM900_TcpRecvData_Quick(u8 *data,u16 *len,u8 timeout);	//SIM900 通过TCP接收数据(中断方式)
#else
	bool BSP_SIM900_TcpRecvData_Stab(u8 *data,u16 *len,u8 timeout);		//SIM900 通过TCP接收数据(查询方式)
#endif

#endif

