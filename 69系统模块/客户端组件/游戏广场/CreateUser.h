#pragma once


// CCreateUser 对话框

class CCreateUser : public CDialog
{
	DECLARE_DYNAMIC(CCreateUser)

public:
	CCreateUser(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCreateUser();

protected:
	CPngImage			m_ImageBack;

	CSkinButton			m_btOk;
	CSkinButton			m_btCancel;
	CSkinButton			m_btCheck;
    
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
};
