#ifndef DLG_INSURE_COUNTER_HEAD_FILE
#define DLG_INSURE_COUNTER_HEAD_FILE

#pragma once

#include "MissionManager.h"
#include "PasswordControl.h"
#include "WebPublicize.h"
#include "ShareControlHead.h"
#include "PasswordControlEx.h"
//////////////////////////////////////////////////////////////////////////////////

//类说明
class CDlgInsureItem;
class CDlgInsureMain;
class CDlgInsureSave;

//////////////////////////////////////////////////////////////////////////////////

//操作方式
#define INSURE_SAVE					2									//存取方式
#define INSURE_TRANSFER				0									//转账方式
#define INSURE_PASSWORD				3									//密码修改
#define INSURE_RECORD				1									//赠送记录
//////////////////////////////////////////////////////////////////////////////////

//银行信息
struct tagUserInsureInfo
{
	WORD							wRevenueTake;						//税收比例
	WORD							wRevenueTransfer;					//税收比例
	WORD							wServerID;							//房间标识
	SCORE							lUserScore;							//用户游戏币
	SCORE							lUserInsure;						//银行游戏币
	SCORE							lTransferPrerequisite;				//转账条件
};

//转账信息
struct tagTransInfo
{
	DWORD							dwSrcGameID;
	DWORD							dwTagGameID;
    
	TCHAR							szSrcNickName[LEN_NICKNAME];
	TCHAR							szTagNickName[LEN_NICKNAME];

	SCORE							lScore;
};
//////////////////////////////////////////////////////////////////////////////////

//银行接口
interface IInsureCounterAction
{
	//查询事件
	virtual VOID PerformQueryInfo()=NULL;
	//存入事件
	virtual VOID PerformSaveScore(SCORE lSaveScore)=NULL;
	//取出事件
	virtual VOID PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass)=NULL;
	//修改密码
	virtual VOID PerformModifyInsurePassword(LPCTSTR szSrcPassword, LPCTSTR szDesPassword)=NULL;
	//转账事件
	virtual VOID PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

//页面子类
class CDlgInsureItem
{
	//函数定义
public:
	//构造函数
	CDlgInsureItem();
	//析构函数
	virtual ~CDlgInsureItem();

	//功能函数
public:
	//获取游戏币
	SCORE GetUserControlScore(CWnd * pWndScore);

	//功能函数
public:
	//绘画数字
	VOID DrawNumberString(CDC * pDC, SCORE lScore, INT nXPos, INT nYPos);
	//转换字符
	VOID SwitchScoreString(SCORE lScore, LPTSTR pszBuffer, WORD wBufferSize);
	//转换字符
	VOID SwitchScoreFormat(SCORE lScore, UINT uSpace, LPTSTR pszBuffer, WORD wBufferSize);
};

//////////////////////////////////////////////////////////////////////////////////

//交易记录
class CDlgInsureRecord : public CDialog
{
public:
	CDlgInsureRecord();
	virtual ~CDlgInsureRecord();

protected:
	CWebPublicize		m_WebRecord;

protected:
	//确定按钮
	virtual VOID OnOK();
	//取消按钮
	virtual VOID OnCancel();


	DECLARE_MESSAGE_MAP()	
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

//////////////////////////////////////////////////////////////////////////////////
//存取游戏币
class CDlgInsureSave : public CDialog
{
	//友元定义
	friend class CDlgInsureMain;

	//辅助变量
protected:
	bool							m_bSwitchIng;						//转换标志
	CSkintDPI                       m_SkinDPI;                          //设置DPI
	CBitImage						m_ImageBack;

	//对象指针
protected:
	CDlgInsureMain *				m_pDlgInsureMain;					//窗口指针
	tagUserInsureInfo *				m_pUserInsureInfo;					//用户信息

	//控件变量
public:
	CSkinEditEx						m_edScore;							//操作游戏币
	CSkinButton						m_btTakeScore;						//取款按钮
	CSkinButton						m_btSaveScore;						//存款按钮

	//其他控件
public:
	CSkinHyperLink					m_ForgetInsure;						//忘记密码
	CDlgInsureItem					m_DlgInsureItem;					//银行子项
	CPasswordControl				m_PasswordControl;					//用户密码
	CSkinHyperLink					m_bt100W;
	CSkinHyperLink					m_bt500W;
	CSkinHyperLink					m_bt1000W;
	CSkinHyperLink					m_bt5000W;
	CSkinHyperLink					m_bt1Y;
	CSkinHyperLink					m_bt5Y;
	CSkinHyperLink					m_btAllInsure;
	CSkinHyperLink					m_btAllScore;;


	//函数定义
public:
	//构造函数
	CDlgInsureSave();
	//析构函数
	virtual ~CDlgInsureSave();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//创建函数
	virtual BOOL OnInitDialog();
	//确定按钮
	virtual VOID OnOK();
	//取消按钮
	virtual VOID OnCancel();

