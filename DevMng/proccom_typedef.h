
//定义进程间通讯消息
#ifndef _PROCCOM_TYPEDEF_H_

#define _PROCCOM_TYPEDEF_H_



//-------------------------服务端主动发送的消息------------------------------------
#define WM_MESSAGE_LOGIN			(WM_USER+1001)  // 设备上线，
#define WM_MESSAGE_LOGOUT			(WM_USER+1002)  // 设备下线

#define WM_MESSAGE_CHECKDEV_RSP		(WM_USER+1003)  // 设备上报的设备状态
#define WM_MESSAGE_CHECKSENSOR_RSP	(WM_USER+1004)  // 设备上报的传感器消息

//-------------------------服务端接收的消息-------------------------------
#define WM_MESSAGE_CHECKDEV_REQ		(WM_USER+1101)	// 查询指定设备状态，
#define WM_MESSAGE_CHECKSENSOR_REQ	(WM_USER+1102)  // 查询指定设备传感器
#define WM_MESSAGE_SETRELAY_REQ		(WM_USER+1106)  // 设置指定设备的继电器状态






#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
