#pragma once


// CDlgModifyPassword 对话框

class CDlgModifyPassword : public CDialog, public CMissionItem
{
	DECLARE_DYNAMIC(CDlgModifyPassword)

	//变量定义
protected:
	bool							m_bLogonMission;					//修改任务
	CMissionManager					m_MissionManager;					//任务管理

	CPngImage						m_ImageBack;

	//密码资料
protected:
	TCHAR							m_szSrcPassword[LEN_PASSWORD];		//用户密码
	TCHAR							m_szDesPassword[LEN_PASSWORD];		//用户密码

	//控件变量
protected:
	CEdit							m_edLogonPassword1;					//帐号密码
	CEdit							m_edLogonPassword2;					//帐号密码
	CEdit							m_edLogonPassword3;					//帐号密码

	//控件变量
protected:
	CSkinButton						m_btLogonPassword;					//修改按钮

public:
	CDlgModifyPassword();   // 标准构造函数
	virtual ~CDlgModifyPassword();

	//网络事件
private:
	//连接事件
	virtual bool OnEventMissionLink(INT nErrorCode);
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//读取事件
	virtual bool OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize);

	//事件处理
protected:
	//开始任务
	VOID OnMissionStart();
	//终止任务
	VOID OnMissionConclude();

	//消息映射
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//密码输入
	VOID OnEnChangeLogonPassword();
	//密码修改
	VOID OnBnClickedLogonPassword();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	DECLARE_MESSAGE_MAP()
};