	//控件函数
protected:
	//输入信息
	VOID OnEnChangeScore();
	//存款按钮
	VOID OnBnClickedSaveScore();
	//取款按钮
	VOID OnBnClickedTakeScore();
	//忘记密码
	VOID OnBnClickedForgetInsure();
	//
	VOID OnBnClickedScore(UINT nID);

	//消息函数
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//控件颜色
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////
//转账结果

class CDlgTransferResult : public CSkinDialog
{
public:
	CDlgTransferResult();
	virtual ~CDlgTransferResult();

protected:
	CBitImage			   m_ImageBk;

	CDlgInsureItem			m_DlgInsureItem;					//银行子项

	tagTransInfo		   *m_pTransInfo;
    
protected:
	//确定按钮
	virtual VOID OnOK();
	//绘画消息
	virtual VOID OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight);

public:
	VOID SetTransInfo(tagTransInfo* pInfo);


	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

//////////////////////////////////////////////////////////////////////////////////

//转账金豆
class CDlgInsureTransfer : public CDialog
{
	//友元定义
	friend class CDlgInsureMain;

	//辅助变量
protected:
	bool							m_bSwitchIng;						//转换标志
	CSkintDPI                       m_SkinDPI;                          //设置DPI
	CBitImage						m_ImageBack;

	//对象指针
protected:
	CDlgInsureMain *				m_pDlgInsureMain;					//窗口指针
	tagUserInsureInfo *				m_pUserInsureInfo;					//用户信息

	//控件变量
public:
	CSkinEditEx						m_edScore;							//操作游戏币
	CSkinEditEx						m_edNickName;						//用户昵称
	CSkinButton						m_btTransferScore;					//转账按钮

	//其他控件
public:
	CSkinHyperLink					m_ForgetInsure;						//忘记密码
	CDlgInsureItem					m_DlgInsureItem;					//银行子项
	CPasswordControl				m_PasswordControl;					//用户密码

	CSkinHyperLink					m_bt100W;
	CSkinHyperLink					m_bt500W;
	CSkinHyperLink					m_bt1000W;
	CSkinHyperLink					m_bt5000W;
	CSkinHyperLink					m_bt1Y;
	CSkinHyperLink					m_bt5Y;

	//函数定义
public:
	//构造函数
	CDlgInsureTransfer();
	//析构函数
	virtual ~CDlgInsureTransfer();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//创建函数
	virtual BOOL OnInitDialog();
	//确定按钮
	virtual VOID OnOK();
	//取消按钮
	virtual VOID OnCancel();

	//控件函数
protected:
	//输入信息
	VOID OnEnChangeScore();
	//输入信息
	VOID OnEnChangeNick();
	//失去焦点
	VOID OnEnKillFocus();
	//忘记密码
	VOID OnBnClickedForgetInsure();
	//转账按钮
	VOID OnBnClickedTransferScore();
	//
	VOID OnBnClickedScore(UINT nID);

	//消息函数
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//控件颜色
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedById();
	afx_msg void OnBnClickedByName();
};

/////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

//存取游戏币
class CDlgInsurePassword : public CDialog,public IKeyBoardSink
{
	//友元定义
	friend class CDlgInsureMain;
	friend class CDlgInsurePlaza;
	friend class CDlgInsureServer;

	//辅助变量
protected:
	bool							m_bSwitchIng;						//转换标志
	CBitImage                       m_ImageBk;                          //设置DPI

	//对象指针
protected:
	CDlgInsureMain *				m_pDlgInsureMain;					//窗口指针
	tagUserInsureInfo *				m_pUserInsureInfo;					//用户信息


	//密码资料
protected:
	TCHAR							m_szSrcPassword[LEN_PASSWORD];		//用户密码
	TCHAR							m_szDesPassword[LEN_PASSWORD];		//用户密码

	//控件变量
public:
	CEdit							m_edInsurePassword1;				//二级密码
	CEdit							m_edInsurePassword2;				//二级密码
	CEdit							m_edInsurePassword3;				//二级密码
	CSkinButton						m_btTakeScore;						//修改按钮
	CSkinButton						m_btSaveScore;						//存款按钮
	CPasswordKeyboard2				m_KeyBoard;


	//其他控件
public:
	CDlgInsureItem					m_DlgInsureItem;					//银行子项

	//函数定义
public:
	//构造函数
	CDlgInsurePassword();
	//析构函数
	virtual ~CDlgInsurePassword();

public:
	//重置界面
	VOID ResetControlStatus();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//创建函数
	virtual BOOL OnInitDialog();
	//确定按钮
	virtual VOID OnOK();
	//取消按钮
	virtual VOID OnCancel();

	virtual	VOID OnKeyBoardChar(WORD wChar);
	virtual	VOID OnKeyBoardClean();

	//控件函数
protected:
	//输入信息
	VOID OnEnChangeScore();
	//存款按钮
	VOID OnBnClickedSaveScore();
	//取款按钮
	VOID OnBnClickedTakeScore();
	//忘记密码
	VOID OnBnClickedForgetInsure();

