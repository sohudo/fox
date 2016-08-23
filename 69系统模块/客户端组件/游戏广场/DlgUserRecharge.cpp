#include "StdAfx.h"
#include "GamePlaza.h"
#include "DlgUserRecharge.h"


//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgUserRecharge, CDialog)
	ON_WM_ERASEBKGND()
	//系统消息
	
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONDOWN()
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////
//圆角大小
#define ROUND_CX					1									//圆角宽度
#define ROUND_CY					1									//圆角高度
//屏幕位置
#define LAYERED_SIZE				5									//分层大小
#define CAPTION_SIZE				60									//标题大小
//构造函数
CDlgUserRecharge::CDlgUserRecharge() : CDialog(IDD_DLG_USER_RECHARGE)
{
	//设置变量
	m_cbMachine=FALSE;
	
	ZeroMemory(m_szID,sizeof(m_szID));
	ZeroMemory(m_szCard,sizeof(m_szCard));
    ZeroMemory(m_szPassword,sizeof(m_szPassword));
	
	//任务组件
	m_MissionManager.InsertMissionItem(this);

	return;
}

//析够函数
CDlgUserRecharge::~CDlgUserRecharge()
{
}

//控件绑定
VOID CDlgUserRecharge::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//绑定控件
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_ACCOUNTS, m_IDControl);
	DDX_Control(pDX, IDC_CARD_NUMBER, m_CardControl);
	DDX_Control(pDX, IDC_PASSWORD, m_PasswordControl);
}

//创建函数
BOOL CDlgUserRecharge::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	//SetWindowText(TEXT("绑定机器设置"));

	m_PasswordControl.SetLimitText(LEN_PASSWORD-1);

	m_ImageBack.LoadImage(AfxGetInstanceHandle(),TEXT("BT_USER_INFO_CHONGZHI"));
	

	
     //设置大小
	CSize SizeWindow(m_ImageBack.GetWidth(),m_ImageBack.GetHeight());
	SetWindowPos(NULL,0,0,SizeWindow.cx,SizeWindow.cy,SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);
	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
	TCHAR szControl[128]=TEXT("");
	_sntprintf(szControl,CountArray(szControl),TEXT("%d"),pGlobalUserData->dwGameID);
    SetDlgItemText(IDC_ACCOUNTS,szControl);
	//设置按钮
	//m_cbMachine=(pGlobalUserData->cbMoorMachine==0)?TRUE:FALSE;
	//m_btOk.SetWindowText(TEXT("")/*pGlobalUserData->cbMoorMachine==0)?TEXT("绑定本机"):TEXT("解除绑定")*/);
	//m_btOk.SetButtonImage((pGlobalUserData->cbMoorMachine==0)?IDB_BT_LOCK:IDB_BT_UNLOCK,AfxGetInstanceHandle(),false,false);
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btOk.SetButtonImage(IDB_BT_OK,hInstance,false,false);
    m_btCancel.SetButtonImage(IDB_BT_SERVICE_CLOSE,hInstance,false,false);
	//关闭按钮
	CRect rcCancel;
	m_btCancel.GetWindowRect(&rcCancel);
	m_btCancel.SetWindowPos(NULL,SizeWindow.cx-rcCancel.Width()-15,4,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOREDRAW);
	m_btOk.SetWindowPos(NULL,210,240,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOREDRAW);
	//居中窗口
	CenterWindow(this);

	//获取窗口
	CRect rcWindow;
	GetWindowRect(&rcWindow);

	//计算位置
	CRect rcUnLayered;
	rcUnLayered.top=LAYERED_SIZE;
	rcUnLayered.left=LAYERED_SIZE;
	rcUnLayered.right=rcWindow.Width()-LAYERED_SIZE;
	rcUnLayered.bottom=rcWindow.Height()-LAYERED_SIZE;

	//设置区域
	CRgn RgnWindow;
	RgnWindow.CreateRoundRectRgn(LAYERED_SIZE,LAYERED_SIZE,SizeWindow.cx-LAYERED_SIZE+1,SizeWindow.cy-LAYERED_SIZE+1,ROUND_CX,ROUND_CY);

	//设置区域
	SetWindowRgn(RgnWindow,FALSE);

	//分层窗口
	m_SkinLayered.CreateLayered(this,rcWindow);
	m_SkinLayered.InitLayeredArea(m_ImageBack,255,rcUnLayered,CPoint(ROUND_CX,ROUND_CY),false);
	return TRUE;
}

