// DlgTreeDev.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HCJ.h"
#include "DlgTreeDev.h"
#include "afxdialogex.h"


// CDlgTreeDev �Ի���

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


// CDlgTreeDev ��Ϣ�������


BOOL CDlgTreeDev::OnSetActive()
{
	// TODO: �ڴ����ר�ô����/����û���

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
