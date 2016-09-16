#include "StdAfx.h"
#include "SockProcMC.h"
#include "SockDevice.h"

#define printf SOCK_printf

CSockDevice::CSockDevice(void)
{	

	m_lock = CreateMutex(NULL,0, NULL);
	assert(m_lock);

	m_pu_id = 0;
	
	m_pu_ver = 0;

	//memset(m_pu_key, 0, sizeof(m_pu_key));
	//memset(m_pu_name, 0, sizeof(m_pu_name));
	//memset(m_pu_model, 0, sizeof(m_pu_model));

	memset(m_DI, 0, sizeof(m_DI));
	memset(m_DO, 0, sizeof(m_DO));
	memset(m_Do_count,0,sizeof(m_Do_count));

	memset(&m_cfg, 0, sizeof(m_cfg));
	for (int i=0; i<NUM_DO; i++)
	{
		m_cfg.m_DO_delay[i] = 5*60; //Ĭ��5���ӹر�
	}
	
	for (int i=0; i<sizeof(m_AI)/sizeof(m_AI[0]); i++)
	{
		m_AI[i]= 0;
	}
	m_bat_v = 0;
	m_load_I = 0;
	res_num = 0;
	
	m_postmsg_handle = NULL;
	

	flg_online = FALSE;

	m_sock_modbus.m_device = this;

	//printf("CSockDevice new\r\n");
}


CSockDevice::~CSockDevice(void)
{
	//printf("CSockDevice del\r\n");
	if (m_lock)
	{
		CloseHandle(m_lock);
	}
}

void CSockDevice::Lock(void)
{
	if (m_lock)
	{
		WaitForSingleObject(m_lock, INFINITE);
	}

}

void CSockDevice::Unlock(void)
{
	if (m_lock)
	{
		ReleaseMutex(m_lock);
	}

}


int CSockDevice::GetStatus(void)
{
	int res = 0;
	Lock();
	res = m_sock_modbus.GetStatusOnline();
	Unlock();
	return res;
}


int CSockDevice::StartProc(void)
{
	Lock();
	m_sock_modbus.StartProc();
	Unlock();
	return 0;
}


int CSockDevice::StopProc(void)
{
	
	Lock();
	m_sock_modbus.StopProc();
	Unlock();

	//����ֹͣ���������������
	ResetData();

	return 0;
}

//��ʼ��ssid��msgid
int CSockDevice::ResetCount(void)
{
	//��ʼ��ssid��ֻ��server����ʱ��һ��
	if (!m_sock_modbus.GetStatusOnline())
	{
		//�����豸puid��ʼssid,1��ʼ��14bit puid+2bit��ʶ��mc��01��LD��10��+16bit ���
		m_sock_modbus.m_ssid = ((m_pu_id&0x3FFF)<<18 ) | (0x01)<<16 | 0x01;
	}


	return 0;
}


int CSockDevice::RefreshData(void)
{
	int i;
	int res = SOCK_OK;
	
	Lock();

	GetDI();
	GetAI();
	GetDO();
	GetPowerV();
	GetLoadI();
	GetTemp();


	Unlock();

	return 0;
}

