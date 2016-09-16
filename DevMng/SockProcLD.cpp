#include "StdAfx.h"

#include "lineyes_common.h"
#include "SockProcLD.h"
#include "LineyesDevMng.h"

CSockProcLD::CSockProcLD(void)
{
	

}


CSockProcLD::~CSockProcLD(void)
{
	int a = 0;
}


int CSockProcLD::VideoInfoCfgGet(SOCK_CMD_FrameDVideoInfoCfgGetRsp *cfg)
{
	return 0;
}


int CSockProcLD::ResetData(void)
{
	int res;
	res =  CSockProc::ResetData();
	return res;

	return 0;
}
