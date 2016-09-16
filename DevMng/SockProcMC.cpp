#include "StdAfx.h"
#include "SockProcMC.h"

#define printf SOCK_printf

CSockProcMC::CSockProcMC(void)
{
	//CLineyesDevice *dev;
	
	
}


CSockProcMC::~CSockProcMC(void)
{
	StopProc();
}

int CSockProcMC::SetProcMsgCB(FUNC_SockProcssMsgCB cb)
{
	lock();
	if (cb)
	{
		m_cb_func_processmsg = cb;
	}
	unlock();
	return 0;
}



int CSockProcMC::ResetData(void)
{
	int res;
	res =  CSockProc::ResetData();
	return res;
}


int CSockProcMC::PowerInfoGet(SOCK_CMD_FrameDPowerInfoGetRsp* info)
{
	int i, res = SOCK_OK;
	unsigned int ssid;
	SOCK_CMD_Frame * pFrameReq = NULL, * pFrameRsp = NULL;
	SOCK_CMD_FrameDPowerInfoGetReq * data_req;
	SOCK_CMD_FrameDPowerInfoGetRsp * data_rsp;

	lock();

	if (GetStatusOnline())
	{
	pFrameReq = (SOCK_CMD_Frame*)malloc(SOCK_MSG_MAX);
		assert(pFrameReq);

		data_req = (SOCK_CMD_FrameDPowerInfoGetReq *)pFrameReq->data;

		pFrameReq->head.type = SOCK_CMD_POWERINFO_GET;
		pFrameReq->head.length = sizeof(SOCK_CMD_Frame) + sizeof(SOCK_CMD_FrameDPowerInfoGetReq);

		FrameSend(pFrameReq);

		ssid = pFrameReq->head.session_id;

		if (SOCK_MSG_TIMEOUT)
		{
			//阻塞，等rsp
			i = 0;
			while(i++<SOCK_MSG_TIMEOUT)
			{
				pFrameRsp = FrameRecvBySSID(ssid);
				if(pFrameRsp)
				{
					break;
				}
				Sleep(1);
			}
		}

		if (pFrameRsp)
		{
			data_rsp = (SOCK_CMD_FrameDPowerInfoGetRsp *)pFrameRsp->data;
			memcpy_s(info, sizeof(SOCK_CMD_FrameDPowerInfoGetRsp), data_rsp, sizeof(SOCK_CMD_FrameDPowerInfoGetRsp));
			res = data_rsp->result;

			free(pFrameRsp);
		}
		else
		{
			res = SOCK_ERROR_OVERTIME;
			printf("%s ssid:%u timeover\r\n", __FUNCTION__, ssid);
		}
	}
	
	unlock();
	return res;
}


int CSockProcMC::PowerSectionGet(SOCK_CMD_FrameDPowerSectionGetRsp * info)
{
	int i, res = SOCK_OK;
	unsigned int ssid;
	SOCK_CMD_Frame * pFrameReq = NULL, * pFrameRsp = NULL;
	SOCK_CMD_FrameDPowerSectionGetReq * data_req;
	SOCK_CMD_FrameDPowerSectionGetRsp * data_rsp;

	lock();

	if (GetStatusOnline())
	{
	pFrameReq = (SOCK_CMD_Frame*)malloc(SOCK_MSG_MAX);
		assert(pFrameReq);

		data_req = (SOCK_CMD_FrameDPowerSectionGetReq *)pFrameReq->data;

		pFrameReq->head.type = SOCK_CMD_POWERSECTION_GET;
		pFrameReq->head.length = sizeof(SOCK_CMD_Frame) + sizeof(SOCK_CMD_FrameDPowerSectionGetReq);

		FrameSend(pFrameReq);

		ssid = pFrameReq->head.session_id;

		if (SOCK_MSG_TIMEOUT)
		{
			//阻塞，等rsp
			i = 0;
			while(i++<SOCK_MSG_TIMEOUT)
			{
				pFrameRsp = FrameRecvBySSID(ssid);
				if(pFrameRsp)
				{
					break;
				}
				Sleep(1);
			}
		}

		if (pFrameRsp)
		{
			data_rsp = (SOCK_CMD_FrameDPowerSectionGetRsp *)pFrameRsp->data;
			memcpy_s(info, sizeof(SOCK_CMD_FrameDPowerSectionGetRsp), data_rsp, sizeof(SOCK_CMD_FrameDPowerSectionGetRsp));
			res = data_rsp->result;

			free(pFrameRsp);
		}
		else
		{
			res = SOCK_ERROR_OVERTIME;
			printf("%s ssid:%u timeover\r\n", __FUNCTION__, ssid);
		}
	}
	
	unlock();
	return res;
}


