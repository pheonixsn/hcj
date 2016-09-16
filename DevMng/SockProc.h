#pragma once

#include "stdafx.h"
#include "SockCommon.h"


class SockMsgFrame
{

public:
	SOCK_CMD_Frame * pdata;
public:
	SockMsgFrame()
	{
		pdata =NULL;
	};

	SockMsgFrame(SOCK_CMD_Frame * frame)
	{
		pdata =NULL;
		pdata = (SOCK_CMD_Frame*)malloc(frame->head.length);
		assert(pdata);
		memcpy_s(pdata, frame->head.length, frame, frame->head.length);
	};

	SockMsgFrame(SockMsgFrame &frame)
	{
		if (pdata)
		{
			free(pdata);
		}
		pdata = (SOCK_CMD_Frame*)malloc(frame.pdata->head.length);
		memcpy(pdata, (char*)(&frame), frame.pdata->head.length);
	};

	~SockMsgFrame()
	{
		if (pdata)
		{
			free(pdata);
		}
	};
public:
	void operator = (const SockMsgFrame &frame)
	{
		if (pdata)
		{
			free(pdata);
		}
		pdata = (SOCK_CMD_Frame*)malloc(frame.pdata->head.length);
		memcpy(pdata, (char*)(&frame), frame.pdata->head.length);
		;

	}


};

//DWORD WINAPI SOCKLISTEN_thread_server(LPVOID lpParam )
typedef unsigned (__stdcall * FUNC_SockProcCB)(LPVOID lpParam);
typedef unsigned (__stdcall * FUNC_SockProcssMsgCB)(LPVOID lpParam1,LPVOID lpParam2);
class CSockProc
{
public:
	CSockProc(void);
	~CSockProc(void);


	void * m_device;	//设备指针
	volatile long m_beat_count;	//进程心跳


	int flg_online;
	HANDLE flg_event_exit;
	HANDLE m_lock;
	HANDLE m_lock_sock;
	SOCKET m_sock;
	HANDLE m_thread_hand;

	unsigned int m_ssid;

	int m_port;
	int m_port1;

	//消息缓冲队列
	
	CList<SockMsgFrame*,SockMsgFrame*> m_list_msg_recv;
	CList<SockMsgFrame*,SockMsgFrame*> m_list_msg_send;
	FUNC_SockProcCB m_cb_func;
	int SetProcCB(FUNC_SockProcCB cb);
	int StartProc(void);
	int StopProc(void);
	int SetPort(int port);
	int lockSock(void);
	int unlockSock(void);
	int closeSock(void);
	int lock(void);
	int unlock(void);
	int L_recv(char * buf, int lentgth, int time_out);
	int L_send(char * buf, int lentgth, int time_out);
	int AttachSock(SOCKET sock);
	int DetachSock();

	int FrameSend(SOCK_CMD_Frame* frame);
	SOCK_CMD_Frame *  FrameRecvBySSID(unsigned int ssid);
	SOCK_CMD_Frame * FrameRecv(void);
	int GetStatusOnline(void);
	int ResetCount(void);
	int ResetData(void);

};



