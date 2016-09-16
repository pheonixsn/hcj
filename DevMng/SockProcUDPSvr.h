#pragma once
#include "stdafx.h"
#include "sockproc.h"
#include "modbus_typedef.h"

extern unsigned WINAPI SOCKUDP_thread_server(LPVOID lpParam );

class CSockProcUDPSvr :
	public CSockProc
{
public:
	CSockProcUDPSvr(void);
	~CSockProcUDPSvr(void);
	
	void * m_device_mng;	//设备管理器指针

	//等到注册的临时线程
	int socket_wait_index_new;
	SOCKET m_socket_wait;
	HANDLE m_thread_wait;//等待连接的线程句柄
	

	int StopProc(void);

	int NotifyLogin(int pu_id);	//通知设备上线或下线

};

