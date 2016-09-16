#pragma once
#include "./../sqlite/sqlite3.h"
#include <process.h>

const LPCTSTR SQLITE_NAME = _T("database.db"); //创建的数据文件名
class CMySqlite
{
public:
	CMySqlite(LPCTSTR sql = SQLITE_NAME);
	~CMySqlite(void);
	int ExecuteSQL(LPCTSTR sql = NULL, sqlite3_stmt **stat = NULL);
	bool OpenDB(LPCTSTR sql_name=NULL);
	void CloseDB();
private: 
	HANDLE m_lock_rw;
	
	static void ThreadFunc(PVOID pData); int DoSQL();  //可以不用多线程
	HANDLE m_hRead;
	sqlite3 *db;
	TCHAR dbName[MAX_PATH];
	LPCTSTR sqlTemp;
	sqlite3_stmt **ssTemp;
	int nResult;
	// char errormessage[256];
public:
	void Lock(void);
	void UnLock(void);
};