int CSockProcMC::CommParaGet(SOCK_CMD_FrameDCommParaGetRsp * cfg)
{
	int i, res = SOCK_OK;
	unsigned int ssid;
	SOCK_CMD_Frame * pFrameReq = NULL, * pFrameRsp = NULL;
	SOCK_CMD_FrameDCommParaGetReq * data_req;
	SOCK_CMD_FrameDCommParaGetRsp * data_rsp;

	lock();
	if (GetStatusOnline())
	{
		pFrameReq = (SOCK_CMD_Frame*)malloc(SOCK_MSG_MAX);
		assert(pFrameReq);

		data_req = (SOCK_CMD_FrameDCommParaGetReq *)pFrameReq->data;

		pFrameReq->head.type = SOCK_CMD_COMMPARA_GET;
		pFrameReq->head.length = sizeof(SOCK_CMD_Frame) + sizeof(SOCK_CMD_FrameDCommParaGetReq);
	
		FrameSend(pFrameReq);

		ssid = pFrameReq->head.session_id;

		if (SOCK_MSG_TIMEOUT)
		{
			//阻塞，等rsp
			i = 0;
			while(i++<SOCK_MSG_TIMEOUT)
			{
				pFrameRsp = FrameRecvBySSID(ssid);
				if(pFrameRsp)
				{
					break;
				}
				Sleep(1);
			}
		}

		if (pFrameRsp)
		{
			data_rsp = (SOCK_CMD_FrameDCommParaGetRsp *)pFrameRsp->data;
			memcpy_s(cfg, sizeof(SOCK_CMD_FrameDCommParaGetRsp), data_rsp, sizeof(SOCK_CMD_FrameDCommParaGetRsp));
			res = data_rsp->result;

			free(pFrameRsp);
		}
		else
		{
			res = SOCK_ERROR_OVERTIME;
			printf("%s ssid:%u timeover\r\n", __FUNCTION__, ssid);
		}
	}
	
	unlock();
	return res;
}


int CSockProcMC::WarnLimitsParaGet(SOCK_CMD_FrameDWarnLimitsParaGetRsp * para)
{
	return 0;
}


int CSockProcMC::SensorDataCfgGet(SOCK_CMD_FrameDSensorDataCfgGetRsp * cfg)
{
	return 0;
}

//主动查询当前传感器状态
int CSockProcMC::SensorStatGet(unsigned int res_id, double *val)
{
	int i, res = SOCK_OK;
	unsigned int ssid;
	SOCK_CMD_Frame * pFrameReq = NULL, * pFrameRsp = NULL;
	SOCK_CMD_FrameDSensorStatGetReq * data_req;
	SOCK_CMD_FrameDSensorStatGetRsp * data_rsp;
	
	lock();
	if (GetStatusOnline())
	{
	pFrameReq = (SOCK_CMD_Frame*)malloc(SOCK_MSG_MAX);
		assert(pFrameReq);

		data_req = (SOCK_CMD_FrameDSensorStatGetReq *)pFrameReq->data;

		pFrameReq->head.type = SOCK_CMD_SENSORSTATUS_GET;
		pFrameReq->head.length = sizeof(SOCK_CMD_Frame) + sizeof(SOCK_CMD_FrameDSensorStatGetReq);
		data_req->res_id = res_id;

		FrameSend(pFrameReq);

		ssid = pFrameReq->head.session_id;

		if (SOCK_MSG_TIMEOUT)
		{
			//阻塞，等rsp
			i = 0;
			while(i++<SOCK_MSG_TIMEOUT)
			{
				pFrameRsp = FrameRecvBySSID(ssid);
				if(pFrameRsp)
				{
					break;
				}
				Sleep(1);
			}
		}

		if (pFrameRsp)
		{
			data_rsp = (SOCK_CMD_FrameDSensorStatGetRsp *)pFrameRsp->data;
			*val = data_rsp->status;
			res = data_rsp->result;

			free(pFrameRsp);
		}
		else
		{
			res = SOCK_ERROR_OVERTIME;
			printf("%s ssid:%u timeover\r\n", __FUNCTION__, ssid);
		}
	}
	unlock();
	return res;
}



int CSockProcMC::PowerSectionSet(int index, int status)
{
	int i, res = SOCK_OK;
	unsigned int ssid;
	SOCK_CMD_Frame * pFrameReq = NULL, * pFrameRsp = NULL;
	SOCK_CMD_FrameDPowerSectionSetReq * data_req;
	SOCK_CMD_FrameDPowerSectionSetRsp * data_rsp;

	lock();

	if (GetStatusOnline())
	{
		pFrameReq = (SOCK_CMD_Frame*)malloc(SOCK_MSG_MAX);
		assert(pFrameReq);

		data_req = (SOCK_CMD_FrameDPowerSectionSetReq *)pFrameReq->data;

		pFrameReq->head.type = SOCK_CMD_POWERSECTION_SET;
		pFrameReq->head.length = sizeof(SOCK_CMD_Frame) + sizeof(SOCK_CMD_FrameDPowerSectionSetReq);
		data_req->index = index;
		data_req->status = status;

		FrameSend(pFrameReq);

		ssid = pFrameReq->head.session_id;

		if (SOCK_MSG_TIMEOUT)
		{
			//阻塞，等rsp
			i = 0;
			while(i++<SOCK_MSG_TIMEOUT)
			{
				pFrameRsp = FrameRecvBySSID(ssid);
				if(pFrameRsp)
				{
					break;
				}
				Sleep(1);
			}
		}

		if (pFrameRsp)
		{
			data_rsp = (SOCK_CMD_FrameDPowerSectionSetRsp *)pFrameRsp->data;

			res = data_rsp->result;

			free(pFrameRsp);
		}
		else
		{
			res = SOCK_ERROR_OVERTIME;
			printf("%s ssid:%u timeover\r\n", __FUNCTION__, ssid);
		}
	}

	unlock();
	return res;
}
