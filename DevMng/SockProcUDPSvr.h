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
	
	void * m_device_mng;	//�豸������ָ��

	//�ȵ�ע�����ʱ�߳�
	int socket_wait_index_new;
	SOCKET m_socket_wait;
	HANDLE m_thread_wait;//�ȴ����ӵ��߳̾��
	

	int StopProc(void);

	int NotifyLogin(int pu_id);	//֪ͨ�豸���߻�����

};

