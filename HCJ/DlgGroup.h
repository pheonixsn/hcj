#pragma once


// CDlgGroup 对话框

class CDlgGroup : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgGroup)

public:
	CDlgGroup();   // 标准构造函数
	virtual ~CDlgGroup();

// 对话框数据
	enum { IDD = IDD_DIALOG_GROUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();

	CWnd * m_main_wnd;
};
