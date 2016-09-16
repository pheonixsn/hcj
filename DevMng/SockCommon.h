#ifndef __SOCK_COMMON_H__
#define __SOCK_COMMON_H__

//��ʱ��Ӧ��
#define SOCK_ERROR_OVERTIME                 5  

//MAX MSG
#define SOCK_MSG_MAX 1024000

//CMD RSP TIMEOUT(ms)
#define SOCK_MSG_TIMEOUT 5000

//������յ�msg��Ŀ	100
#define SOCK_MSG_BUFNUM 100

//mc������ʱ
#define MC_BEATEN_TIMEOUT	(120*10) 

//�Զ�����Ϣ������MFC�����е�һ��
#define WM_MESSAGE_TRAY (WM_USER+100)  // about tray
#define WM_MESSAGE_EVENT (WM_USER+101)  // 
#define WM_MESSAGE_REFRESH (WM_USER+102)  // 


#define SOCK_MC_ONLINE  0x01 
#define SOCK_MC_OFFLINE  0x02
#define SOCK_LD_ONLINE  0x100001 
#define SOCK_LD_OFFLINE  0x100002

/**********************************����ϵͳ����ֵ����������*****************************************/
//�ɹ�
#define SOCK_OK                         0
//��������
#define SOCK_ERROR                    100            
//��ǰ�û���Ȩ��
#define SOCK_ERROR_NOPERMIT    1
//�û���Ч���ѵ��ߵ������
#define SOCK_ERROR_USER_NOLOGIN                 2  
/***********************ע�����������***********************/
//ID��Ч
#define SOCK_ERROR_SERVER_IDENTIFY      11
//��֧���豸���������ˣ�
#define SOCK_ERROR_SERVER_DEVICE        12
/******************************�ͻ��˵�¼************************************/
//�û�������
#define SOCK_ERROR_USER_NO                   13
//������֤ʧ��
#define SOCK_ERROR_USER_PWD                        14
//�û��ѵ�¼
#define SOCK_ERROR_USER_LOGINED               15
//��¼�û�������
#define SOCK_ERROR_USER_LOGINFULL                 16  

#define  SOCK_CMD_LABLE "LEYE"

//cmd frame head ͨ��֡ͷ��ʽ
typedef struct tagSOCK_CMD_FrameHead 
{
	//֡ͷ��־"LEYE"
	char smark[4];

	unsigned int 	length;
	unsigned short 	type;
	unsigned short 	msg_id;
	unsigned int 	time_stamp;
	unsigned int	reserved;
	unsigned int	session_id;
	unsigned int 	pu_id;

}SOCK_CMD_FrameHead;

typedef struct tagSOCK_CMD_Frame 
{
	SOCK_CMD_FrameHead	head;
	char					data[4];		//crc32 value

}SOCK_CMD_Frame;


//-------------------------��������Ϣ���Ͷ���------------------------------------
#define SOCK_CMDTYPE_BASE			0x1000
#define SOCK_CMDTYPE_LOGIN			(SOCK_CMDTYPE_BASE+1002)  // �豸���ߣ�

#define SOCK_CMDTYPE_LOGOUT			(SOCK_CMDTYPE_BASE+1002)  // �豸����


#define SOCK_CMDTYPE_CHECKDEV_RSP		(SOCK_CMDTYPE_BASE+1003)  // �豸�ϱ����豸״̬
#define SOCK_CMDTYPE_CHECKSENSOR_RSP	(SOCK_CMDTYPE_BASE+1004)  // �豸�ϱ��Ĵ�������Ϣ
#define SOCK_CMDTYPE_SETRELAY_RSP			(SOCK_CMDTYPE_BASE+1006)  // ����ָ���豸�ļ̵���״̬

//-------------------------����˽��յ���Ϣ-------------------------------
#define SOCK_CMDTYPE_CHECKDEV_REQ		(SOCK_CMDTYPE_BASE+1101)	// ��ѯָ���豸״̬��

#define SOCK_CMDTYPE_CHECKSENSOR_REQ	(SOCK_CMDTYPE_BASE+1102)  // ��ѯָ���豸������
#define SOCK_CMDTYPE_SETRELAY_REQ		(SOCK_CMDTYPE_BASE+1106)  // ����ָ���豸�ļ̵���״̬
//����豸�Ƿ�����
typedef struct  
{
	SOCK_CMD_FrameHead	head;

	int pu_id;								//0: all
	
	char					data[4];		//crc32 value
}SOCK_CMD_Frame_CHECKDEV_Req;
typedef struct  
{
	SOCK_CMD_FrameHead	head;

	int pu_id;								//0: all
	int pu_stat;							//0: offline; 1:online

	char					data[4];		//crc32 value
}SOCK_CMD_Frame_CHECKDEV_Rsp;

//����豸������ֵ
typedef struct  
{
	SOCK_CMD_FrameHead	head;

	int pu_id;								

	char					data[4];		//crc32 value
}SOCK_CMD_Frame_CHECKSENSOR_Req;
typedef struct  
{
	SOCK_CMD_FrameHead	head;

	int pu_id;								
	unsigned int DO_stat;		//max:32
	unsigned int DI_stat;		//max:32
	unsigned int PowerV;		//��ص�ѹ*100
	unsigned int Reserve1;		//
	unsigned int Reserve2;		//
	unsigned int Reserve3;		//

	char					data[4];		//crc32 value
}SOCK_CMD_Frame_CHECKSENSOR_Rsp;

//�����豸�̵���
typedef struct  
{
	SOCK_CMD_FrameHead	head;
	int pu_id;
	unsigned int DO_index;		//
	unsigned int DO_stat;		//0��open��default����1��close

	char					data[4];		//crc32 value
}SOCK_CMD_Frame_SETRELAY_Req;

typedef struct  
{
	SOCK_CMD_FrameHead	head;
	int pu_id;
	unsigned int DO_index;		//
	unsigned int DO_stat;		//0��open��default����1��close

	char					data[4];		//crc32 value
}SOCK_CMD_Frame_SETRELAY_Rsp;

#ifdef __cplusplus
extern "C" {
#endif

	

	extern int SOCK_recv(SOCKET s, char * buf, int lentgth, int time_out);

	extern int SOCK_send(SOCKET s, char * buf, int lentgth, int time_out);

	extern int SOCK_printf(const char * format, ...);

	/*************************************************************************************
	Function name:	crc32

	Parameters:
		crc		: crc��ʼֵ��Ĭ��0
		buf		: ��У�������ָ��
		len		����У������ݳ���

	Return Values:	У�����ݵ�crcֵ

	Remarks: 

	*************************************************************************************
	*/

	unsigned int crc32_ccitt(unsigned int crc, const unsigned char *buf, unsigned int len);

	unsigned short crc16_ccitt(unsigned short crc, const unsigned char *buf, unsigned int len);

	unsigned short CRC16_modbus_CalCRC16(const unsigned char *buf, int len);
	

	/*************************************************************************************
	Function name:	GenerateCRC
	b
	Parameters:
		crc_buffer	: ��У�������ָ��
		len			����У������ݳ���

	Return Values:	

	Remarks: �ڴ�У������ݺ������4�ֽڴ�С��CRCУ��ֵ�����У�����ݳ���10�ֽڣ���ôcrc_buffer�Ĵ�С����Ҫ����14�ֽ�

	*************************************************************************************
	*/

	void GenerateCRC32(unsigned char * crc_buffer, int len);

	void GenerateCRC16(unsigned char * crc_buffer, int len);

	void GenerateCRC_modubus(unsigned char * crc_buffer, int len);


#ifdef __cplusplus
}
#endif

#endif