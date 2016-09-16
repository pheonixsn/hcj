#pragma once
#include "stdafx.h"
#include "sockproc.h"
class CSockProcLD :
	public CSockProc
{
public:

	CSockProcLD(void);
	~CSockProcLD(void);	
	
	

	int ProcessMessage(SOCK_CMD_Frame * pFrameReq, SOCK_CMD_Frame * pFrameRsp);
	int VideoInfoCfgGet(SOCK_CMD_FrameDVideoInfoCfgGetRsp * cfg);
	int ResetData(void);
};

