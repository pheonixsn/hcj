// crc.cpp : Defines the entry point for the console application.
//

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "stdafx.h"
#include "sockcommon.h"


/* ========================================================================
* Table of CRC-32's of all single-byte values (made by make_crc_table)
*/


static const unsigned int crc_table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

/* ========================================================================= */
#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

/* ========================================================================= */
unsigned int crc32_ccitt(unsigned int crc, const unsigned char *buf, unsigned int len)

{

    if (buf == 0) return 0L;
#ifdef DYNAMIC_CRC_TABLE
    if (crc_table_empty)
      make_crc_table();
#endif
    crc = crc ^ 0;
    while (len >= 8)
    {
      DO8(buf);
      len -= 8;
    }

    if (len) do {
      DO1(buf);
    } while (--len);
    return crc ^ 0;
}

void GenerateCRC32(unsigned char * crc_buffer, int len)
{
	unsigned int crc_value;

	crc_value = crc32_ccitt(0, crc_buffer, len);

	*((unsigned int*)(crc_buffer + len)) = crc_value;
}

static unsigned short crc16_ccitt_table[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
	0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
	0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
	0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
	0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
	0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
	0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
	0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
	0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
	0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
	0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
	0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
	0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
	0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
	0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
	0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
	0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
	0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
	0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
	0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
	0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
	0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
	0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

unsigned short crc16_ccitt(unsigned short crc, const unsigned char *buf, unsigned int len)
{
	while (len-- > 0)
		crc = crc16_ccitt_table[(crc >> 8 ^ *buf++) & 0xff] ^ (crc << 8);
	return ~crc;
}

void GenerateCRC16(unsigned char * crc_buffer, int len)
{
	unsigned short crc_value;

	crc_value = crc16_ccitt(0, crc_buffer, len);

	*((unsigned short*)(crc_buffer + len)) = crc_value;
}

/////////////////////////////////////////////////////
//函数名称：CalOneByte
//功能：计算一个字节的CRC
//入口参数：AByte ：要计算的字节
//crc：返回的计算结果，这个也是一个入口参数，是上一个字节的计算结果。
/////////////////////////////////////////////////////
static void CRC16_modbus_CalOneByte(unsigned char AByte, unsigned short *crc)
{
	int i;
	unsigned char tmp;
	unsigned short crc2;
	crc2 = *crc;
	crc2 = crc2 ^ AByte;

	for(i=0; i<8; i++)
	{
		tmp = crc2 & 1;
		crc2 = crc2 >> 1;
		crc2 = crc2 & 0x7FFF;
		if(tmp)
		{
			crc2 = crc2 ^ 0xA001;
		}
		crc2 = crc2 & 0xFFFF;
	}
	*crc = crc2;
}

////////////////////////////////////////////////////////////
//函数名称：CalCRC16
//功能：计算一个数组的16位的CRC
//入口参数说明：
//buf: 要计算的数组(字节型）
//AStart：要计算的数组的起始序号，从零开始
//AEnd:   要计算的数组的结束序号，不能超过数组的长度
//返回值：返回16位的CRC，高字节在前，低字节在后
///////////////////////////////////////////////////////////
unsigned short CRC16_modbus_CalCRC16(const unsigned char *buf, int len)
{
	int i;
	unsigned short crc;

	crc = 0xFFFF;
	for(i=0; i<len; i++)
	{
		CRC16_modbus_CalOneByte(buf[i], &crc);
	}
	return crc;
}

void GenerateCRC_modubus(unsigned char * crc_buffer, int len)
{
	unsigned short crc_value;

	crc_value = CRC16_modbus_CalCRC16(crc_buffer, len);

	*((unsigned short*)(crc_buffer + len)) = crc_value;
}

int SOCK_recv(SOCKET s, char * buf, int lentgth, int time_out)
{
	int res;
	int offset;
	int size, id;
	int over_count = 0;
	offset = 0;
	res = 0;
	
	if (time_out == 0)
	{
		time_out = 0x7fffffff;
	}

	//calculate crc16 value

	while (offset < lentgth)
	{
		size = recv(s, (char*)(buf) + offset, lentgth-offset, 0);
		if (size > 0)
		{
			offset += size;
			res = offset;
		}
		else if(size == 0)
		{
			//close
			res = -1;
			break;
		}
		else
		{
			id =WSAGetLastError();
			if ( id == WSAEWOULDBLOCK || id == WSAEINPROGRESS || id == WSAEALREADY ) 
			{
				Sleep(1);
				over_count++;
				if (over_count >= time_out)
				{
					break;
				}
			}
			else
			{
				res = -1;
				break;
			}
		}
	}

	return res;
}


int SOCK_send(SOCKET s, char * buf, int lentgth, int time_out)
{
	int res;
	int offset;
	int size;
	int buf_size;
	int s_len;
	int err_code;
	int over_count = 0;

	if (time_out == 0)
	{
		time_out = 0x7fffffff;
	}
	offset = 0;
	res = 0;
	buf_size = lentgth;

	while (offset < buf_size)
	{
		s_len = buf_size-offset;

		//if (s_len > 2048)
		//{
		//	s_len = 2048;
		//}
		size = send(s, (char*)(buf) + offset, s_len, 0);

		if (size >0)
		{
			offset += size;
			res = offset;
		}
		else if(size == 0)
		{
			//close
			res = -1;
			break;
		}
		else
		{
			err_code = WSAGetLastError();
			if ( err_code == WSAEWOULDBLOCK || err_code == WSAEINPROGRESS || err_code == WSAEALREADY )
			{
				Sleep(1);
				over_count++;
				if (over_count >= time_out)
				{
					break;
				}
			}
			else
			{
				res = -1;
				break;
			}
		}
	}

	return res;
}


#define MAX_PRINTBUF 512
static volatile long _index = 0;
int SOCK_printf(const char * format, ...)
{
	va_list ap;
	char buffer[MAX_PRINTBUF];
	int size;
	long i;
	time_t tt;
	struct tm now_time;

	time(&tt);
	localtime_s(&now_time, &tt);

	memset(buffer, 0, MAX_PRINTBUF);
	i = _InterlockedIncrement(&_index);
	size = sprintf_s(buffer, MAX_PRINTBUF, "%05d-%02d:%02d:%02d ", i, now_time.tm_hour, now_time.tm_min, now_time.tm_sec);
	//size = strlen(buffer);

	va_start(ap, format);
	size += vsnprintf_s(buffer+size, MAX_PRINTBUF -size, MAX_PRINTBUF -size, format, ap);
	va_end(ap);

	printf_s(buffer);
	return size;
}