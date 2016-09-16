#include "StdAfx.h"
#include "SockProc.h"

#define printf SOCK_printf

static unsigned int SOCK_getLocalIP(UINT dest_ip)
{
	unsigned int ip = 0;
	char host_name[255]; 
	//获取本地主机名称 
	if (gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR) { 
		printf("Error %d when getting local host name. \r\n", WSAGetLastError()); 
		return 1; 
	} 
	printf("Host name is: %s \r\n", host_name); 

	//从主机名数据库中得到对应的“主机” 
	struct hostent *phe = gethostbyname(host_name); 
	if (phe == 0) { 
		printf("Yow! Bad host lookup.\r\n"); 
		return 1; 
	} 

	//循环得出本地机器所有IP地址 
	for (int i = 0; phe->h_addr_list[i] != 0; ++i) { 
		struct in_addr addr; 
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr)); 
		printf("Address %d : %s \r\n" , i, inet_ntoa(addr)); 

		ip = ntohl(addr.S_un.S_addr);
		if ((ip&0xffffff00) ==(dest_ip&0xffffff00))
		{
			break;
		}
		//break;
	} 

	return ip;
}

CSockProc::CSockProc(void)
{
	flg_online = 0;

	m_device = NULL;
	m_beat_count = 0;
	
	flg_event_exit = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(flg_event_exit);
	
	m_lock = CreateMutex(NULL,0, NULL);
	assert(m_lock);

	m_lock_sock = CreateMutex(NULL,0, NULL);
	assert(m_lock_sock);
	
	m_sock = INVALID_SOCKET;
	m_thread_hand = NULL;
	m_cb_func = NULL;
	m_port = 0;

	m_ssid = 0;
}


CSockProc::~CSockProc(void)
{
	StopProc();

	if (flg_event_exit)
	{
		CloseHandle(flg_event_exit);
	}

	if (m_lock)
	{
		CloseHandle(m_lock);
	}

	if (m_lock_sock)
	{
		CloseHandle(m_lock_sock);
	}

	if (m_thread_hand)
	{
		CloseHandle(m_thread_hand);
	}
	
}


int CSockProc::SetProcCB(FUNC_SockProcCB cb)
{
	lock();
	if (cb)
	{
		m_cb_func = cb;
	}
	unlock();
	return 0;
}


int CSockProc::StartProc(void)
{
	lock();
	
	if (m_thread_hand)
	{
		StopProc();
	}
	lockSock();
	//SOCK_getLocalIP(0);
	ResetEvent(flg_event_exit);
	//m_sock = INVALID_SOCKET;
	m_thread_hand = NULL;
	unlockSock();
	if (m_cb_func)
	{
		m_thread_hand = (HANDLE)_beginthreadex(NULL, 0, m_cb_func, this, 0, NULL);
		assert(m_thread_hand);
		printf("%p Service start proc at  %d\r\n", this, m_port);
	
	}
	
	unlock();

	return 0;
}


int CSockProc::StopProc(void)
{
	lock();
	if (m_thread_hand)
	{
		lockSock();
		SetEvent(flg_event_exit);
		if (m_sock != INVALID_SOCKET)
		{
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
		}
		m_sock = INVALID_SOCKET;
		unlockSock();


		WaitForSingleObject(m_thread_hand, INFINITE);
		
		lockSock();
		CloseHandle(m_thread_hand);
		m_thread_hand = NULL;
		printf("%p stop proc\r\n", this);
		unlockSock();
	}

	//停止服务后清除数据
	ResetData();

	unlock();
	return 0;
}


int CSockProc::SetPort(int port)
{
	lock();

	m_port = port;

	unlock();
	return 0;
}

int CSockProc::closeSock(void)
{
	lockSock();//在子线程中被调用

	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}

	unlockSock();
	return 0;
}

int CSockProc::lockSock(void)
{
	if (m_lock_sock)
	{
		WaitForSingleObject(m_lock_sock, INFINITE);
	}
	
	return 0;
}


int CSockProc::unlockSock(void)
{
	if (m_lock_sock)
	{
		ReleaseMutex(m_lock_sock);
	}
	
	return 0;
}

int CSockProc::lock(void)
{
	if (m_lock)
	{
		WaitForSingleObject(m_lock, INFINITE);
	}

	return 0;
}


int CSockProc::unlock(void)
{
	if (m_lock)
	{
		ReleaseMutex(m_lock);
	}

	return 0;
}


int CSockProc::L_recv(char * buf, int lentgth, int time_out)
{
	int res;
	int offset;
	int size, id;
	int over_count = 0;
	offset = 0;
	res = 0;

	if (time_out == 0)
	{
		time_out = 0x7fffffff;
	}

	lockSock();
	while (offset < lentgth)
	{

		size = recv(m_sock, (char*)(buf) + offset, lentgth-offset, 0);
		if (size > 0)
		{
			offset += size;
			res = offset;
		}
		else if(size == 0)
		{
			//close
			res = -1;
			break;
		}
		else
		{
			id =WSAGetLastError();
			if (id == WSAEWOULDBLOCK || id == WSAEINPROGRESS || id == WSAEALREADY )
			{
				Sleep(1);
				over_count++;
				if (over_count >= time_out)
				{
					break;
				}
			}
			else
			{
				res = -1;
				break;
			}
		}
	}
	unlockSock();
	return res;
}

