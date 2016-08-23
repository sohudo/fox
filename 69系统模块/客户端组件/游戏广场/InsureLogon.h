#pragma once


// CInsureLogon 对话框

class CInsureLogon : public CDialog, public CMissionItem,public IKeyBoardSink
{
	DECLARE_DYNAMIC(CInsureLogon)

public:
	CInsureLogon(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInsureLogon();

protected:
	CPngImage			m_ImageBack;

	TCHAR				m_szInsurePassword[LEN_PASSWORD];

	CSkinButton			m_btClose;
	CSkinButton			m_btOk;
	CSkinButton			m_btCancle;
	CEdit				m_edInsurePassword;				//二级密码

	CPasswordKeyboard2	m_KeyBoard;

	//组件变量
protected:
	CMissionManager					m_MissionManager;					//任务管理

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual	VOID OnKeyBoardChar(WORD wChar);
	virtual	VOID OnKeyBoardClean();

	VOID OnBnClickedOk();

	//重载函数
public:
	//连接事件
	virtual bool OnEventMissionLink(INT nErrorCode);
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//读取事件
	virtual bool OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
