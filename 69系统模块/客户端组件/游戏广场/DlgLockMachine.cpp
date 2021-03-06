#include "StdAfx.h"
#include "GamePlaza.h"
#include "DlgLockMachine.h"
#include ".\dlglockmachine.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgLockMachine, CDialog)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgLockMachine::CDlgLockMachine() : CDialog(IDD_DLG_USER_RECHARGE)
{
	//设置变量
	m_cbMachine=FALSE;
	ZeroMemory(m_szPassword,sizeof(m_szPassword));

	//任务组件
	m_MissionManager.InsertMissionItem(this);

	return;
}

//析够函数
CDlgLockMachine::~CDlgLockMachine()
{
}

//控件绑定
VOID CDlgLockMachine::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//绑定控件
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_PASSWORD, m_PasswordControl);
}

//创建函数
BOOL CDlgLockMachine::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	//SetWindowText(TEXT("绑定机器设置"));

	m_PasswordControl.SetLimitText(LEN_PASSWORD-1);

	m_ImageBack.LoadImage(AfxGetInstanceHandle(),TEXT("SERVICE_PAGE_2"));
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE);

	////获取大小
	//CRect rcClient;
	//GetClientRect(&rcClient);

	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//设置按钮
	m_cbMachine=(pGlobalUserData->cbMoorMachine==0)?TRUE:FALSE;
	m_btOk.SetWindowText(TEXT("")/*pGlobalUserData->cbMoorMachine==0)?TEXT("绑定本机"):TEXT("解除绑定")*/);
	m_btOk.SetButtonImage((pGlobalUserData->cbMoorMachine==0)?IDB_BT_LOCK:IDB_BT_UNLOCK,AfxGetInstanceHandle(),false,false);

	//CRect rcButton(0,0,0,0);
	//m_btOk.GetWindowRect(&rcButton);
	//m_btOk.MoveWindow(rcClient.Width()/2-rcButton.Width()-30,rcClient.Height()-32,rcButton.Width(),rcButton.Height(),SWP_NOZORDER|SWP_NOMOVE);
	//m_btCancel.MoveWindow(rcClient.Width()/2+30,rcClient.Height()-32,rcButton.Width(),rcButton.Height(),SWP_NOZORDER|SWP_NOMOVE);

	return TRUE;
}

//确定函数
VOID CDlgLockMachine::OnOK()
{
	//获取信息
	m_PasswordControl.GetWindowText(m_szPassword,CountArray(m_szPassword));

	//密码判断
	if (m_szPassword[0]==0)
	{
		//错误提示
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("密码不能为空，请输入保险柜密码进行绑定或者解除绑定！"),MB_ICONERROR);

		//设置焦点
		m_PasswordControl.SetFocus();

		return;
	}

	//变量定义
	ASSERT(GetMissionManager()!=NULL);
	CMissionManager * pMissionManager=GetMissionManager();

	//发起连接
	if (pMissionManager->AvtiveMissionItem(this,false)==false)
	{
		//错误提示
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("服务器连接失败，请检查网络配置以及网络连接状况！"),MB_ICONERROR);

		return;
	}

	//设置控件
	m_btOk.EnableWindow(FALSE);

	return;
}

//连接事件
bool CDlgLockMachine::OnEventMissionLink(INT nErrorCode)
{
	//错误处理
	if (nErrorCode!=0L)
	{
		//设置控件
		m_btOk.EnableWindow(TRUE);

		//显示提示
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("尝试了所有的服务器都无法成功连接服务器，请留意网站维护公告！"),MB_ICONERROR);
	}
	else
	{
		//变量定义
		CMD_GP_ModifyMachine ModifyMachine;
		ZeroMemory(&ModifyMachine,sizeof(ModifyMachine));

		//变量定义
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//设置变量
		ModifyMachine.cbBind=m_cbMachine;
		ModifyMachine.dwUserID=pGlobalUserData->dwUserID;

		//加密密码
		CWHService::GetMachineIDEx(ModifyMachine.szMachineID);
		CWHEncrypt::MD5Encrypt(m_szPassword,ModifyMachine.szPassword);

		//发送数据
		ASSERT(GetMissionManager()!=NULL);
		GetMissionManager()->SendData(MDM_GP_USER_SERVICE,SUB_GP_MODIFY_MACHINE,&ModifyMachine,sizeof(ModifyMachine));
	}

	return true;
}

//关闭事件
bool CDlgLockMachine::OnEventMissionShut(BYTE cbShutReason)
{
	//关闭处理
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//重试任务
		CMissionManager * pMissionManager=GetMissionManager();
		if (pMissionManager->AvtiveMissionItem(this,true)==true) return true;

		//设置控件
		m_btOk.EnableWindow(TRUE);

		//显示提示
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("由于当前服务器处理繁忙，帐号绑定操作处理失败，请稍后再重试！"),MB_ICONERROR);
	}

	return true;
}

//读取事件
bool CDlgLockMachine::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//命令处理
	if (Command.wMainCmdID==MDM_GP_USER_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_OPERATE_FAILURE:	//操作失败
			{
				//效验参数
				CMD_GP_OperateFailure * pOperateFailure=(CMD_GP_OperateFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString))) return false;

				//关闭连接
				CMissionManager * pMissionManager=GetMissionManager();
				if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,false);

				//显示消息
				if (pOperateFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pOperateFailure->szDescribeString,MB_ICONERROR,60);
				}

				//设置控件
				m_btOk.EnableWindow(TRUE);
				m_PasswordControl.SetFocus();

				return true;
			}
		case SUB_GP_OPERATE_SUCCESS:	//操作成功
			{
				//变量定义
				CMD_GP_OperateSuccess * pOperateSuccess=(CMD_GP_OperateSuccess *)pData;

				//效验数据
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString))) return false;

				//关闭连接
				CMissionManager * pMissionManager=GetMissionManager();
				if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,false);

				//变量定义
				CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
				tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

				//设置变量
				pGlobalUserData->cbMoorMachine=(m_cbMachine==TRUE)?TRUE:FALSE;

				//m_btOk.SetButtonImage((pGlobalUserData->cbMoorMachine==0)?IDB_BT_LOCK:IDB_BT_UNLOCK,AfxGetInstanceHandle(),false,false);

				//发送事件
				CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
				if (pPlatformEvent!=NULL) pPlatformEvent->SendPlatformEvent(EVENT_USER_MOOR_MACHINE,0L);

				//显示消息
				if (pOperateSuccess->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pOperateSuccess->szDescribeString,MB_ICONINFORMATION,60);
				}
				
				m_btOk.EnableWindow(TRUE);

				if(GetParent()!=NULL)	GetParent()->PostMessage(WM_CLOSE,0,0);
				//关闭窗口
			//	__super::OnCancel();

				return true;
			}
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////

BOOL CDlgLockMachine::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	m_ImageBack.DrawImage(pDC,0,0);

	return TRUE;
	//return __super::OnEraseBkgnd(pDC);
}
