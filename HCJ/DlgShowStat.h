#pragma once


// CDlgShowStat �Ի���

class CDlgShowStat : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgShowStat)

public:
	CDlgShowStat();
	virtual ~CDlgShowStat();

// �Ի�������
	enum { IDD = IDD_DIALOG_SHOWSTAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
