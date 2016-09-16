
// HCJDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "proccom_typedef.h"
#include "HCJ.h"
#include "HCJDlg.h"
#include "afxdialogex.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHCJDlg �Ի���




CHCJDlg::CHCJDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHCJDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHCJDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHCJDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CHCJDlg ��Ϣ�������

BOOL CHCJDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	//��ʼ��winsock���绷��
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD( 2, 2 );
	WSAStartup( wVersionRequested, &wsaData );

	//�򿪵����ն�
#ifdef	_DEBUG
	AllocConsole();	
	HWND hand_con = GetConsoleWindow();
	HMENU hand_menu = ::GetSystemMenu(hand_con, FALSE);
	DeleteMenu(hand_menu, SC_CLOSE, MF_BYCOMMAND);//���ùر�
	freopen("CONOUT$","w+t",stdout);
	printf_s("HCJ server Console\r\n");
#endif

	//���������ļ�����
	int len=GetModuleFileName(NULL,file_exe_name, MAX_PATH);//�õ�Ӧ�ó���·��
	_tcscpy_s(path_exe_name, file_exe_name);
	*_tcsrchr(path_exe_name,_T('\\')) = _T('\0');//ȥ���ļ���

	_tcscpy_s(path_cfg_name, MAX_PATH,path_exe_name);
	_tcscat_s(path_cfg_name,MAX_PATH-len-1,PATH_NAME_CFG);
	_tcscpy_s(path_rec_name, MAX_PATH,path_exe_name);
	_tcscat_s(path_rec_name,MAX_PATH-len-1,PATH_NAME_REC);

	_tcscpy_s(file_log_name, MAX_PATH,path_exe_name);
	_tcscat_s(file_log_name,MAX_PATH-len-1,FILE_NAME_LOG);
	_tcscpy_s(file_cfg_name, MAX_PATH,path_exe_name);
	_tcscat_s(file_cfg_name,MAX_PATH-len-1,FILE_NAME_CFG);
	_tcscpy_s(file_db_name, MAX_PATH,path_exe_name);
	_tcscat_s(file_db_name,MAX_PATH-len-1,FILE_NAME_DB);
	_tcscpy_s(file_log_name, MAX_PATH,path_exe_name);
	_tcscat_s(file_log_name,MAX_PATH-len-1,FILE_NAME_LOG);


	m_sql_db.OpenDB(file_db_name);
	m_sql_db.CloseDB();

	//m_sql_db.ExecuteSQL(_T("insert into recorders values(NULL,34,1,1,1,1,1,\"�Ǵ�ʿ������ط�\")"), NULL);

	//load cfg file
	CfgLoad();



	//update UI
	m_dlg_dev.m_main_wnd = &this->m_sheet_list;
	m_dlg_group.m_main_wnd = &this->m_sheet_list;

	m_sheet_list.AddPage(&m_dlg_dev);
	m_sheet_list.AddPage(&m_dlg_group);
	m_sheet_list.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	
	//m_sheet_list.AddPage(&m_dlg_showstat);

	//������ǩ�Ի���ؼ�
	m_sheet_list.Create(this,WS_CHILD | WS_VISIBLE, WS_EX_CONTROLPARENT );
//	m_sheet_list.DoModal();

	//ʹTab����Ϣ��Ч������ͨ��Tab��ת�����뽹��

	//m_sheet_list.ModifyStyle( 0, WS_TABSTOP );

	//ȷ����ǩ�Ի���ؼ��ڶԻ����г��ֵ�λ��

	CRect rcSheet;
	GetDlgItem( IDC_STATIC_TREE )->GetWindowRect( &rcSheet );
	ScreenToClient( &rcSheet );
	m_sheet_list.SetWindowPos( NULL,rcSheet.left, rcSheet.top, rcSheet.Width(), rcSheet.Height(),
		 SWP_NOSIZE |SWP_NOZORDER | SWP_NOACTIVATE );

	m_sheet_list.SetActivePage(0);

	this->SetWindowText(_T("Hybird Cable Judge Sys V0.1"));

	UpdateData(FALSE);


	//OnBnClickedButtonStart();

	SetTimer(1, 1000, NULL);

	PrintfLog("Start!\r\n");


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CHCJDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CHCJDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CHCJDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



int CHCJDlg::CfgLoad(void)
{
	return 0;
}


int CHCJDlg::CfgSave(void)
{
	return 0;
}

//������־���
#define MAX_PRINTBUF 512
static volatile long _index = 0;
int CHCJDlg::PrintfLog(const char * format, ...)
{
	CFile cf;
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
	size = sprintf_s(buffer, MAX_PRINTBUF, "%04d%02d%02d-%02d:%02d:%02d ", now_time.tm_year+1900, now_time.tm_mon+1, now_time.tm_mday, now_time.tm_hour, now_time.tm_min, now_time.tm_sec);
	//size = strlen(buffer);

	va_start(ap, format);
	size += vsnprintf_s(buffer+size, MAX_PRINTBUF -size, MAX_PRINTBUF -size, format, ap);
	va_end(ap);

	if (cf.Open(file_log_name, CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate))
	{
		cf.SeekToEnd();
		cf.Write(buffer, size);

		cf.Close();
	}

	//if (m_log.GetLength() > 65536)
	//{
	//	m_log.Delete(32768-1, m_log.GetLength()-32768);
	//}

	//m_log = buffer + m_log;
	UpdateData(FALSE);
	return size;
	return 0;
}
