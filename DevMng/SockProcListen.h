#pragma once
#include "stdafx.h"
#include "sockproc.h"
#include "modbus_typedef.h"


//���ȴ�������
#define SOCK_WAIT_MAX	4

extern unsigned WINAPI SOCKLISTEN_thread_client(LPVOID  lpParam);
extern unsigned WINAPI SOCKLISTEN_thread_server(LPVOID lpParam );
class CSockProcListen :
	public CSockProc
{
public:
	CSockProcListen(void);
	~CSockProcListen(void);
	
	void * m_device_mng;	//�豸������ָ��

	//�ȵ�ע�����ʱ�߳�
	int socket_wait_index_new;
	SOCKET m_socket_wait[SOCK_WAIT_MAX];
	HANDLE m_thread_wait[SOCK_WAIT_MAX];//�ȴ����ӵ��߳̾��
	
	//����ע��ĺ���
	FUNC_SockProcCB m_cb_tmp_func;

	int AddTmpSocket(SOCKET sock);
	int DelTmpSocket(SOCKET sock);
	int SetProcTmpCB(FUNC_SockProcCB cb);
	int CloseTmpSocket(SOCKET sock);
	int StopProc(void);
	int RemoveTmpSocket(SOCKET sock);
};