int CSockDevice::SetDO(int index, int stat)
{
	int i;
	unsigned int ssid;
	int res = SOCK_OK;

	MODBUS_CMD05_FrameReq frame_req;
	MODBUS_CMD05_FrameRsp *frame_rsp;

	if (!GetStatus())
	{
		return res;
	}

	Lock();

	memset(&frame_req, 0, sizeof(frame_req));

	frame_req.dev_id = 1;
	frame_req.func_code = 0x05;
	frame_req.reg_add_hi = 0;
	frame_req.reg_add_lo = index;
	if (stat)
	{
		frame_req.reg_val_hi = 0xff;
	}
	else
	{
		frame_req.reg_val_lo = 0;
	}
	frame_req.reg_val_lo = 0;
	
	GenerateCRC_modubus((unsigned char*)(&frame_req), sizeof(frame_req)-2);

	res = m_sock_modbus.L_send((char*)(&frame_req), sizeof(frame_req), 5);
	if (res  < 0)
	{
		Unlock();
		return res;
	}

	//
	if (stat)
	{
		__time32_t tt;
		_time32(&tt);
		m_Do_count[index] = tt;
	}
	else
	{
		m_Do_count[index] = 0;
	}
	
	//����Ӧ����Ϣ
	ssid = (frame_req.dev_id<<8) + frame_req.func_code;
	if (SOCK_MSG_TIMEOUT)
	{
		//��������rsp
		i = 0;
		while(i++<SOCK_MSG_TIMEOUT)
		{
			frame_rsp = (MODBUS_CMD05_FrameRsp*)m_sock_modbus.FrameRecvBySSID(ssid);
			if(frame_rsp)
			{
				break;
			}
			Sleep(1);
		}
	}

	if (frame_rsp)
	{
		m_DO[index] = (unsigned short)stat;

		delete frame_rsp;
	}
	else
	{
		res = SOCK_MSG_TIMEOUT;
		printf("%s ssid:%X timeover\r\n", __FUNCTION__, ssid);
	}
	
	Unlock();

	return 0;
}
//�ж��Ƿ�Ҫ�Զ��ر�DO���رշ���1�����رշ���0
int CSockDevice::SetDOAuto(int index, unsigned int time_now)
{
	int i;
	int res = 0;

	if (!GetStatus())
	{
		return res;
	}
	Lock();

	if (m_cfg.m_DO_delay[index]>0 
		&&m_Do_count[index] >0 
		&& time_now > m_Do_count[index] 
		&& (time_now - m_Do_count[index] > m_cfg.m_DO_delay[index]))
	{
		SetDO(index, 0);
		res = 1;
	}

	Unlock();
	return res;
}

int CSockDevice::GetDO( )
{
	int i;
	unsigned int ssid;
	int res = SOCK_OK;

	MODBUS_CMD01_FrameReq frame_req;
	MODBUS_CMD01_FrameRsp *frame_rsp;

	if (!GetStatus())
	{
		return res;
	}

	Lock();

	memset(&frame_req, 0, sizeof(frame_req));
	frame_req.dev_id = 1;
	frame_req.func_code = 0x01;
	frame_req.reg_add_hi = 0;
	frame_req.reg_add_lo = 0;
	frame_req.reg_num_hi = 0;
	frame_req.reg_num_lo = NUM_DO;

	GenerateCRC_modubus((unsigned char*)(&frame_req), sizeof(frame_req)-2);

	res = m_sock_modbus.L_send((char*)(&frame_req), sizeof(frame_req), 5);
	if (res  < 0)
	{
		Unlock();
		return res;
	}

	//����Ӧ����Ϣ
	ssid = (frame_req.dev_id<<8) + frame_req.func_code;
	if (SOCK_MSG_TIMEOUT)
	{
		//��������rsp
		i = 0;
		while(i++<SOCK_MSG_TIMEOUT)
		{
			frame_rsp = (MODBUS_CMD01_FrameRsp*)m_sock_modbus.FrameRecvBySSID(ssid);
			if(frame_rsp)
			{
				break;
			}
			Sleep(1);
		}
	}

	if (frame_rsp)
	{
		//���ݴ����ڽ����߳��������˴�������ȷ������ȷ��Ӧ��
		delete frame_rsp;
	}
	else
	{
		res = SOCK_MSG_TIMEOUT;
		printf("%s ssid:%X timeover\r\n", __FUNCTION__, ssid);
	}

	Unlock();

	return 0;
}


