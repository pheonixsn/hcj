

#ifndef _MODBUS_TYPEDEF_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _MODBUS_TYPEDEF_H_

#define MODBUS_CMD_LABLE "MDBS"

/**********************************定义系统返回值及错误类型*****************************************/
//成功
#define MODBUS_OK                         0
//致命错误
#define MODBUS_ERROR                    100            
//当前用户无权限
#define MODBUS_ERROR_NOPERMIT    1
//用户无效（已掉线等情况）
#define MODBUS_ERROR_USER_NOLOGIN                 2  
/***********************注册服务器错误***********************/
//ID无效
#define MODBUS_ERROR_SERVER_IDENTIFY      11
//不支持设备（服务器端）
#define MODBUS_ERROR_SERVER_DEVICE        12
/******************************客户端登录************************************/
//用户不存在
#define MODBUS_ERROR_USER_NO                   13
//密码验证失败
#define MODBUS_ERROR_USER_PWD                        14
//用户已登录
#define MODBUS_ERROR_USER_LOGINED               15
//登录用户数已满
#define MODBUS_ERROR_USER_LOGINFULL                 16  
/******************************用户管理*************************************/
//用户已满
#define MODBUS_ERROR_USER_FULL                 21
//用户登录中
#define MODBUS_ERROR_USER_ONLINE             22
/*******************************布撤防************************************/
//不在有效的预置位上
#define MODBUS_ERROR_ARM_POSINVALID                31
/*******************************云镜控制************************************/
//布防中，不能动作
#define MODBUS_ERROR_ARM_ARMED                32
//不支持命令
#define MODBUS_ERROR_PTZ_NOCMD                 33
/*******************************设备升级************************************/
//超时
#define MODBUS_ERROR_UPDATE_OVERTIME         41
//升级文件已被损坏(CRC32校验错误)
#define MODBUS_ERROR_UPDATE_IDENTIFY         42
//升级文件版本错误
#define MODBUS_ERROR_UPDATE_VERSION          43
//升级文件适用的型号与目标设备的型号不匹配
#define MODBUS_ERROR_UPDATE_MATCH            44
//升级文件的格式不正确
#define MODBUS_ERROR_UPDATE_INVALID_FORMAT   45

/*******************************通道连接请求（媒体、二进制）************************************/
//不支持协议
#define MODBUS_ERROR_NET_PROT                    51
//不支持的媒体类型
#define MODBUS_ERROR_NET_MEDIA                   52
//目的地址不可达(TCP)
#define MODBUS_ERROR_NET_DEST                     53
/*******************************前端文件管理（音频文件，云镜协议文件等）************************************/
//文件使用中
#define MODBUS_ERROR_FILE_USING                    61
//文件不存在
#define MODBUS_ERROR_FILE_NO                       62
//文件存储满
#define MODBUS_ERROR_FILE_FULL                     63
//其他错误
#define MODBUS_ERROR_OTHER      100 

//------------注册帧--------------
typedef struct 
{
	char smark[4];
	unsigned int 	pu_id;
	unsigned int 	pu_key;
	unsigned char	crc16_hi;
	unsigned char	crc16_low;
}MODBUS_CMD_RegReq;

//------------心跳帧--------------
typedef struct 
{
	unsigned char 	dev_id;
	unsigned char 	func_code;		//100 == beat
	unsigned char	crc16_hi;
	unsigned char	crc16_low;
}MODBUS_CMD_BeatReq;

//cmd frame head
typedef struct tagMODBUS_CMD_FrameHead 
{
	unsigned char 	dev_id;
	unsigned char 	func_code;
	unsigned char 	size;
	unsigned char 	data;
}MODBUS_CMD_FrameHead;

//--------------------04 cmd，读取模拟量值---------------------
typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	reg_add_hi;
	unsigned char	reg_add_lo;
	unsigned char	reg_num_hi;
	unsigned char	reg_num_lo;
	unsigned char	crc16_hi;
	unsigned char	crc16_low;

}MODBUS_CMD04_FrameReq;

typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	size;		//不含crc
	unsigned char	data[3];	//含了crc16
}MODBUS_CMD04_FrameRsp;

//--------------------02 cmd，读取数字量值---------------------
typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	reg_add_hi;
	unsigned char	reg_add_lo;
	unsigned char	reg_num_hi;
	unsigned char	reg_num_lo;
	unsigned char	crc16_hi;
	unsigned char	crc16_low;

}MODBUS_CMD02_FrameReq;

typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	size;		//不含crc
	unsigned char	data[3];	//含了crc16
}MODBUS_CMD02_FrameRsp;


//--------------------01 cmd，读取线圈值---------------------
typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	reg_add_hi;
	unsigned char	reg_add_lo;
	unsigned char	reg_num_hi;
	unsigned char	reg_num_lo;
	unsigned char	crc16_hi;
	unsigned char	crc16_low;

}MODBUS_CMD01_FrameReq;

typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	size;		//不含crc
	unsigned char	data[3];	//含了crc16
}MODBUS_CMD01_FrameRsp;

//--------------------05 cmd，设置线圈值---------------------
typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	reg_add_hi;
	unsigned char	reg_add_lo;
	unsigned char	reg_val_hi;
	unsigned char	reg_val_lo;
	unsigned char	crc16_hi;
	unsigned char	crc16_low;

}MODBUS_CMD05_FrameReq;

typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	reg_add_hi;
	unsigned char	reg_add_lo;
	unsigned char	reg_val_hi;
	unsigned char	reg_val_lo;
	unsigned char	crc16_hi;
	unsigned char	crc16_low;
}MODBUS_CMD05_FrameRsp;


//--------------------03 cmd，读寄存器，太阳能控制器---------------------
typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	reg_add_hi;
	unsigned char	reg_add_lo;
	unsigned char	reg_num_hi;
	unsigned char	reg_num_lo;
	unsigned char	crc16_hi;
	unsigned char	crc16_low;

}MODBUS_CMD03_FrameReq;


typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	size;		//不含crc
	unsigned char	data[3];	//含了crc16
}MODBUS_CMD03_FrameRsp;


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
