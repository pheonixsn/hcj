#pragma once


// CDlgTreeDev 对话框

class CDlgTreeDev : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgTreeDev)

public:
	CDlgTreeDev();   // 标准构造函数
	virtual ~CDlgTreeDev();

// 对话框数据
	enum { IDD = IDD_DIALOG_DEV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CWnd * m_main_wnd;

	virtual BOOL OnSetActive();
};
