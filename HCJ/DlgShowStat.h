#pragma once


// CDlgShowStat 对话框

class CDlgShowStat : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgShowStat)

public:
	CDlgShowStat();
	virtual ~CDlgShowStat();

// 对话框数据
	enum { IDD = IDD_DIALOG_SHOWSTAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
