#include "StdAfx.h"

#include "SockCommon.h"
#include "SockProcListen.h"
#include "SockDevMng.h"

#define printf SOCK_printf

CSockProcListen::CSockProcListen(void)
{
	int i;
	m_cb_tmp_func = NULL;

	for (i=0; i<SOCK_WAIT_MAX; i++)
	{
		m_socket_wait[i] = INVALID_SOCKET;
		m_thread_wait[i]= NULL;
		
	}
	m_device_mng = NULL;
}


CSockProcListen::~CSockProcListen(void)
{

	int i;
	StopProc();

	for (i=0; i<SOCK_WAIT_MAX; i++)
	{
		DelTmpSocket(m_socket_wait[i]);
	}
}

//�����ʱ��sock�������ȥ����ȴ�ע��
int CSockProcListen::AddTmpSocket(SOCKET sock)
{
	int res = -1;

	//lock();

	//�ҵ�һ�����е�
	int i;
	for (i=0; i<SOCK_WAIT_MAX; i++)
	{
		if (m_socket_wait[i] == INVALID_SOCKET )
		{
			break;
		}
	}

	if (i<SOCK_WAIT_MAX)
	{
		//�п�
		HANDLE client_h = NULL;
		//��socket���nonblock
		u_long iMode = 1;

		lockSock();
		//clean thread
		if (m_thread_wait[i])
		{
			CloseHandle(m_thread_wait[i]);
			m_thread_wait[i] = NULL;
		}

		ioctlsocket(sock, FIONBIO, &iMode);

		//create thread ȥ����
		assert(m_cb_tmp_func);

		
		
		socket_wait_index_new = i;
		m_socket_wait[i] = sock;
		unlockSock();

		client_h = (HANDLE)_beginthreadex(NULL, 0, m_cb_tmp_func, this, 0, NULL);
		
		lockSock();
		
		//idle
		Sleep(500);		

		
		if(client_h)
		{
			DWORD exit_word;
			GetExitCodeThread(client_h, &exit_word);
			if (exit_word == STILL_ACTIVE)
			{
				//�����ʱ�̵߳ľ��
				m_thread_wait[i] = client_h;			
				res = 0;
				printf("Waiting processes %d created!\r\n", i);
			}

		}
		else
		{
			closesocket(m_socket_wait[i]);
			m_socket_wait[i] = INVALID_SOCKET;			
			printf("Waiting processes register failed!\r\n");
		}
		unlockSock();
	}
	else
	{
		//����
		printf("Waiting processes is full!\r\n");
	}


	//unlock();
	return res;
}


//��ֹ�̹߳ر�sock
int CSockProcListen::DelTmpSocket(SOCKET sock)
{
	lock();

	//�ҵ�һ��
	int i;

	//clean thread
	for (i=0; i<SOCK_WAIT_MAX; i++)
	{
		if (m_socket_wait[i] == INVALID_SOCKET && m_thread_wait[i])
		{
			CloseHandle(m_thread_wait[i]);
			m_thread_wait[i] = NULL;
		}
	}
	for (i=0; i<SOCK_WAIT_MAX; i++)
	{
		if (m_socket_wait[i] == sock )
		{
			break;
		}
	}

	if (i<SOCK_WAIT_MAX)
	{
		lockSock();
		if (m_socket_wait[i] != INVALID_SOCKET)
		{
			closesocket(m_socket_wait[i]);
			m_socket_wait[i] = INVALID_SOCKET;
		}
		unlockSock();

		if (m_thread_wait[i])
		{
			WaitForSingleObject(m_thread_wait[i], INFINITE);
			CloseHandle(m_thread_wait[i]);
			m_thread_wait[i] = NULL;
		}
		printf("Waiting processes del %d\r\n", i);
	}

	unlock();
	return 0;
}