//确定函数
VOID CDlgUserRecharge::OnOK()
{
	//获取信息
	m_PasswordControl.GetWindowText(m_szPassword,CountArray(m_szPassword));
    m_IDControl.GetWindowText(m_szID,CountArray(m_szID));
	m_CardControl.GetWindowText(m_szCard,CountArray(m_szCard));
	//密码判断
	if (m_szPassword[0]==0)
	{
		//错误提示
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("充值卡密码不能为空"),MB_ICONERROR);

		//设置焦点
		m_PasswordControl.SetFocus();

		return;
	}
    if(m_szID[0]==0)
	{
      //错误提示
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("请输入充值的ID"),MB_ICONERROR);
		//设置焦点
		m_IDControl.SetFocus();
	}
	if(m_szCard[0]==0)
	{
         //错误提示
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("请输入充值的卡号"),MB_ICONERROR);
		//设置焦点
		m_CardControl.SetFocus();
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
bool CDlgUserRecharge::OnEventMissionLink(INT nErrorCode)
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
		CMD_GP_UserRecharge UserRecharge;
		ZeroMemory(&UserRecharge,sizeof(UserRecharge));

		//变量定义
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//设置变量
		//UserRecharge.szID=m_szID;
		//UserRecharge.szCard=m_szCard;
        lstrcpyn(UserRecharge.szID,m_szID,CountArray(UserRecharge.szID));
		lstrcpyn(UserRecharge.szCard,m_szCard,CountArray(UserRecharge.szCard));
		//加密密码
		//CWHService::GetMachineIDEx(ModifyMachine.szMachineID);
		CWHEncrypt::MD5Encrypt(m_szPassword,UserRecharge.szPassword);
        
		//发送数据
		ASSERT(GetMissionManager()!=NULL);
		GetMissionManager()->SendData(MDM_GP_USER_SERVICE,SUB_GP_USER_RECHARGE,&UserRecharge,sizeof(UserRecharge));
	}

	return true;
}

//关闭事件
bool CDlgUserRecharge::OnEventMissionShut(BYTE cbShutReason)
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
bool CDlgUserRecharge::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
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

				//if(GetParent()!=NULL)	GetParent()->PostMessage(WM_CLOSE,0,0);
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
//显示消息
VOID CDlgUserRecharge::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	//显示分层
	if (m_SkinLayered.m_hWnd!=NULL)
	{
		m_SkinLayered.ShowWindow((bShow==FALSE)?SW_HIDE:SW_SHOW);
	}

	return;
}

//鼠标消息
VOID CDlgUserRecharge::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//模拟标题
	if (Point.y<=CAPTION_SIZE)
	{
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));
	}

	return;
}

//位置改变
VOID CDlgUserRecharge::OnWindowPosChanged(WINDOWPOS * lpWndPos)
{
	__super::OnWindowPosChanging(lpWndPos);

	//移动分层
	if ((m_SkinLayered.m_hWnd!=NULL)&&(lpWndPos->cx>=0)&&(lpWndPos->cy>0))
	{
		m_SkinLayered.SetWindowPos(NULL,lpWndPos->x,lpWndPos->y,lpWndPos->cx,lpWndPos->cy,SWP_NOZORDER);
	}

	return;
}
BOOL CDlgUserRecharge::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	m_ImageBack.DrawImage(pDC,0,0);

	return TRUE;
	//return __super::OnEraseBkgnd(pDC);
}
