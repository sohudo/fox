// InsureLogon.cpp : 实现文件
//

#include "stdafx.h"
#include "InsureLogon.h"
#include "resource.h"
#include "GlobalUnits.h"
// CInsureLogon 对话框

IMPLEMENT_DYNAMIC(CInsureLogon, CDialog)
CInsureLogon::CInsureLogon(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_INSURE_LOGON, pParent)
{
	ZeroMemory(m_szInsurePassword,CountArray(m_szInsurePassword));

	//任务组件
	m_MissionManager.InsertMissionItem(this);

}

CInsureLogon::~CInsureLogon()
{
}

void CInsureLogon::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,IDC_BT_CLOSE,m_btClose);
	DDX_Control(pDX,IDOK,m_btOk);
	DDX_Control(pDX,IDCANCEL,m_btCancle);
	DDX_Control(pDX,IDC_INSURE_PASSWORD1,m_edInsurePassword);
}


BEGIN_MESSAGE_MAP(CInsureLogon, CDialog)
ON_WM_LBUTTONDOWN()
ON_WM_ERASEBKGND()
ON_BN_CLICKED(IDC_BT_CLOSE,OnCancel)
ON_BN_CLICKED(IDOK,OnBnClickedOk)
END_MESSAGE_MAP()


// CInsureLogon 消息处理程序

BOOL CInsureLogon::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ImageBack.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INSURE_LOGON_BACK"));


	m_KeyBoard.Create(IDD_PASSWORD_KEYBOARD2,this);
	m_KeyBoard.SetKeyBoardSink(this);
	

	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE);

	m_btOk.SetButtonImage(IDB_BT_OK,AfxGetInstanceHandle(),false,false);
	m_btCancle.SetButtonImage(IDB_BT_CANCLE,AfxGetInstanceHandle(),false,false);
	m_btClose.SetButtonImage(IDB_BT_QUIT,AfxGetInstanceHandle(),false,false);

	m_btClose.SetWindowPos(NULL,435,4,0,0,SWP_NOSIZE);
	m_btOk.SetWindowPos(NULL,159,154,0,0,SWP_NOSIZE);
	m_btCancle.SetWindowPos(NULL,249,154,0,0,SWP_NOSIZE);

	m_KeyBoard.SetWindowPos(NULL,76,214,0,0,SWP_NOSIZE);

	GetDlgItem(IDC_INSURE_PASSWORD1)->SetFocus();

	return TRUE; 
}

VOID CInsureLogon::OnKeyBoardChar(WORD wChar)
{
	CWnd* pWnd=GetFocus();
	if(pWnd->GetSafeHwnd()==m_edInsurePassword.GetSafeHwnd())
	{
		m_edInsurePassword.SendMessage(WM_CHAR,wChar,0);
	}
}

VOID CInsureLogon::OnKeyBoardClean()
{
	CWnd* pWnd=GetFocus();
	if(pWnd->GetSafeHwnd()==m_edInsurePassword.GetSafeHwnd())
	{
		m_edInsurePassword.SetWindowText(TEXT(""));
	}
}

void CInsureLogon::OnLButtonDown(UINT nFlags, CPoint point)
{
	
	//模拟标题
	if (point.y<30)
	{
		//模拟标题
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

		return;
	}

	__super::OnLButtonDown(nFlags, point);

}

VOID CInsureLogon::OnBnClickedOk()
{
	ZeroMemory(m_szInsurePassword,CountArray(m_szInsurePassword));

	GetDlgItemText(IDC_INSURE_PASSWORD1,m_szInsurePassword,sizeof(m_szInsurePassword));
	//密码判断
	if (m_szInsurePassword[0]==0)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("保险柜密码错误，请重新输入！"),MB_ICONERROR,0);

		GetDlgItem(IDC_INSURE_PASSWORD1)->SetFocus();

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

	m_btOk.EnableWindow(FALSE);
}

BOOL CInsureLogon::OnEraseBkgnd(CDC* pDC)
{
	m_ImageBack.DrawImage(pDC,0,0);

	return TRUE;
}

//连接事件
bool CInsureLogon::OnEventMissionLink(INT nErrorCode)
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
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
		//变量定义
		CMD_GP_UserInsureLogon InsureLogon;
		ZeroMemory(&InsureLogon,sizeof(InsureLogon));

		//加密密码
		TCHAR szPassword[LEN_PASSWORD]=TEXT("");
		CWHEncrypt::MD5Encrypt(m_szInsurePassword,szPassword);

		//构造数据
		InsureLogon.dwUserID=pGlobalUserData->dwUserID;
		lstrcpyn(InsureLogon.szPassword,szPassword,CountArray(InsureLogon.szPassword));
		CWHService::GetMachineIDEx(InsureLogon.szMachineID);

		//发送数据
		ASSERT(GetMissionManager()!=NULL);
		GetMissionManager()->SendData(MDM_GP_USER_SERVICE,SUB_GP_USER_INSURE_LOGON,&InsureLogon,sizeof(InsureLogon));
	}

	return true;
}

//关闭事件
bool CInsureLogon::OnEventMissionShut(BYTE cbShutReason)
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
bool CInsureLogon::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//命令处理
	if (Command.wMainCmdID==MDM_GP_USER_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_USER_INSURE_LOGON_SUCCESS:	//登陆成功
			{

				//关闭连接
				CMissionManager * pMissionManager=GetMissionManager();
				if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,false);

				//设置控件
				m_btOk.EnableWindow(TRUE);

				//加密密码
				TCHAR szPassword[LEN_PASSWORD]=TEXT("");
				CWHEncrypt::MD5Encrypt(m_szInsurePassword,szPassword);
				lstrcpy(CGlobalUnits::GetInstance()->m_szInsurePassword,szPassword);

				OnOK();

				return true; 
			}
		case SUB_GP_USER_INSURE_LOGON_FAILURE:	//操作失败
			{
				//效验参数
				CMD_GP_UserInsureLogonFailure * pUserInsureLogonFailure=(CMD_GP_UserInsureLogonFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_UserInsureLogonFailure)-sizeof(pUserInsureLogonFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_UserInsureLogonFailure)-sizeof(pUserInsureLogonFailure->szDescribeString))) return false;

				//关闭连接
				CMissionManager * pMissionManager=GetMissionManager();
				if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this,false);

				if (pUserInsureLogonFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pUserInsureLogonFailure->szDescribeString,MB_ICONERROR,60);					
				}

				//设置控件
				m_btOk.EnableWindow(TRUE);

				return true;
			}
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
				GetDlgItem(IDC_INSURE_PASSWORD1)->SetFocus();

				return true;
			}
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}
