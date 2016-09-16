#ifndef __SOCK_COMMON_H__
#define __SOCK_COMMON_H__

//超时无应答
#define SOCK_ERROR_OVERTIME                 5  

//MAX MSG
#define SOCK_MSG_MAX 1024000

//CMD RSP TIMEOUT(ms)
#define SOCK_MSG_TIMEOUT 5000

//缓冲接收的msg数目	100
#define SOCK_MSG_BUFNUM 100

//mc心跳超时
#define MC_BEATEN_TIMEOUT	(120*10) 

//自定义消息，和主MFC进程中的一致
#define WM_MESSAGE_TRAY (WM_USER+100)  // about tray
#define WM_MESSAGE_EVENT (WM_USER+101)  // 
#define WM_MESSAGE_REFRESH (WM_USER+102)  // 


#define SOCK_MC_ONLINE  0x01 
#define SOCK_MC_OFFLINE  0x02
#define SOCK_LD_ONLINE  0x100001 
#define SOCK_LD_OFFLINE  0x100002

/**********************************定义系统返回值及错误类型*****************************************/
//成功
#define SOCK_OK                         0
//致命错误
#define SOCK_ERROR                    100            
//当前用户无权限
#define SOCK_ERROR_NOPERMIT    1
//用户无效（已掉线等情况）
#define SOCK_ERROR_USER_NOLOGIN                 2  
/***********************注册服务器错误***********************/
//ID无效
#define SOCK_ERROR_SERVER_IDENTIFY      11
//不支持设备（服务器端）
#define SOCK_ERROR_SERVER_DEVICE        12
/******************************客户端登录************************************/
//用户不存在
#define SOCK_ERROR_USER_NO                   13
//密码验证失败
#define SOCK_ERROR_USER_PWD                        14
//用户已登录
#define SOCK_ERROR_USER_LOGINED               15
//登录用户数已满
#define SOCK_ERROR_USER_LOGINFULL                 16  

#define  SOCK_CMD_LABLE "LEYE"

//cmd frame head 通用帧头格式
typedef struct tagSOCK_CMD_FrameHead 
{
	//帧头标志"LEYE"
	char smark[4];

	unsigned int 	length;
	unsigned short 	type;
	unsigned short 	msg_id;
	unsigned int 	time_stamp;
	unsigned int	reserved;
	unsigned int	session_id;
	unsigned int 	pu_id;

}SOCK_CMD_FrameHead;

typedef struct tagSOCK_CMD_Frame 
{
	SOCK_CMD_FrameHead	head;
	char					data[4];		//crc32 value

}SOCK_CMD_Frame;


//-------------------------二进制消息类型定义------------------------------------
#define SOCK_CMDTYPE_BASE			0x1000
#define SOCK_CMDTYPE_LOGIN			(SOCK_CMDTYPE_BASE+1002)  // 设备上线，

#define SOCK_CMDTYPE_LOGOUT			(SOCK_CMDTYPE_BASE+1002)  // 设备下线


#define SOCK_CMDTYPE_CHECKDEV_RSP		(SOCK_CMDTYPE_BASE+1003)  // 设备上报的设备状态
#define SOCK_CMDTYPE_CHECKSENSOR_RSP	(SOCK_CMDTYPE_BASE+1004)  // 设备上报的传感器消息
#define SOCK_CMDTYPE_SETRELAY_RSP			(SOCK_CMDTYPE_BASE+1006)  // 设置指定设备的继电器状态

//-------------------------服务端接收的消息-------------------------------
#define SOCK_CMDTYPE_CHECKDEV_REQ		(SOCK_CMDTYPE_BASE+1101)	// 查询指定设备状态，

#define SOCK_CMDTYPE_CHECKSENSOR_REQ	(SOCK_CMDTYPE_BASE+1102)  // 查询指定设备传感器
#define SOCK_CMDTYPE_SETRELAY_REQ		(SOCK_CMDTYPE_BASE+1106)  // 设置指定设备的继电器状态
//检查设备是否在线
typedef struct  
{
	SOCK_CMD_FrameHead	head;

	int pu_id;								//0: all
	
	char					data[4];		//crc32 value
}SOCK_CMD_Frame_CHECKDEV_Req;
typedef struct  
{
	SOCK_CMD_FrameHead	head;

	int pu_id;								//0: all
	int pu_stat;							//0: offline; 1:online

	char					data[4];		//crc32 value
}SOCK_CMD_Frame_CHECKDEV_Rsp;

//检查设备传感器值
typedef struct  
{
	SOCK_CMD_FrameHead	head;

	int pu_id;								

	char					data[4];		//crc32 value
}SOCK_CMD_Frame_CHECKSENSOR_Req;
typedef struct  
{
	SOCK_CMD_FrameHead	head;

	int pu_id;								
	unsigned int DO_stat;		//max:32
	unsigned int DI_stat;		//max:32
	unsigned int PowerV;		//电池电压*100
	unsigned int Reserve1;		//
	unsigned int Reserve2;		//
	unsigned int Reserve3;		//

	char					data[4];		//crc32 value
}SOCK_CMD_Frame_CHECKSENSOR_Rsp;

//设置设备继电器
typedef struct  
{
	SOCK_CMD_FrameHead	head;
	int pu_id;
	unsigned int DO_index;		//
	unsigned int DO_stat;		//0：open（default）；1：close

	char					data[4];		//crc32 value
}SOCK_CMD_Frame_SETRELAY_Req;

typedef struct  
{
	SOCK_CMD_FrameHead	head;
	int pu_id;
	unsigned int DO_index;		//
	unsigned int DO_stat;		//0：open（default）；1：close

	char					data[4];		//crc32 value
}SOCK_CMD_Frame_SETRELAY_Rsp;

#ifdef __cplusplus
extern "C" {
#endif

	

	extern int SOCK_recv(SOCKET s, char * buf, int lentgth, int time_out);

	extern int SOCK_send(SOCKET s, char * buf, int lentgth, int time_out);

	extern int SOCK_printf(const char * format, ...);

	/*************************************************************************************
	Function name:	crc32

	Parameters:
		crc		: crc初始值，默认0
		buf		: 待校验的数据指针
		len		：待校验的数据长度

	Return Values:	校验数据的crc值

	Remarks: 

	*************************************************************************************
	*/

	unsigned int crc32_ccitt(unsigned int crc, const unsigned char *buf, unsigned int len);

	unsigned short crc16_ccitt(unsigned short crc, const unsigned char *buf, unsigned int len);

	unsigned short CRC16_modbus_CalCRC16(const unsigned char *buf, int len);
	

	/*************************************************************************************
	Function name:	GenerateCRC
	b
	Parameters:
		crc_buffer	: 待校验的数据指针
		len			：待校验的数据长度

	Return Values:	

	Remarks: 在待校验的数据后，添加上4字节大小的CRC校验值，如果校验数据长度10字节，那么crc_buffer的大小至少要大于14字节

	*************************************************************************************
	*/

	void GenerateCRC32(unsigned char * crc_buffer, int len);

	void GenerateCRC16(unsigned char * crc_buffer, int len);

	void GenerateCRC_modubus(unsigned char * crc_buffer, int len);


#ifdef __cplusplus
}
#endif

#endif