int CSockDevice::GetDI( )
{
	int i;
	unsigned int ssid;
	int res = SOCK_OK;

	MODBUS_CMD02_FrameReq frame_req;
	MODBUS_CMD02_FrameRsp *frame_rsp;

	if (!GetStatus())
	{
		return res;
	}

	Lock();

	memset(&frame_req, 0, sizeof(frame_req));
	frame_req.dev_id = 1;
	frame_req.func_code = 0x02;
	frame_req.reg_add_hi = 0;
	frame_req.reg_add_lo = 0;
	frame_req.reg_num_hi = 0;
	frame_req.reg_num_lo = NUM_DI;

	GenerateCRC_modubus((unsigned char*)(&frame_req), sizeof(frame_req)-2);

	res = m_sock_modbus.L_send((char*)(&frame_req), sizeof(frame_req), 5);
	if (res  < 0)
	{
		Unlock();
		return res;
	}

	//����Ӧ����Ϣ
	ssid = (frame_req.dev_id<<8) + frame_req.func_code;
	if (SOCK_MSG_TIMEOUT)
	{
		//��������rsp
		i = 0;
		while(i++<SOCK_MSG_TIMEOUT)
		{
			frame_rsp = (MODBUS_CMD02_FrameRsp*)m_sock_modbus.FrameRecvBySSID(ssid);
			if(frame_rsp)
			{
				break;
			}
			Sleep(1);
		}
	}

	if (frame_rsp)
	{
		//���ݴ����ڽ����߳��������˴�������ȷ������ȷ��Ӧ��
		delete frame_rsp;
	}
	else
	{
		res = SOCK_MSG_TIMEOUT;
		printf("%s ssid:%X timeover\r\n", __FUNCTION__, ssid);
	}

	Unlock();

	return 0;
}

int CSockDevice::GetAI( )
{
	int i;
	unsigned int ssid;
	int res = SOCK_OK;


	MODBUS_CMD04_FrameReq frame_req;
	MODBUS_CMD04_FrameRsp *frame_rsp;

	if (!GetStatus())
	{
		return res;
	}

	Lock();

	memset(&frame_req, 0, sizeof(frame_req));
	frame_req.dev_id = 1;
	frame_req.func_code = 0x04;
	frame_req.reg_add_hi = 0;
	frame_req.reg_add_lo = 0;
	frame_req.reg_num_hi = 0;
	frame_req.reg_num_lo = NUM_DI;

	GenerateCRC_modubus((unsigned char*)(&frame_req), sizeof(frame_req)-2);

	res = m_sock_modbus.L_send((char*)(&frame_req), sizeof(frame_req), 5);
	if (res  < 0)
	{
		Unlock();
		return res;
	}

	//����Ӧ����Ϣ
	ssid = (frame_req.dev_id<<8) + frame_req.func_code;
	if (SOCK_MSG_TIMEOUT)
	{
		//��������rsp
		i = 0;
		while(i++<SOCK_MSG_TIMEOUT)
		{
			frame_rsp = (MODBUS_CMD04_FrameRsp*)m_sock_modbus.FrameRecvBySSID(ssid);
			if(frame_rsp)
			{
				break;
			}
			Sleep(1);
		}
	}

	if (frame_rsp)
	{
		//���ݴ����ڽ����߳��������˴�������ȷ������ȷ��Ӧ��
		delete frame_rsp;
	}
	else
	{
		res = SOCK_MSG_TIMEOUT;
		printf("%s ssid:%X timeover\r\n", __FUNCTION__, ssid);
	}

	Unlock();

	return 0;
}

int CSockDevice::GetPowerV(void)
 {
	int i;
	unsigned int ssid;
	int res = SOCK_OK;


	MODBUS_CMD03_FrameReq frame_req;
	MODBUS_CMD03_FrameRsp *frame_rsp;
	SOCK_CMD_Frame  * sock_msg_rsp;

	if (!GetStatus())
	{
		return res;
	}

	Lock();

	memset(&frame_req, 0, sizeof(frame_req));
	frame_req.dev_id = 2;//�˴���̫���ܿ�����
	frame_req.func_code = 0x03;
	frame_req.reg_add_hi = 0;
	frame_req.reg_add_lo = 0x18;
	frame_req.reg_num_hi = 0;
	frame_req.reg_num_lo = 1;

	GenerateCRC_modubus((unsigned char*)(&frame_req), sizeof(frame_req)-2);

	res = m_sock_modbus.L_send((char*)(&frame_req), sizeof(frame_req), 5);
	if (res  < 0)
	{
		Unlock();
		return res;
	}

	//����Ӧ����Ϣ
	ssid = (frame_req.dev_id<<8) + frame_req.func_code;
	if (SOCK_MSG_TIMEOUT)
	{
		//��������rsp
		i = 0;
		while(i++<SOCK_MSG_TIMEOUT)
		{
			sock_msg_rsp = m_sock_modbus.FrameRecvBySSID(ssid);
			if(sock_msg_rsp)
			{
				frame_rsp = (MODBUS_CMD03_FrameRsp*)sock_msg_rsp->data;
				m_bat_v = ((frame_rsp->data[0]<<8)+frame_rsp->data[1]);
				break;
			}
			Sleep(1);
		}
	}

	if (frame_rsp)
	{
		//���ݴ����ڽ����߳��������˴�������ȷ������ȷ��Ӧ��
		delete sock_msg_rsp;
	}
	else
	{
		res = SOCK_MSG_TIMEOUT;
		printf("%s ssid:%X timeover\r\n", __FUNCTION__, ssid);
	}

	Unlock();


	return 0;
}

