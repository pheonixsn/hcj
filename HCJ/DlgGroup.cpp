// DlgGroup.cpp : 实现文件
//

#include "stdafx.h"
#include "HCJ.h"
#include "DlgGroup.h"
#include "afxdialogex.h"


// CDlgGroup 对话框

IMPLEMENT_DYNAMIC(CDlgGroup, CPropertyPage)

CDlgGroup::CDlgGroup()
: CPropertyPage(CDlgGroup::IDD)
{

}

CDlgGroup::~CDlgGroup()
{
}

void CDlgGroup::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgGroup, CPropertyPage)
END_MESSAGE_MAP()


// CDlgGroup 消息处理程序


BOOL CDlgGroup::OnSetActive()
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