//�ر�sock�����ǲ��ȴ��߳̽���
int CSockProcListen::CloseTmpSocket(SOCKET sock)
{
	//lock();//�����߳��б�����

	//�ҵ�һ�����е�
	int i;
	lockSock();
	for (i=0; i<SOCK_WAIT_MAX; i++)
	{
		if (m_socket_wait[i] == sock )
		{
			break;
		}
	}

	if (i<SOCK_WAIT_MAX)
	{

		if (m_socket_wait[i] != INVALID_SOCKET)
		{
			closesocket(m_socket_wait[i]);
			m_socket_wait[i] = INVALID_SOCKET;
			printf("Waiting processes close %d\r\n", i);
		}
		
	}
	unlockSock();
	//unlock();
	return 0;
}
//��sock �Ƴ������������̴߳����������ر�
int CSockProcListen::RemoveTmpSocket(SOCKET sock)
{
	//lock();//�����߳��б�����

	//�ҵ�һ�����е�
	int i;
	lockSock();
	for (i=0; i<SOCK_WAIT_MAX; i++)
	{
		if (m_socket_wait[i] == sock )
		{
			break;
		}
	}

	if (i<SOCK_WAIT_MAX)
	{

		if (m_socket_wait[i] != INVALID_SOCKET)
		{
			m_socket_wait[i] = INVALID_SOCKET;
		}

	}
	unlockSock();
	//unlock();
	return 0;
}


int CSockProcListen::SetProcTmpCB(FUNC_SockProcCB cb)
{
	lock();
	if (cb)
	{
		m_cb_tmp_func = cb;
	}
	unlock();
	return 0;
}


//�����������̺߳���
unsigned WINAPI SOCKLISTEN_thread_server(LPVOID lpParam )
{
	int res = 0;
	CSockProcListen* handle;

	SOCKET client_s = INVALID_SOCKET;
	HANDLE client_h = NULL;
	struct sockaddr_in client_addr, service_addr;
	int size;
	u_long iMode;

	handle = (CSockProcListen*)lpParam;
		
	handle->lockSock();

	// Create a SOCKET for the server to listen for client connections
	handle->m_sock	= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	assert (handle->m_sock != INVALID_SOCKET);
		
	//���ó�non-blocking
	iMode = 1;
	ioctlsocket(handle->m_sock, FIONBIO, &iMode);


	service_addr.sin_family = AF_INET;
	service_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	service_addr.sin_port = htons(handle->m_port);
	res = bind( handle->m_sock, 		(SOCKADDR*) &service_addr, 		sizeof(service_addr));
	handle->unlockSock();

	if (res == SOCKET_ERROR)
	{
		handle->closeSock();
		printf("Listen Port is used!\r\n");
		res = 1;
		goto END0;
	}

	res = listen( handle->m_sock, SOMAXCONN );
	//assert(res != SOCKET_ERROR); //
	if (res == SOCKET_ERROR)
	{
		handle->closeSock();
		printf("Listen failed!\r\n");
		res = 2;
		goto END0;
	}


	// Accept a client socket
	while (WaitForSingleObject(handle->flg_event_exit, 0) != WAIT_OBJECT_0)
	{
		fd_set         read;
		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		FD_ZERO(&read);
		FD_SET(handle->m_sock, &read);

		res=select(0,&read,NULL,NULL,&tv);
		if (res <0)
		{
			//����			
			int err = WSAGetLastError();
			printf("select error %d!\r\n", err);

			handle->closeSock();
			res = 3;
			goto END0;
		}
		else if (res == 0)
		{
			//��ʱ
			continue;
		}


		size = sizeof(client_addr);
		client_s = accept(handle->m_sock,(struct sockaddr*) &client_addr, &size);
		if (client_s == INVALID_SOCKET)
		{
			int err = WSAGetLastError();
			printf("accept error %d!\r\n", err);
			
			handle->closeSock();
			res = 4;
			goto END0;               
		}

		//��ӵ�һ����ʱ��socket������
		unsigned int ip;
		ip = ntohl(client_addr.sin_addr.s_addr);
		printf("accept from %d.%d.%d.%d:%d\r\n", (ip&0xFF000000)>>24, (ip&0xFF0000)>>16, (ip&0xFF00)>>8, ip&0xFF, ntohs(client_addr.sin_port));
		res = handle->AddTmpSocket(client_s);

		if (res == 0)
		{
			//ok;

		}
		else
		{
			//failed,ͨ��������������
			Sleep(1000);
			closesocket(client_s);
		}

	}

	handle->closeSock();
END0:
	
	return res;
}