int CSockDevice::GetLoadI(void)
{
		int i;
	unsigned int ssid;
	int res = SOCK_OK;


	MODBUS_CMD03_FrameReq frame_req;
	MODBUS_CMD03_FrameRsp *frame_rsp;
	SOCK_CMD_Frame  * sock_msg_rsp;

	if (!GetStatus())
	{
		return res;
	}

	Lock();

	memset(&frame_req, 0, sizeof(frame_req));
	frame_req.dev_id = 2;//�˴���̫���ܿ�����
	frame_req.func_code = 0x03;
	frame_req.reg_add_hi = 0;
	frame_req.reg_add_lo = 0x1D;
	frame_req.reg_num_hi = 0;
	frame_req.reg_num_lo = 1;

	GenerateCRC_modubus((unsigned char*)(&frame_req), sizeof(frame_req)-2);

	res = m_sock_modbus.L_send((char*)(&frame_req), sizeof(frame_req), 5);
	if (res  < 0)
	{
		Unlock();
		return res;
	}

	//����Ӧ����Ϣ
	ssid = (frame_req.dev_id<<8) + frame_req.func_code;
	if (SOCK_MSG_TIMEOUT)
	{
		//��������rsp
		i = 0;
		while(i++<SOCK_MSG_TIMEOUT)
		{
			sock_msg_rsp = m_sock_modbus.FrameRecvBySSID(ssid);
			if(sock_msg_rsp)
			{
				frame_rsp = (MODBUS_CMD03_FrameRsp*)sock_msg_rsp->data;
				m_load_I = ((frame_rsp->data[0]<<8)+frame_rsp->data[1]);
				break;
			}
			Sleep(1);
		}
	}

	if (frame_rsp)
	{
		//���ݴ����ڽ����߳��������˴�������ȷ������ȷ��Ӧ��
		delete sock_msg_rsp;
	}
	else
	{
		res = SOCK_MSG_TIMEOUT;
		printf("%s ssid:%X timeover\r\n", __FUNCTION__, ssid);
	}

	Unlock();


	return 0;
}

int CSockDevice::GetTemp(void)
{
	int i;
	unsigned int ssid;
	int res = SOCK_OK;


	MODBUS_CMD03_FrameReq frame_req;
	MODBUS_CMD03_FrameRsp *frame_rsp;
	SOCK_CMD_Frame  * sock_msg_rsp;

	if (!GetStatus())
	{
		return res;
	}

	Lock();

	memset(&frame_req, 0, sizeof(frame_req));
	frame_req.dev_id = 2;//�˴���̫���ܿ�����
	frame_req.func_code = 0x03;
	frame_req.reg_add_hi = 0;
	frame_req.reg_add_lo = 0x25;
	frame_req.reg_num_hi = 0;
	frame_req.reg_num_lo = 1;

	GenerateCRC_modubus((unsigned char*)(&frame_req), sizeof(frame_req)-2);

	res = m_sock_modbus.L_send((char*)(&frame_req), sizeof(frame_req), 5);
	if (res  < 0)
	{
		Unlock();
		return res;
	}

	//����Ӧ����Ϣ
	ssid = (frame_req.dev_id<<8) + frame_req.func_code;
	if (SOCK_MSG_TIMEOUT)
	{
		//��������rsp
		i = 0;
		while(i++<SOCK_MSG_TIMEOUT)
		{
			sock_msg_rsp = m_sock_modbus.FrameRecvBySSID(ssid);
			if(sock_msg_rsp)
			{
				frame_rsp = (MODBUS_CMD03_FrameRsp*)sock_msg_rsp->data;
				m_env_temp = ((frame_rsp->data[0]<<8)+frame_rsp->data[1]);
				break;
			}
			Sleep(1);
		}
	}

	if (frame_rsp)
	{
		//���ݴ����ڽ����߳��������˴�������ȷ������ȷ��Ӧ��
		delete sock_msg_rsp;
	}
	else
	{
		res = SOCK_MSG_TIMEOUT;
		printf("%s ssid:%X timeover\r\n", __FUNCTION__, ssid);
	}

	Unlock();


	return 0;
}

