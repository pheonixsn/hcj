#pragma once
#include "sockproc.h"
#include "modbus_typedef.h"


extern unsigned WINAPI MC_thread_client(LPVOID  lpParam);
class CSockProcMC :
	public CSockProc
{
public:
	
	CSockProcMC(void);
	~CSockProcMC(void);
	FUNC_SockProcssMsgCB m_cb_func_processmsg;

	int SetProcMsgCB(FUNC_SockProcssMsgCB cb);

	int ResetData(void);
	int GetDI(int index, int *status);
	int SetDO(int index, int status);
};