//����ÿһ���豸����
unsigned WINAPI SOCKLISTEN_thread_client(LPVOID  lpParam)
{
	int res = 0;

	CSockDevMng * mng_dev;
	CSockProcListen* handle;
	SOCKET s_client;
	int time_timeout = 0;
	int index;
	char buf_recv[1024];
	int buf_size = 1024;
	char buf_send[128];

	MODBUS_CMD_RegReq * pFrame;

	fd_set         read;
	struct timeval tv;

	handle = (CSockProcListen *)lpParam;
	index = handle->socket_wait_index_new;
	s_client = handle->m_socket_wait[index];
	//memset(&msg, 0, sizeof(SLD_Msg));

	mng_dev = (CSockDevMng*)handle->m_device_mng;
	assert(mng_dev);

	while(1)
	{	
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		handle->lockSock();
		s_client = handle->m_socket_wait[index];
		handle->unlockSock();

		if (s_client == INVALID_SOCKET)
		{
			res = 1;
			goto END0;
		}

		FD_ZERO(&read);
		FD_SET(s_client, &read);

		res=select(0,&read,NULL,NULL,&tv);
		if (res <0)
		{
			//����//�����socket���ⲿ���رգ�return
			int err = WSAGetLastError();
			printf("client thread error %d\r\n", err);
			res = 1;
			goto END0;
		}
		else if (res == 0)
		{
			//��ʱ
			time_timeout++;
			//����ʱ10��ʱ���ߵ�������
			if (time_timeout >= 15)
			{
				printf("client thread timeout \r\n");
				res = 2;
				goto END0;
			}
			else
			{
				continue;
			}
		} 
		else
		{
			time_timeout = 0;

			buf_size = sizeof(buf_recv);
			memset(buf_recv, 0, buf_size);
			
			pFrame = (MODBUS_CMD_RegReq * )buf_recv;
			buf_size = SOCK_recv(s_client,buf_recv,sizeof(MODBUS_CMD_RegReq), 5000);//5s �ڱ�����������
			if (buf_size <= 0)
			{
				res = 3;
				//�������ݴ�
				goto END0;
			}
			if (buf_size != sizeof(MODBUS_CMD_RegReq))
			{
				res = 4;
				//�������ݴ�
				goto END0;
			}
			if (memcmp(pFrame->smark, MODBUS_CMD_LABLE, 4))
			{
				res = 5;
				//Э��ͷ��
				goto END0;
			}

			if (CRC16_modbus_CalCRC16((const unsigned char *)buf_recv, sizeof(MODBUS_CMD_RegReq)))
			{
				res = 8;
				//У���
				//----ע�᲻У��
				//goto END0;
			}

			//ע��
			res = mng_dev->DevLogin(s_client, pFrame->pu_id, pFrame, 0);
			if (res != SOCK_OK)
			{
				//failed!
				res = 10;
			}
			else
			{
				//OK
				res = 0;
			}
			if (res == SOCK_OK)
			{
				handle->RemoveTmpSocket(s_client);;//�˴�socket�ѽ���mng���̹߳���
			}
			break;
		}

	}

	//�˳�ʱ�ر���ʱ����
	
END0:
	handle->CloseTmpSocket(s_client);

	printf("Waiting processes exit at %d\r\n", res);

	return res;

}



//overload
int CSockProcListen::StopProc(void)
{

	CSockProc::StopProc();
	lock();

	int i;
	for (i=0; i<SOCK_WAIT_MAX; i++)
	{
		if (m_socket_wait[i] != INVALID_SOCKET )
		{
			DelTmpSocket(m_socket_wait[i]);
		}
	}
	
	unlock();


	return 0;
}