int CSockDevice::ResetPower(void)
{
	int i;
	unsigned int ssid;
	int res = SOCK_OK;


	MODBUS_CMD05_FrameReq frame_req;
	MODBUS_CMD05_FrameRsp *frame_rsp;

	if (!GetStatus())
	{
		return res;
	}

	Lock();

	memset(&frame_req, 0, sizeof(frame_req));
	frame_req.dev_id = 2;//�˴���̫���ܿ�����
	frame_req.func_code = 0x05;
	frame_req.reg_add_hi = 0;
	frame_req.reg_add_lo = 0xff;
	frame_req.reg_val_hi = 0xff;
	frame_req.reg_val_lo = 0;

	GenerateCRC_modubus((unsigned char*)(&frame_req), sizeof(frame_req)-2);

	res = m_sock_modbus.L_send((char*)(&frame_req), sizeof(frame_req), 5);
	if (res  < 0)
	{
		;
	}

	Unlock();
	
	return res;
}

//���������ݸ�λΪ��ʼ״̬
int CSockDevice::ResetData(void)
{

	Lock();
	m_pu_ver = 0;
	m_pu_name.Empty();
	m_pu_model.Empty();

	res_num = 0;

	//m_postmsg_handle = NULL;

	flg_online = FALSE;

	memset(m_DI, 0, sizeof(m_DI));
	memset(m_DO, 0, sizeof(m_DO));
	memset(m_Do_count,0,sizeof(m_Do_count));

	memset(&m_cfg, 0, sizeof(m_cfg));
	for (int i=0; i<NUM_DO; i++)
	{
		m_cfg.m_DO_delay[i] = 5*60; //Ĭ��5���ӹر�
	}

	for (int i=0; i<sizeof(m_AI)/sizeof(m_AI[0]); i++)
	{
		m_AI[i]= 0;
	}

	m_sock_modbus.ResetData();

	Unlock();
	return 0;
}




int ProcessMessage(SOCK_CMD_Frame * pFrameReq, SOCK_CMD_Frame * pFrameRsp)
{
	int res = SOCK_OK;
	//::PostMessageA( dev->m_postmsg_handle, WM_MESSAGE_EVENT,dev->m_pu_id, event->event_type);

	return res;
}
//MC �������߳�  ��Ҫ�������������ͽ��շ���ֵ
#define BUF_RECV_MAX 1024
#define BUF_SEND_MAX 1024


