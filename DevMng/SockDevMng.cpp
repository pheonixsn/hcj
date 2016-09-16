#include "StdAfx.h"

#include "SockDevMng.h"

#define printf SOCK_printf

CSockDevMng::CSockDevMng(void)
{
	m_flg_running = 0;

	m_mutex_listdev = CreateMutex(NULL,0, NULL);
	assert(m_mutex_listdev);

	m_config.m_svr_port = 6000;
	m_config.m_udp_port = 6200;

	m_svr_mc.m_device_mng = this;
	m_svr_udp.m_device_mng = this;

}


CSockDevMng::~CSockDevMng(void)
{
	int i;

	stopSvr();

	while (m_list_device.GetCount() > 0)
	{
		delDev(m_list_device[0]->m_pu_id); 
	}

	if (m_mutex_listdev)
	{
		CloseHandle(m_mutex_listdev);
	}

}


int CSockDevMng::addDev(int pu_id, LPCTSTR pu_key)
{
	int i, res = 0;
	CSockDevice *dev = new CSockDevice;
	dev->ResetData();
	dev->m_pu_id = pu_id;
	dev->m_pu_key = pu_key;
	//给主窗口发消息的句柄
	dev->m_postmsg_handle = (HWND)m_parent_handle;

	dev->m_sock_modbus.SetProcCB(MC_thread_client);

	dev->ResetCount();
	
	WaitForSingleObject(m_mutex_listdev, INFINITE);
	for (i=0; i<m_list_device.GetCount(); i++)
	{
		if (m_list_device[i]->m_pu_id == dev->m_pu_id)
		{
			res =  -1; //device already existed
			break;
		}
	}

	if (res >= 0)
	{
		m_list_device.Add(dev);	
	}
	ReleaseMutex(m_mutex_listdev);
	return res;
}


int CSockDevMng::delDev(int pu_id)
{
	int	i, res = -1;
	CSockDevice *device = NULL;
	WaitForSingleObject(m_mutex_listdev, INFINITE);
	for (i=0; i<m_list_device.GetCount(); i++)
	{
		if (m_list_device[i]->m_pu_id == pu_id)
		{
			
			device = m_list_device.GetAt(i);
			if (device->GetStatus())
			{
				device->StopProc();
			}
			
			m_list_device.RemoveAt(i);
			delete device;
			res = SOCK_OK;
			break;
		}
	}
	ReleaseMutex(m_mutex_listdev);
	return res;  //no this PU
}


int CSockDevMng::GetCount(void)
{
	return m_list_device.GetCount();	
}

//
int CSockDevMng::DevKickout(int pu_id)
{
	int	i, res = SOCK_ERROR;
	CSockDevice *device = NULL;
	WaitForSingleObject(m_mutex_listdev, INFINITE);
	for (i=0; i<m_list_device.GetCount(); i++)
	{
		if (m_list_device[i]->m_pu_id == pu_id)
		{
			device = m_list_device.GetAt(i);
			device->StopProc();

			res = SOCK_OK;
			break;
		}
	}
	ReleaseMutex(m_mutex_listdev);
	return res;  //no this PU
	

}


int CSockDevMng::startSvr(void)
{

	int res = 0;
	WaitForSingleObject(m_mutex_listdev, INFINITE);


	if (!m_flg_running)
	{
		//start mc svr;
		m_svr_mc.SetPort(m_config.m_svr_port);
		m_svr_mc.SetProcCB(SOCKLISTEN_thread_server);
		m_svr_mc.SetProcTmpCB(SOCKLISTEN_thread_client);
		m_svr_mc.StartProc();

		m_svr_udp.SetPort(m_config.m_udp_port);
		m_svr_udp.SetProcCB(SOCKUDP_thread_server);
		m_svr_udp.StartProc();

		m_flg_running = 1;
		Sleep(1000);
	}

	ReleaseMutex(m_mutex_listdev);
	return res;
}


int CSockDevMng::stopSvr(void)
{
	int res = 0, i;

	CSockDevice *device = NULL;
	WaitForSingleObject(m_mutex_listdev, INFINITE);
	
	//先踢掉所有在线设备
	for (i=0; i<m_list_device.GetCount(); i++)
	{
		if (m_list_device[i]->GetStatus())
		{
			device = m_list_device.GetAt(i);
			
			DevKickout(device->m_pu_id);
		}
	}

	//再停止监听线程
	if (m_flg_running)
	{
		m_svr_mc.StopProc();
		m_svr_udp.StopProc();

		m_flg_running = 0;
	}

	ReleaseMutex(m_mutex_listdev);
	return res;
}


int CSockDevMng::setCfg(DevMngCfg& cfg)
{
	WaitForSingleObject(m_mutex_listdev, INFINITE);
	memcpy_s(&m_config, sizeof(m_config), &cfg, sizeof(cfg));
	ReleaseMutex(m_mutex_listdev);
	return 0;
}


int CSockDevMng::getCfg(DevMngCfg& cfg)
{
	WaitForSingleObject(m_mutex_listdev, INFINITE);
	memcpy_s(&cfg, sizeof(cfg), &m_config, sizeof(m_config));
	ReleaseMutex(m_mutex_listdev);
	return 0;
}


int CSockDevMng::DevLogin(SOCKET sock, int puid, void* login_param, int res_num = 0)
{
	int i, j;
	int size;
	char str_key[256];
	int res = SOCK_ERROR_SERVER_IDENTIFY;
	MODBUS_CMD_RegReq * param;
	CSockDevice *device = NULL;
	WaitForSingleObject(m_mutex_listdev, INFINITE);

	param = (MODBUS_CMD_RegReq*)login_param;

	for (i=0; i<m_list_device.GetCount(); i++)
	{
		device = m_list_device.GetAt(i);
		if (device->m_pu_id == puid)
		{
			//check key
			size = strnlen_s(device->m_pu_key, 16);
			if (!device->m_pu_key.IsEmpty() )
			{
				sprintf_s(str_key, "%lu",param->pu_key);
				if (device->m_pu_key.Compare(str_key)!=0)
				{
					//key error
					break;
				}
			}

			//会将已在线的LD也同时踢掉
			if (device->GetStatus())
			{
				device->StopProc();
			}
						
			device->m_sock_modbus.StopProc();
			//必须先stop，否则attach的sock会在stop时被关掉。
			device->m_sock_modbus.AttachSock(sock);
			device->m_sock_modbus.StartProc();

			device->flg_online |= device->m_sock_modbus.flg_online;
			res = SOCK_OK;
			printf("MC %d Login succeed!\r\n", device->m_pu_id);

			::PostMessageA((HWND)m_parent_handle, WM_MESSAGE_REFRESH, 1, NULL);
		}
	}
	
	ReleaseMutex(m_mutex_listdev);
	return res;
}



const CSockDevice* CSockDevMng::GetDevByID(int puid)
{
	int	i;
	CSockDevice *device = NULL;
	WaitForSingleObject(m_mutex_listdev, INFINITE);
	for (i=0; i<m_list_device.GetCount(); i++)
	{
		if (m_list_device[i]->m_pu_id == puid)
		{
			device = m_list_device.GetAt(i);

			break;
		}
	}
	ReleaseMutex(m_mutex_listdev);
	return device;
}


const CSockDevice* CSockDevMng::GetDevByIndex(int index)
{
	int	i;
	CSockDevice *device = NULL;
	WaitForSingleObject(m_mutex_listdev, INFINITE);
	if (index < m_list_device.GetCount())
	{
		device = m_list_device[index];
	}
	
	ReleaseMutex(m_mutex_listdev);
	return device;
	
}
