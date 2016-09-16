#include "StdAfx.h"
#include "MySqlite.h"



CMySqlite::CMySqlite(LPCTSTR sql)
	:db(NULL)
	,sqlTemp(NULL),ssTemp(NULL),nResult(0)
{

	memset(dbName, 0, sizeof(dbName));
	m_hRead = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(m_hRead);
	SetEvent(m_hRead);

	m_lock_rw = CreateMutex(NULL,0, NULL);
	assert(m_lock_rw);
	// memset(errormessage,0,sizeof(errormessage));
}

CMySqlite::~CMySqlite(void)
{
	if (m_hRead)
	{
		CloseHandle(m_hRead);
	}
		
	if (m_lock_rw)
	{
		CloseHandle(m_lock_rw);
	}
}

bool CMySqlite::OpenDB(LPCTSTR sql_name )
{
	
	 char* errmsg = NULL;

	if(db != NULL)
	{
		CloseDB();
	}
	if(sql_name )
	{
		_tcscpy_s(dbName, MAX_PATH,sql_name);
	}
	
	if (_tcslen(dbName) == 0)
	{
		return false;
	}

	Lock();
	int rc = sqlite3_open16(dbName,&db);
	if (rc != SQLITE_OK)
	{
		printf("SQLite: open db file error!\r\n", errmsg);

	}
	UnLock();

	if(rc != SQLITE_OK)
	{
		return false;
	}

	//#if USE_CODE                                                           //这个是数据加密的，暂时没用
	// rc = sqlite3_key(m_hDB,"mcnair2011",10);
	// if(rc != SQLITE_OK)
	// {
	//  TRACE("ERROR:%s\n",sqlite3_errmsg(m_hDB));
	//  return false;
	// }
	//#endif

	return true;
}


void CMySqlite::CloseDB()
{
	Lock();
	if(db != NULL)
	{
		if(sqlite3_close(db) == SQLITE_OK)
		{
			db = NULL;
		}
		//memset(dbName, 0, sizeof(dbName));
	}
	UnLock();

}

#include <string>
using namespace std;
static int _mysql_callback(void* ,int nCount,char** pValue,char** pName)
{
	string s;
	for(int i=0;i<nCount;i++)
	{
		s+=pName[i];
		s+=":";
		s+=pValue[i];
		s+="\n";
	}
	//cout<<s<<endl;
	return 0;
}

int CMySqlite::ExecuteSQL(LPCTSTR sql, sqlite3_stmt **stat)
{

	WaitForSingleObject(m_hRead, INFINITE);
	nResult = 0;
	sqlTemp = sql;
	ssTemp = stat;

	HANDLE handle;
	handle = (HANDLE)_beginthread(ThreadFunc, 0, this);

	WaitForSingleObject(handle,INFINITE);

	return nResult;
}

void CMySqlite::ThreadFunc(PVOID pData)
{
	CMySqlite* pThis = (CMySqlite*)pData;
	pThis->DoSQL();
	SetEvent(pThis->m_hRead);
}

int CMySqlite::DoSQL()
{
	LPCTSTR sql = sqlTemp;
	sqlite3_stmt** ss = ssTemp;
	nResult = 0;

	if (!OpenDB())  //打开database
	{
		return -1;
	}


	if (sql == NULL && ss == NULL)//仅判断连接状态
	{
		CloseDB();
		return nResult;
	}

	USES_CONVERSION;

	 char* errmsg = NULL;

	if (ss == NULL)  //仅执行命令
	{
		nResult = sqlite3_exec(db,W2A_CP(sql,CP_UTF8),NULL,NULL,&errmsg);
		if (nResult != SQLITE_OK)
		{
			sqlite3_close(db);
			printf("SQLite: %s\r\n", errmsg);

			sqlite3_free(errmsg);
			return 0;
		}
		CloseDB();
	}
	else
	{
		sqlite3_prepare(db,W2A_CP(sql,CP_UTF8),-1,ss,0);

	}

	return nResult;
}


void CMySqlite::Lock(void)
{
	if (m_lock_rw)
	{
		WaitForSingleObject(m_lock_rw, INFINITE);
	}
}


void CMySqlite::UnLock(void)
{
	if (m_lock_rw)
	{
		ReleaseMutex(m_lock_rw);
	}
}