unsigned WINAPI MC_thread_client(LPVOID  lpParam)
{
	int res = 0;
	int i;

	CSockDevice * device;
	CSockProc* handle;
	SOCKET s_client;
	long time_timeout = 0;

	int frame_size = 0;
	int func_code = 0;
	int dev_id;
	int process_result = 0;

	unsigned char * buf_recv = new unsigned char[BUF_RECV_MAX];
	assert(buf_recv);
	int buf_size = BUF_RECV_MAX;
	unsigned char * buf_send = new unsigned char[BUF_SEND_MAX];
	assert(buf_send);

	SOCK_CMD_Frame *frame_dummy; //����ͬ����frame֡����	

	fd_set         read, write, err;
	struct timeval tv;

	handle = (CSockProc *)lpParam;

	device = (CSockDevice *)handle->m_device;
	//assert(mng_dev);


	//��login�̷߳��أ�������register��Ϣ��rsp��
	Sleep(1000);

	//������ʱ��ʼ
	handle->m_beat_count = 0;

	handle->lockSock();
	//��Ϊ�豸������
	handle->flg_online = TRUE;
	PostMessageA((HWND)device->m_postmsg_handle, WM_MESSAGE_EVENT, device->m_pu_id, SOCK_MC_ONLINE);
	handle->unlockSock();


	while(WaitForSingleObject(handle->flg_event_exit, 0) != WAIT_OBJECT_0)
	{	
		tv.tv_sec = 0;
		tv.tv_usec = 100000;

		handle->lockSock();
		s_client = handle->m_sock;
		handle->unlockSock();

		if (s_client == INVALID_SOCKET)
		{
			res = 100;
			goto END0;
		}

		FD_ZERO(&read);
		FD_SET(s_client, &read);
		//FD_ZERO(&write);
		//FD_SET(s_client, &write);
		FD_ZERO(&err);
		FD_SET(s_client, &err);

		res=select(0,&read,NULL,&err,&tv);
		if (res <0)
		{
			//����//�����socket���ⲿ���رգ�return
			int err = WSAGetLastError();
			printf("PU:%d mc thread error %d\r\n", device->m_pu_id, err);
			res = 1;
			goto END0;
		}
		else if (res == 0)
		{
			//��ʱ
			time_timeout = InterlockedIncrement(&handle->m_beat_count);
			//printf("idel\r\n");
			//����ʱ10��ʱ���ߵ�������
			if (time_timeout >= MC_BEATEN_TIMEOUT)
			{
				printf("PU:%d mc thread timeout\r\n", device->m_pu_id);
				res = 2;
				goto END0;
			}

		} 
		else
		{
			if (FD_ISSET(s_client, &err))
			{
				//failed����
				printf("PU:%d mc thread select exception\r\n", device->m_pu_id);
				res = 3;
				goto END0;
			}

			if (FD_ISSET(s_client, &read))
			{
				buf_size = BUF_RECV_MAX;
				memset(buf_recv, 0, buf_size);

				handle->lockSock();//��֤֡ͷ��֡����Ҫһ�������꣬���ܱ����
				buf_size = handle->L_recv((char*)buf_recv,sizeof(MODBUS_CMD_FrameHead), 5);
				if (buf_size <= 0)
				{
					handle->unlockSock();
					res = 4;
					//�������ݴ�
					goto END0;
				}
				if (buf_size != sizeof(MODBUS_CMD_FrameHead))
				{
					handle->unlockSock();
					res = 5;
					//�������ݴ�
					goto END0;
				}

				//��鹦���룬ȷ�����ݳ��ȡ�
				dev_id = ((MODBUS_CMD_FrameHead*)buf_recv)->dev_id;
				func_code = ((MODBUS_CMD_FrameHead*)buf_recv)->func_code;

				switch(func_code)
				{
				case 1:
					frame_size = ((MODBUS_CMD_FrameHead*)buf_recv)->size+5;
					break;
				case 2:
					frame_size = ((MODBUS_CMD_FrameHead*)buf_recv)->size+5;
					break;
				case 3:
					frame_size = ((MODBUS_CMD_FrameHead*)buf_recv)->size+5;
					break;
				case 4:
					frame_size = ((MODBUS_CMD_FrameHead*)buf_recv)->size+5;
					break;
				case 5:
					frame_size = 8;
					break;
				case 100:
					frame_size = 4;
					break;
				default:
					frame_size = 8;//���п����������Ĳ���ʶ��
					break;
				}
				

				buf_size = handle->L_recv((char*)buf_recv+sizeof(MODBUS_CMD_FrameHead), frame_size-sizeof(MODBUS_CMD_FrameHead), 5);//10s �ڱ�����������
				handle->unlockSock();

				if (buf_size != frame_size-sizeof(MODBUS_CMD_FrameHead))
				{
					res = 6;
					//���ݴ�
					goto END0;
				}
				if (func_code != 100 && CRC16_modbus_CalCRC16( (const unsigned char *)buf_recv, frame_size))
				{
					res = 7;
					//У���
					goto END0;
				}

				printf("PU:%d recv msg addr %d, code:%d\r\n",device->m_pu_id, dev_id, func_code);

				printf("%X,%X,%X,%X,%X,%X,%X,%X\r\n",buf_recv[0],buf_recv[1],buf_recv[2],buf_recv[3],buf_recv[4],buf_recv[5],buf_recv[6],buf_recv[7]);
				//�յ���Ч����,����
				switch(func_code)
				{
				case 1:
					for (i=0; i<NUM_DO; i++)
					{
						device->m_DO[i] = (buf_recv[3]>>i)&0x01;
						//�����һ���豸ע�����ʱ��do���Ǳպ�״̬����Ҫ��ʼ�����Ա��ܹ����Զ��ر�
						if (device->m_DO[i])
						{
							if(device->m_Do_count[i] == 0)
							{
								__time32_t tt;
								_time32(&tt);
								device->m_Do_count[i] = tt;
							}
						}
					}
					process_result = 1;
					break;
				case 2:
					for (i=0; i<NUM_DI; i++)
					{
						device->m_DI[i] = (buf_recv[3]>>i)&0x01;
					}
					process_result = 1;
					break;

				case 3:
					//device->m_bat_v = ((buf_recv[3]<<8)+buf_recv[4]);
					process_result = 1;
					break;

				case 4:
					for (i=0; i<NUM_AI; i++)
					{
						device->m_AI[i] = (buf_recv[3+i*2]<<8) + (buf_recv[3+i*2+1]);
					}					
					break;
				case 5:
					process_result = 1;
					break;
				case 100:
					process_result = 1;
					break;
				default:
					process_result = 0;
					break;
				}

				//������ʱ����
				InterlockedExchange(&handle->m_beat_count, 0);
				if (!process_result)
				{
					//����ʶ����Ϣ������
					printf("Thrown an unknown msg!");
				}

				//ͬ��Ӧ���ź�
				//Ϊ�˱�ֵ����һ���ԣ�ʹ��SOCKPROC��Ļ����������ͬ������
				frame_dummy = (SOCK_CMD_Frame *)malloc(sizeof(SOCK_CMD_Frame) + frame_size);
				assert(frame_dummy);

				frame_dummy->head.session_id = func_code+(dev_id<<8);
				frame_dummy->head.pu_id = dev_id;
				frame_dummy->head.length = sizeof(SOCK_CMD_Frame) + frame_size;
				memcpy(frame_dummy->data, buf_recv, frame_size);
				handle->lockSock();
				SockMsgFrame * new_msg = new SockMsgFrame(frame_dummy);
				assert(new_msg);
				free(frame_dummy);
				handle->m_list_msg_recv.AddTail(new_msg);
				//���̫���ˣ�������õ�
				if(handle->m_list_msg_recv.GetCount() >= SOCK_MSG_BUFNUM)
				{
					printf("FIFO full!");
					new_msg = handle->m_list_msg_recv.RemoveHead();
					delete new_msg;
				}
				handle->unlockSock();

			}
			
		}

	}

END0:

	//�˳�ʱ�ر�����
	handle->lockSock();

	if (handle->m_sock != INVALID_SOCKET)
	{
		closesocket(handle->m_sock);
		handle->m_sock = INVALID_SOCKET;
	}
	//��Ϊ�豸����
	handle->flg_online = FALSE;
	if (device->m_postmsg_handle)
	{
		PostMessageA((HWND)device->m_postmsg_handle, WM_MESSAGE_EVENT, device->m_pu_id, SOCK_MC_OFFLINE);
	}

	handle->unlockSock();

	delete buf_send;
	delete buf_recv;
	printf("PU:%d mc processes exit at %d\r\n",device->m_pu_id, res);

	return res;

}

int CSockDevice::SetCfg(DevCfg &cfg)
{
	int i;
	
	Lock();

	memcpy(&m_cfg, &cfg, sizeof(m_cfg));

	Unlock();

	return 0;
}

int CSockDevice::GetCfg(DevCfg &cfg)
{
	int i;

	Lock();

	memcpy(&cfg, &m_cfg, sizeof(m_cfg));

	Unlock();

	return 0;
}


