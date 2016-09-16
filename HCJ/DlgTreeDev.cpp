// DlgTreeDev.cpp : 实现文件
//

#include "stdafx.h"
#include "HCJ.h"
#include "DlgTreeDev.h"
#include "afxdialogex.h"


// CDlgTreeDev 对话框

IMPLEMENT_DYNAMIC(CDlgTreeDev, CPropertyPage)

CDlgTreeDev::CDlgTreeDev()
: CPropertyPage(CDlgTreeDev::IDD)
{

}

CDlgTreeDev::~CDlgTreeDev()
{
}

void CDlgTreeDev::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgTreeDev, CPropertyPage)
END_MESSAGE_MAP()


// CDlgTreeDev 消息处理程序


BOOL CDlgTreeDev::OnSetActive()
{
	// TODO: 在此添加专用代码和/或调用基类

	if (m_main_wnd)
	{
		CRect rcSheet;
		//m_main_wnd->GetWindowRect(&rcSheet );
		m_main_wnd->GetClientRect(&rcSheet );
		//m_main_wnd->ScreenToClient( &rcSheet );
		//this->SetWindowPos( NULL,rcSheet.left, rcSheet.top+55, rcSheet.Width(), rcSheet.Height(),			NULL );
	}
	return CPropertyPage::OnSetActive();
}
