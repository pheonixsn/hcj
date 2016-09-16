#include "StdAfx.h"

#include "SockCommon.h"
#include "SockProcUDPSvr.h"
#include "SockDevMng.h"

#define printf SOCK_printf

CSockProcUDPSvr::CSockProcUDPSvr(void)
{

		m_socket_wait = INVALID_SOCKET;
		m_thread_wait = NULL;

	m_device_mng = NULL;
}


CSockProcUDPSvr::~CSockProcUDPSvr(void)
{


	StopProc();

}

#define BUF_RECV_MAX 1024
#define BUF_SEND_MAX 1024
//监听服务主线程函数
unsigned WINAPI SOCKUDP_thread_server(LPVOID lpParam )
{
	int res = 0;
	CSockProcUDPSvr* handle;
	CSockDevMng * dev_mng;

	SOCKET client_s = INVALID_SOCKET;
	HANDLE client_h = NULL;
	struct sockaddr_in client_addr, service_addr;
	fd_set         read, err;
	struct timeval tv;
	int size;
	u_long iMode;
	char buf_recv[BUF_RECV_MAX];
	int buf_size = BUF_RECV_MAX;
	char buf_send[BUF_RECV_MAX];


	SOCK_CMD_Frame * pFrame, *pFrameRsp;

	handle = (CSockProcUDPSvr*)lpParam;
	dev_mng = (CSockDevMng*)handle->m_device_mng;

	pFrame = (SOCK_CMD_Frame *)buf_recv;
	pFrameRsp = (SOCK_CMD_Frame *)buf_send;
		
	handle->lockSock();


	// Create a SOCKET for the server 
	handle->m_sock	= socket(AF_INET,SOCK_DGRAM,0);
	assert (handle->m_sock != INVALID_SOCKET);		

	ioctlsocket(handle->m_sock, FIONBIO, &iMode);

	service_addr.sin_family = AF_INET;
	service_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	service_addr.sin_port = htons(handle->m_port);

	res = bind( handle->m_sock, 		(SOCKADDR*) &service_addr, 		sizeof(service_addr));

	handle->unlockSock();

	if (res == SOCKET_ERROR)
	{
		handle->closeSock();
		printf("Bind Failed at port %d\r\n", handle->m_port);
		res = 1;
		goto END0;
	}

	handle->flg_online = 1;
	// wait for udp data
	while (WaitForSingleObject(handle->flg_event_exit, 0) != WAIT_OBJECT_0)
	{

		tv.tv_sec = 0;
		tv.tv_usec = 100000;

		handle->lockSock();
		client_s = handle->m_sock;
		handle->unlockSock();

		FD_ZERO(&read);
		FD_SET(client_s, &read);
		//FD_ZERO(&write);
		//FD_SET(s_client, &write);
		FD_ZERO(&err);
		FD_SET(client_s, &err);

		res=select(0,&read,0,&err,&tv);
		if (res <0)		
		{
			//出错			
			int err = WSAGetLastError();
			printf("select error %d!\r\n", err);

			handle->closeSock();
			res = 3;
			goto END0;
		}
		else if (res == 0)
		{
			//超时
			//if (FD_ISSET(client_s, &write))
			{
				SockMsgFrame * new_msg = NULL;
				SOCK_CMD_Frame * send_frame = NULL;

				//printf("check msg\r\n");
				handle->lockSock();
				if (handle->m_list_msg_send.GetCount() > 0)
				{
					new_msg = handle->m_list_msg_send.RemoveHead();
				}				
				handle->unlockSock();

				while (new_msg)
				{
					send_frame = new_msg->pdata;

					printf("send msg:%d\r\n", send_frame->head.type);
					sendto(client_s,(char*)send_frame,send_frame->head.length,0,(sockaddr*)&client_addr,sizeof(client_addr));
					Sleep(1);
					delete new_msg;

					handle->lockSock();
					new_msg = NULL;
					if (handle->m_list_msg_send.GetCount() > 0)
					{
						new_msg = handle->m_list_msg_send.RemoveHead();
					}
					handle->unlockSock();
				}
			}
		}
		else
		{
			if (FD_ISSET(client_s, &err))
			{
				//failed？？
				printf("UDP thread select exception\r\n");
				res = 3;
				goto END0;
			}

			if (FD_ISSET(client_s, &read))
			{
				buf_size = BUF_RECV_MAX;
				memset(buf_recv, 0, buf_size);

				handle->lockSock();//保证帧头和帧数据要一次性收完，不能被打断

				size = sizeof(client_addr);
				buf_size = recvfrom(client_s,buf_recv,BUF_RECV_MAX,0,(sockaddr*)&client_addr,&size);//用新建的地址结构保存客户端的信息 
				
				printf("UDP recv %s",inet_ntoa(client_addr.sin_addr));
				if (buf_size <= 0)
				{
					
					handle->unlockSock();
					res = 4;
					//接收数据错
					goto END0;
					
				}
				if (memcmp(pFrame->head.smark, SOCK_CMD_LABLE, 4))
				{
					handle->unlockSock();
					res = 6;
					//协议头错
					break;
				}
				if (pFrame->head.length > BUF_RECV_MAX)
				{
					handle->unlockSock();
					res = 7;
					//消息过大
					break;
				}

				if (crc32_ccitt(0, (const unsigned char *)buf_recv, pFrame->head.length))
				{
					res = 9;
					//校验错
					break;
				}

				printf("UDP recv msg:%d\r\n", pFrame->head.type);

				//根据消息类型，处理回复
				switch( pFrame->head.type)
				{
				case SOCK_CMDTYPE_CHECKDEV_REQ:
					{
						SOCK_CMD_Frame_CHECKDEV_Req * frame_req = (SOCK_CMD_Frame_CHECKDEV_Req*)pFrame;
						SOCK_CMD_Frame_CHECKDEV_Rsp * frame_rsp = (SOCK_CMD_Frame_CHECKDEV_Rsp*)pFrameRsp;
						CSockDevice *dev;

						int pu_id;
						int pu_stat;

						if (frame_req->pu_id == 0)
						{
							//check all dev

							for (int i=0; i<dev_mng->GetCount();  i++)
							{
								pu_id = dev_mng->m_list_device[i]->m_pu_id;
								pu_stat = dev_mng->m_list_device[i]->GetStatus();

								memcpy(&frame_rsp->head, &frame_req->head, sizeof(frame_req->head));
								frame_rsp->head.type = SOCK_CMDTYPE_CHECKDEV_RSP;
								frame_rsp->head.length = sizeof(SOCK_CMD_Frame_CHECKDEV_Rsp);

								frame_rsp->pu_id = pu_id;
								frame_rsp->pu_stat = pu_stat;
								handle->FrameSend(pFrameRsp);
							};

						}
						else
						{
							dev = (CSockDevice *)dev_mng->GetDevByID(frame_req->pu_id);
							if (dev)
							{
								pu_id = frame_req->pu_id;
								pu_stat = dev->GetStatus();

								memcpy(&frame_rsp->head, &frame_req->head, sizeof(frame_req->head));
								frame_rsp->head.type = SOCK_CMDTYPE_CHECKDEV_RSP;
								frame_rsp->head.length = sizeof(SOCK_CMD_Frame_CHECKDEV_Rsp);

								frame_rsp->pu_id = pu_id;
								frame_rsp->pu_stat = pu_stat;
								handle->FrameSend(pFrameRsp);
							}

						}
						
					}
					break;

				case SOCK_CMDTYPE_CHECKSENSOR_REQ:
					{
						SOCK_CMD_Frame_CHECKSENSOR_Req * frame_req = (SOCK_CMD_Frame_CHECKSENSOR_Req*)pFrame;
						SOCK_CMD_Frame_CHECKSENSOR_Rsp * frame_rsp = (SOCK_CMD_Frame_CHECKSENSOR_Rsp*)pFrameRsp;
						CSockDevice *dev;

						int pu_id;
						int pu_stat;

						if (frame_req->pu_id == 0)
						{
							//check all dev
							for (int i=0; i<dev_mng->GetCount();  i++)
							{
								
								dev = dev_mng->m_list_device[i];
								pu_id = dev->m_pu_id;

								memcpy(&frame_rsp->head, &frame_req->head, sizeof(frame_req->head));
								frame_rsp->head.type = SOCK_CMDTYPE_CHECKSENSOR_RSP;
								frame_rsp->head.length = sizeof(SOCK_CMD_Frame_CHECKSENSOR_Rsp);

								frame_rsp->DO_stat = dev->m_DO[0] + (dev->m_DO[1]<<1)+ (dev->m_DO[2]<<2)+ (dev->m_DO[3]<<3)+ (dev->m_DO[4]<<4)+ (dev->m_DO[5]<<5);
								frame_rsp->DI_stat = dev->m_DI[0] + (dev->m_DI[1]<<1)+ (dev->m_DI[2]<<2)+ (dev->m_DI[3]<<3)+ (dev->m_DI[4]<<4)+ (dev->m_DI[5]<<5);
								frame_rsp->PowerV = dev->m_bat_v;

								handle->FrameSend(pFrameRsp);
							};

						}
						else
						{
							dev = (CSockDevice *)dev_mng->GetDevByID(frame_req->pu_id);
							if (dev)
							{
								pu_id = frame_req->pu_id;
								pu_stat = dev->GetStatus();

								memcpy(&frame_rsp->head, &frame_req->head, sizeof(frame_req->head));
								frame_rsp->head.type = SOCK_CMDTYPE_CHECKSENSOR_RSP;
								frame_rsp->head.length = sizeof(SOCK_CMD_Frame_CHECKSENSOR_Rsp);

								frame_rsp->DO_stat = dev->m_DO[0] + (dev->m_DO[1]<<1)+ (dev->m_DO[2]<<2)+ (dev->m_DO[3]<<3)+ (dev->m_DO[4]<<4)+ (dev->m_DO[5]<<5);
								frame_rsp->DI_stat = dev->m_DI[0] + (dev->m_DI[1]<<1)+ (dev->m_DI[2]<<2)+ (dev->m_DI[3]<<3)+ (dev->m_DI[4]<<4)+ (dev->m_DI[5]<<5);
								frame_rsp->PowerV = dev->m_bat_v;

								handle->FrameSend(pFrameRsp);
							}

						}

					}
					break;

				case SOCK_CMDTYPE_SETRELAY_REQ:
					{
						SOCK_CMD_Frame_SETRELAY_Req * frame_req = (SOCK_CMD_Frame_SETRELAY_Req*)pFrame;
						SOCK_CMD_Frame_SETRELAY_Rsp * frame_rsp = (SOCK_CMD_Frame_SETRELAY_Rsp*)pFrameRsp;
						CSockDevice *dev;
						
						{
							dev = (CSockDevice *)dev_mng->GetDevByID(frame_req->pu_id);
							if (dev)
							{
								dev->SetDO(frame_req->DO_index, frame_req->DO_stat);

								memcpy(&frame_rsp->head, &frame_req->head, sizeof(frame_req->head));
								frame_rsp->head.type = SOCK_CMDTYPE_SETRELAY_RSP;
								frame_rsp->head.length = sizeof(SOCK_CMD_Frame_SETRELAY_Rsp);

								frame_rsp->DO_index = frame_req->DO_index;
								frame_rsp->DO_stat = frame_req->DO_stat;
								frame_rsp->pu_id = frame_req->pu_id;

								handle->FrameSend(pFrameRsp);
							}

						}

						::PostMessageA((HWND)dev_mng->m_parent_handle, WM_MESSAGE_REFRESH, 1, NULL);

					}
					break;

				default:
					break;
					
				}

				handle->unlockSock();
			}

			
		}

		
	}
	handle->flg_online = 0;

	handle->closeSock();
END0:
	
	return res;
}




//overload
int CSockProcUDPSvr::StopProc(void)
{

	CSockProc::StopProc();
	lock();

	//
	
	unlock();


	return 0;
}



int CSockProcUDPSvr::NotifyLogin(int pu_id)
{


	int buf_size = BUF_RECV_MAX;
	char buf_send[BUF_RECV_MAX];
	int pu_stat;

	SOCK_CMD_Frame_CHECKDEV_Rsp * frame_rsp = (SOCK_CMD_Frame_CHECKDEV_Rsp*)buf_send;
	CSockDevice *dev;


	dev = (CSockDevice *)((CSockDevMng*)m_device_mng)->GetDevByID(pu_id);
	if (dev)
	{
		pu_id = pu_id;
		pu_stat = dev->GetStatus();


		frame_rsp->head.type = SOCK_CMDTYPE_CHECKDEV_RSP;
		frame_rsp->head.length = sizeof(SOCK_CMD_Frame_CHECKDEV_Rsp);

		frame_rsp->pu_id = pu_id;
		frame_rsp->pu_stat = pu_stat;
		FrameSend((SOCK_CMD_Frame*)frame_rsp);
	}

	return 0;
}