int CSockProc::L_send(char * buf, int lentgth, int time_out)
{
	int res;
	int offset;
	int size;
	int buf_size;
	int s_len;
	int err_code;
	int over_count = 0;
	offset = 0;
	res = 0;
	buf_size = lentgth;

	if (time_out == 0)
	{
		time_out = 0x7fffffff;
	}

	lockSock();
	while (offset < buf_size)
	{
		s_len = buf_size-offset;

		//if (s_len > 2048)
		//{
		//	s_len = 2048;
		//}
		size = send(m_sock, (char*)(buf) + offset, s_len, 0);

		if (size >0)
		{
			offset += size;
			res = offset;
			Sleep(1);
		}
		else if(size == 0)
		{
			//close
			res = -1;
			break;
		}
		else
		{
			err_code = WSAGetLastError();
			if (err_code == WSAEWOULDBLOCK || err_code == WSAEINPROGRESS || err_code == WSAEALREADY)
			{
				Sleep(1);
				over_count++;
				if (over_count >= time_out)
				{
					break;
				}
			}
			else
			{
				res = -1;
				break;
			}
		}
	}
	unlockSock();

	return res;
}


int CSockProc::AttachSock(SOCKET sock)
{
	lockSock();

	if (m_sock != INVALID_SOCKET)
	{
		DetachSock();
	}
	m_sock = sock;
	flg_online = TRUE;
	unlockSock();
	return 0;
}


int CSockProc::DetachSock()
{
	lockSock();

	m_sock = INVALID_SOCKET;
	
	flg_online = FALSE;
	unlockSock();
	return 0;
}

//发送一帧命令，仅仅是放到队列中等待。
int CSockProc::FrameSend(SOCK_CMD_Frame* frame)
{
	int res = SOCK_OK;
	
	__time32_t timer;

	if (!GetStatusOnline())
	{
		return res;
	}
	_time32(&timer);
	//timer -= 8*3600; //校正时区

	lockSock();
	if (frame && frame->head.length < SOCK_MSG_MAX)
	{
		frame->head.msg_id = m_ssid;
		frame->head.session_id = m_ssid++;
		frame->head.time_stamp = timer;
		if (m_device)
		{
			//frame->head.pu_id = ((CLineyesDevice*)m_device)->m_pu_id;
		}
		int tt = sizeof(SOCK_CMD_LABLE)-1;
		memcpy_s(frame->head.smark, tt, SOCK_CMD_LABLE, tt);

		//添加校验
		GenerateCRC32((unsigned char*)frame, frame->head.length-4);

		SockMsgFrame * new_frame = new SockMsgFrame(frame);
		
		m_list_msg_send.AddTail(new_frame);
	}
	unlockSock();

	return res;
}

//返回的指针要主动释放，用free!
SOCK_CMD_Frame * CSockProc::FrameRecv(void)
{
	SockMsgFrame * new_msg = NULL;
	SOCK_CMD_Frame * res = NULL;

	if (!GetStatusOnline())
	{
		return res;
	}

	lockSock();
	if (m_list_msg_recv.GetCount()>0)
	{
		new_msg = m_list_msg_recv.RemoveHead();
	}

	unlockSock();
	
	if (new_msg)
	{
		res = (SOCK_CMD_Frame *)malloc(new_msg->pdata->head.length);
		memcpy_s(res, new_msg->pdata->head.length, new_msg->pdata, new_msg->pdata->head.length);
		delete new_msg;
	}

	return res;
}
//根据ssid查找对应的rsp
SOCK_CMD_Frame *  CSockProc::FrameRecvBySSID(unsigned int ssid)
{
	SockMsgFrame * new_msg = NULL, * tmp_msg;
	SOCK_CMD_Frame * res = NULL;

	POSITION pos;
	int i;
	
	if (!GetStatusOnline())
	{
		return res;
	}

	lockSock();

	if (m_list_msg_recv.GetCount()>0)
	{
		pos = m_list_msg_recv.GetHeadPosition();

		for (i=0; i<m_list_msg_recv.GetCount(); i++)
		{
			tmp_msg = m_list_msg_recv.GetNext(pos);
			if (tmp_msg->pdata->head.session_id == ssid)
			{
				new_msg = tmp_msg;

				m_list_msg_recv.RemoveAt(m_list_msg_recv.Find(new_msg));

				break;
			}
		}
		
	}

	unlockSock();

	if (new_msg)
	{
		res = (SOCK_CMD_Frame *)malloc(new_msg->pdata->head.length);
		memcpy_s(res, new_msg->pdata->head.length, new_msg->pdata, new_msg->pdata->head.length);
		delete new_msg;
	}

	return res;
}

int CSockProc::GetStatusOnline(void)
{
	int res;
	res = flg_online;
	return res;
}


int CSockProc::ResetCount(void)
{
	return 0;
}

//复位内置数据
int CSockProc::ResetData(void)
{
	SockMsgFrame *frame;
	m_beat_count = 0;
	lock();
	lockSock();
	//清空消息队列
	while(m_list_msg_send.GetCount() > 0)
	{
		frame = m_list_msg_send.RemoveHead();
		delete frame;
	}

	while(m_list_msg_recv.GetCount() > 0)
	{
		frame = m_list_msg_recv.RemoveHead();
		delete frame;
	}
	unlockSock();
	unlock();
	return 0;
}



