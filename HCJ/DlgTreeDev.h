#pragma once


// CDlgTreeDev �Ի���

class CDlgTreeDev : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgTreeDev)

public:
	CDlgTreeDev();   // ��׼���캯��
	virtual ~CDlgTreeDev();

// �Ի�������
	enum { IDD = IDD_DIALOG_DEV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CWnd * m_main_wnd;

	virtual BOOL OnSetActive();
};
