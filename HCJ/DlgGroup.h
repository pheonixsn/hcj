#pragma once


// CDlgGroup �Ի���

class CDlgGroup : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgGroup)

public:
	CDlgGroup();   // ��׼���캯��
	virtual ~CDlgGroup();

// �Ի�������
	enum { IDD = IDD_DIALOG_GROUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();

	CWnd * m_main_wnd;
};
