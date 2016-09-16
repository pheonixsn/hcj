#pragma once

#include "stdafx.h"
#include "SockCommon.h"
#include "modbus_typedef.h"

//����һ��֧��modbus���豸����

#define NUM_AI (6)
#define NUM_DI (6)
#define NUM_DO (6)

#define SOCK_RES_MAX 32

typedef struct tagDevCfg
{
	int auto_refresh_internal;	
	unsigned short m_DO_delay[NUM_DO];	//do����ʱ״̬��0���ر�
}DevCfg;

class CSockDevice
{

private:
	HANDLE m_lock;

	DevCfg m_cfg;
public:
	CSockDevice(void);
	~CSockDevice(void);
	//����Ϣ�Ĵ��ھ��
	HWND m_postmsg_handle;

	int m_pu_id;
	CString m_pu_key;
	CString m_pu_name;
	CString m_pu_model;
	//char m_pu_key[16];
	//char m_pu_name[32];
	//char m_pu_model[32];	
	unsigned int m_pu_ver;

	int res_num;
	unsigned short m_AI[NUM_AI];
	unsigned short m_DI[NUM_DI];
	unsigned short m_DO[NUM_DO];
	
	unsigned short m_bat_v;			//��ص�ѹ
	unsigned short m_load_I;		//���ص���
	short m_env_temp;		//�����¶�

	CSockProc m_sock_modbus;

	//---status------------
	int flg_online;
	unsigned int m_Do_count[NUM_DO];//��ʱ��
	void Lock();
	void Unlock();
	int GetStatus(void);
	int StartProc(void);
	int StopProc(void);
	int ResetCount(void);
	int RefreshData(void);
	int ResetData(void);

	int CSockDevice::SetDOAuto(int index, unsigned int time_now);
	int SetDO(int index, int stat); //05 cmd,index 0-based
	int GetDO(void);	//01 cmd
	int GetDI(void);	//02 cmd
	int GetAI(void);	//04 cmd
	int GetPowerV(void);	//03 cmd
	int GetLoadI(void);		//03 cmd
	int GetTemp(void);		//03 cmd
	int ResetPower(void);

	int SetCfg(DevCfg& cfg);
	int GetCfg(DevCfg& cfg);
};

unsigned WINAPI MC_thread_client(LPVOID  lpParam);

