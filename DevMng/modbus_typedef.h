

#ifndef _MODBUS_TYPEDEF_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _MODBUS_TYPEDEF_H_

#define MODBUS_CMD_LABLE "MDBS"

/**********************************����ϵͳ����ֵ����������*****************************************/
//�ɹ�
#define MODBUS_OK                         0
//��������
#define MODBUS_ERROR                    100            
//��ǰ�û���Ȩ��
#define MODBUS_ERROR_NOPERMIT    1
//�û���Ч���ѵ��ߵ������
#define MODBUS_ERROR_USER_NOLOGIN                 2  
/***********************ע�����������***********************/
//ID��Ч
#define MODBUS_ERROR_SERVER_IDENTIFY      11
//��֧���豸���������ˣ�
#define MODBUS_ERROR_SERVER_DEVICE        12
/******************************�ͻ��˵�¼************************************/
//�û�������
#define MODBUS_ERROR_USER_NO                   13
//������֤ʧ��
#define MODBUS_ERROR_USER_PWD                        14
//�û��ѵ�¼
#define MODBUS_ERROR_USER_LOGINED               15
//��¼�û�������
#define MODBUS_ERROR_USER_LOGINFULL                 16  
/******************************�û�����*************************************/
//�û�����
#define MODBUS_ERROR_USER_FULL                 21
//�û���¼��
#define MODBUS_ERROR_USER_ONLINE             22
/*******************************������************************************/
//������Ч��Ԥ��λ��
#define MODBUS_ERROR_ARM_POSINVALID                31
/*******************************�ƾ�����************************************/
//�����У����ܶ���
#define MODBUS_ERROR_ARM_ARMED                32
//��֧������
#define MODBUS_ERROR_PTZ_NOCMD                 33
/*******************************�豸����************************************/
//��ʱ
#define MODBUS_ERROR_UPDATE_OVERTIME         41
//�����ļ��ѱ���(CRC32У�����)
#define MODBUS_ERROR_UPDATE_IDENTIFY         42
//�����ļ��汾����
#define MODBUS_ERROR_UPDATE_VERSION          43
//�����ļ����õ��ͺ���Ŀ���豸���ͺŲ�ƥ��
#define MODBUS_ERROR_UPDATE_MATCH            44
//�����ļ��ĸ�ʽ����ȷ
#define MODBUS_ERROR_UPDATE_INVALID_FORMAT   45

/*******************************ͨ����������ý�塢�����ƣ�************************************/
//��֧��Э��
#define MODBUS_ERROR_NET_PROT                    51
//��֧�ֵ�ý������
#define MODBUS_ERROR_NET_MEDIA                   52
//Ŀ�ĵ�ַ���ɴ�(TCP)
#define MODBUS_ERROR_NET_DEST                     53
/*******************************ǰ���ļ�������Ƶ�ļ����ƾ�Э���ļ��ȣ�************************************/
//�ļ�ʹ����
#define MODBUS_ERROR_FILE_USING                    61
//�ļ�������
#define MODBUS_ERROR_FILE_NO                       62
//�ļ��洢��
#define MODBUS_ERROR_FILE_FULL                     63
//��������
#define MODBUS_ERROR_OTHER      100 

//------------ע��֡--------------
typedef struct 
{
	char smark[4];
	unsigned int 	pu_id;
	unsigned int 	pu_key;
	unsigned char	crc16_hi;
	unsigned char	crc16_low;
}MODBUS_CMD_RegReq;

//------------����֡--------------
typedef struct 
{
	unsigned char 	dev_id;
	unsigned char 	func_code;		//100 == beat
	unsigned char	crc16_hi;
	unsigned char	crc16_low;
}MODBUS_CMD_BeatReq;

//cmd frame head
typedef struct tagMODBUS_CMD_FrameHead 
{
	unsigned char 	dev_id;
	unsigned char 	func_code;
	unsigned char 	size;
	unsigned char 	data;
}MODBUS_CMD_FrameHead;

//--------------------04 cmd����ȡģ����ֵ---------------------
typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	reg_add_hi;
	unsigned char	reg_add_lo;
	unsigned char	reg_num_hi;
	unsigned char	reg_num_lo;
	unsigned char	crc16_hi;
	unsigned char	crc16_low;

}MODBUS_CMD04_FrameReq;

typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	size;		//����crc
	unsigned char	data[3];	//����crc16
}MODBUS_CMD04_FrameRsp;

//--------------------02 cmd����ȡ������ֵ---------------------
typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	reg_add_hi;
	unsigned char	reg_add_lo;
	unsigned char	reg_num_hi;
	unsigned char	reg_num_lo;
	unsigned char	crc16_hi;
	unsigned char	crc16_low;

}MODBUS_CMD02_FrameReq;

typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	size;		//����crc
	unsigned char	data[3];	//����crc16
}MODBUS_CMD02_FrameRsp;


//--------------------01 cmd����ȡ��Ȧֵ---------------------
typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	reg_add_hi;
	unsigned char	reg_add_lo;
	unsigned char	reg_num_hi;
	unsigned char	reg_num_lo;
	unsigned char	crc16_hi;
	unsigned char	crc16_low;

}MODBUS_CMD01_FrameReq;

typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	size;		//����crc
	unsigned char	data[3];	//����crc16
}MODBUS_CMD01_FrameRsp;

//--------------------05 cmd��������Ȧֵ---------------------
typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	reg_add_hi;
	unsigned char	reg_add_lo;
	unsigned char	reg_val_hi;
	unsigned char	reg_val_lo;
	unsigned char	crc16_hi;
	unsigned char	crc16_low;

}MODBUS_CMD05_FrameReq;

typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	reg_add_hi;
	unsigned char	reg_add_lo;
	unsigned char	reg_val_hi;
	unsigned char	reg_val_lo;
	unsigned char	crc16_hi;
	unsigned char	crc16_low;
}MODBUS_CMD05_FrameRsp;


//--------------------03 cmd�����Ĵ�����̫���ܿ�����---------------------
typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	reg_add_hi;
	unsigned char	reg_add_lo;
	unsigned char	reg_num_hi;
	unsigned char	reg_num_lo;
	unsigned char	crc16_hi;
	unsigned char	crc16_low;

}MODBUS_CMD03_FrameReq;


typedef struct 
{
	unsigned char 	dev_id;
	unsigned char	func_code;
	unsigned char	size;		//����crc
	unsigned char	data[3];	//����crc16
}MODBUS_CMD03_FrameRsp;


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