	//消息函数
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//控件颜色
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};



//////////////////////////////////////////////////////////////////////////////////

//保险柜类
class SHARE_CONTROL_CLASS CDlgInsureMain : public CDialog, public IInsureCounterAction
{
	//变量定义
protected:
	bool							m_bSaveMission;						//存款任务
	bool							m_bTakeMission;						//取款任务
	bool							m_bQueryMission;					//查询任务
	bool							m_bTransferMission;					//转账任务
	bool							m_bModifyPassword;					//密码修改

	LPTSTR							m_pInsurePass;						//银行密码
	//资源变量
protected:
	CPngImage						m_ImageBack;	

	//查询变量
protected:
	bool							m_bInitInfo;						//银行信息
	BYTE							m_cbInsureMode;						//银行模式
	tagUserInsureInfo				m_UserInsureInfo;					//银行信息

	tagTransInfo					m_TransInfo;

	//其他控件
protected:
	CSkinTabCtrl					m_TabControl;						//操作选择
	CSkinButton						m_btQueryInfo;						//查询按钮

	//子项窗口
protected:
	CDlgInsureItem					m_DlgInsureItem;					//银行子项
	CDlgInsureSave					m_DlgInsureSave;					//存取子项
	CDlgInsureTransfer				m_DlgInsureTransfer;				//转账子项
	CDlgInsureRecord				m_DlgInsureRecord;					//记录子项
	CDlgInsurePassword				m_DlgInsurePassword;				//银行密码
	//函数定义
public:
	//构造函数
	CDlgInsureMain();
	//析构函数
	virtual ~CDlgInsureMain();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//创建函数
	virtual BOOL OnInitDialog();
	//确定按钮
	virtual VOID OnOK();
	//
	virtual VOID OnCancel();

	//窗口函数
protected:
	//绘画消息
	virtual VOID OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight);

	//事件处理
public:
	//开始任务
	VOID OnMissionStart();
	//终止任务
	VOID OnMissionConclude();

	//设置密码
public:
	VOID SetInsurePassword(LPTSTR lpInsure);

	//功能函数
protected:
	//查询资料
	VOID QueryUserInsureInfo();
	//设置模式
	VOID SwitchInsureModeView(BYTE cbInsureMode);

	//消息函数
protected:
	//查询按钮
	VOID OnBnClickedQueryInfo();
	//类型改变
	VOID OnTcnSelchangeInsureMode(NMHDR * pNMHDR, LRESULT * pResult);
	//
	void OnLButtonDown(UINT nFlags, CPoint point);
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//广场银行
class SHARE_CONTROL_CLASS CDlgInsurePlaza : public CDlgInsureMain, public CMissionItem
{
	//变量定义
protected:
	BYTE                            m_cbByNickName;                       //昵称赠送
	SCORE							m_lScore;							//游戏币数目
	TCHAR							m_szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							m_szInsurePass[LEN_PASSWORD];		//银行密码

	//组件变量
protected:
	CMissionManager					m_MissionManager;					//任务管理

	//函数定义
public:
	//构造函数
	CDlgInsurePlaza();
	//析构函数
	virtual ~CDlgInsurePlaza();

	//执行函数
protected:
	//查询事件
	virtual VOID PerformQueryInfo();
	//存入事件
	virtual VOID PerformSaveScore(SCORE lSaveScore);
	//取出事件
	virtual VOID PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass);
	//修改密码
	virtual VOID PerformModifyInsurePassword(LPCTSTR szSrcPassword, LPCTSTR szDesPassword);
	//转账事件
	virtual VOID PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass);

	//网络事件
private:
	//连接事件
	virtual bool OnEventMissionLink(INT nErrorCode);
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//读取事件
	virtual bool OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////////////

//房间银行
class SHARE_CONTROL_CLASS CDlgInsureServer : public CDlgInsureMain
{
	//变量定义
protected:
	BYTE                            m_cbByNickName;                       //昵称赠送
	SCORE							m_lScore;							//游戏币数目
	TCHAR							m_szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							m_szInsurePass[LEN_PASSWORD];		//银行密码

	//变量定义
protected:
	ITCPSocket *					m_pITCPSocket;						//网络接口
	IClientUserItem *				m_pIMySelfUserItem;					//自己指针

	//函数定义
public:
	//构造函数
	CDlgInsureServer();
	//析构函数
	virtual ~CDlgInsureServer();

	//执行函数
protected:
	//查询事件
	virtual VOID PerformQueryInfo();
	//存入事件
	virtual VOID PerformSaveScore(SCORE lSaveScore);
	//取出事件
	virtual VOID PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass);
	//修改密码
	virtual VOID PerformModifyInsurePassword(LPCTSTR szSrcPassword, LPCTSTR szDesPassword);
	//转账事件
	virtual VOID PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass);

	//功能函数
public:
	//配置银行
	VOID Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem);
	//银行消息
	bool OnServerInsureMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////////////

#endif