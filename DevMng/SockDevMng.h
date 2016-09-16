#pragma once

#include "stdafx.h"

#include "SockProcListen.h"
#include "SockProcUDPSvr.h"

#include "SockDevice.h"


typedef struct tagDevMngCfg
{

	int m_svr_port;
	int m_udp_port;	//udp¼àÌý¶Ë¿Ú

}DevMngCfg;
class CSockDevMng
{
private:
	int m_flg_running;
public:
	CSockDevMng(void);
	~CSockDevMng(void);
	HANDLE m_parent_handle;

	DevMngCfg m_config;

	HANDLE m_mutex_listdev;
	
	CArray<CSockDevice*, CSockDevice*> m_list_device;

	CSockProcListen m_svr_mc;
	CSockProcUDPSvr m_svr_udp;

	int addDev(int pu_id, LPCTSTR pu_key);
	int delDev(int pu_id);
	int GetCount(void);
	
	int startSvr(void);
	int stopSvr(void);
	int setCfg(DevMngCfg& cfg);
	int getCfg(DevMngCfg& cfg);

	int DevLogin(SOCKET sock, int puid, void* login_param, int res_num);
	int DevKickout(int pu_id);
	int DevLoginLd(int puid, void* login_param);
	const CSockDevice * GetDevByID(int puid);
	const CSockDevice * GetDevByIndex(int index);
};

