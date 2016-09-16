
// HCJDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"

#include "./../sqlite/sqlite3.h"
#include "MySqlite.h"

#include "DlgTreeDev.h"
#include "DlgGroup.h"
#include "DlgShowStat.h"

//define user msg
//�Զ�����Ϣ
#define WM_MESSAGE_TRAY (WM_USER+100)  // about tray
#define WM_MESSAGE_EVENT (WM_USER+101)  // new event
#define WM_MESSAGE_REFRESH (WM_USER+102)  // update


//difine file name
#define FILE_NAME_CFG _T("\\config.ini")
#define FILE_NAME_LOG _T("\\log.txt")
#define FILE_NAME_DB _T("\\hcjdatabase.sqlite3")
#define PATH_NAME_CFG _T("\\config")
#define PATH_NAME_REC _T("\\Rec")


class CHCJSheet : public CPropertySheet
{
	HANDLE  m_ivfd_handle;
};

// CHCJDlg �Ի���
class CHCJDlg : public CDialogEx
{
// ����
public:
	CHCJDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_HCJ_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	CHCJSheet m_sheet_list;
	CHCJSheet m_sheet_show;
	CHCJSheet m_sheet_event;

	CDlgTreeDev m_dlg_dev;
	CDlgGroup	m_dlg_group;
	CDlgShowStat m_dlg_showstat;

	TCHAR file_exe_name[MAX_PATH];
	TCHAR file_cfg_name[MAX_PATH];
	TCHAR file_log_name[MAX_PATH];
	TCHAR file_db_name[MAX_PATH];
	TCHAR path_rec_name[MAX_PATH];
	TCHAR path_cfg_name[MAX_PATH];
	TCHAR path_exe_name[MAX_PATH];

	CMySqlite m_sql_db;

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:	
	CTreeCtrl m_treectrl_dev;
	int CfgLoad(void);
	int CfgSave(void);
	int PrintfLog(const char * format, ...);
};
