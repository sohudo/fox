#include "Stdafx.h"
#include "GamePlaza.h"
#include "DlgUserInfo.h"
#include "GlobalUnits.h"
#include "PlatformFrame.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgUserInfo, CDialog)

	//系统消息
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgUserInfo::CDlgUserInfo() : CDialog(IDD_DLG_USER_INFO)
{
	
	m_pUserData=NULL;

	ZeroMemory(m_szStatus,CountArray(m_szStatus));
}

//析构函数
CDlgUserInfo::~CDlgUserInfo()
{
}

//控件绑定
VOID CDlgUserInfo::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//配置函数
BOOL CDlgUserInfo::OnInitDialog()
{
	__super::OnInitDialog();

	::SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(GetSafeHwnd(),GWL_EXSTYLE)|WS_EX_TRANSPARENT|WS_EX_LAYERED);
	 SetLayeredWindowAttributes(0, 150, LWA_ALPHA); //设置半透明  
	 //SetWindowPos(NULL,0,0,145,225,SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);
	 SetWindowPos(NULL,0,0,217,120,SWP_NOZORDER|SWP_NOMOVE|SWP_NOREDRAW);
	if (m_UserOrderParserModule.CreateInstance()==false) throw TEXT("会员等级组件创建失败！");
	
	return FALSE;
}

//绘画背景
BOOL CDlgUserInfo::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//建立缓冲
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//创建 DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pBufferDC=CDC::FromHandle(BufferDC);

	////加载资源
	//CPngImage ImageBack;
	//ImageBack.LoadImage(AfxGetInstanceHandle(),TEXT("USER_INFO_BACK"));
    pBufferDC->FillSolidRect(rcClient,RGB(255,255,255));
	//绘画背景
	//ImageBack.DrawImage(pBufferDC,0,0);
	if(m_pUserData!=NULL)
	{
			//设置环境
		pBufferDC->SetBkMode(TRANSPARENT);
		pBufferDC->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

        pBufferDC->SetTextColor(RGB(0,0,0));
		TCHAR szBuffer[128]=TEXT("");
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("用户名：%s"),m_pUserData->GetNickName());
		pBufferDC->TextOut(8,8,szBuffer);

		IUserOrderParser * pIUserOrderParser=m_UserOrderParserModule.GetInterface();
		
		//LPCTSTR pszMember=pIUserOrderParser->GetMemberDescribe(m_pUserData->GetMemberOrder());

		////用户信息
		//LPCTSTR pszUserOrder=pszMember[0]!=0?pszMember:TEXT("普通玩家");

		//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("会  员：%s"),pszUserOrder);
		//pBufferDC->TextOut(8,8+20,szBuffer);
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("欢乐豆：%I64d"),m_pUserData->GetUserScore());
		pBufferDC->TextOut(8,8+20,szBuffer);

		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("个性签名：%s"),m_pUserData->GetUnderWrite());
		pBufferDC->TextOut(8,8+20*2,szBuffer);

		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("状  态："));
		pBufferDC->TextOut(8,8+20*3,szBuffer);

		pBufferDC->TextOut(12,8+20*4,m_szStatus);

		//pBufferDC->TextOut(8,8+20*2,szBuffer);
		//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("积  分：%I64d"),m_pUserData->GetUserGrade());
		//pBufferDC->TextOut(8,8+20*3,szBuffer);
		//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("级  别：%s"),m_pIGameLevelParser->GetLevelDescribe(m_pUserData));
		//pBufferDC->TextOut(8,8+20*4,szBuffer);
		//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("    输：%d"),m_pUserData->GetUserLostCount());
		//pBufferDC->TextOut(8,8+20*5,szBuffer);  
		//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("    赢：%d"),m_pUserData->GetUserWinCount());
		//pBufferDC->TextOut(8,8+20*6,szBuffer);
		//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("    和：%d"),m_pUserData->GetUserDrawCount());
		//pBufferDC->TextOut(8,8+20*7,szBuffer);
		//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("    逃：%d"),m_pUserData->GetUserFleeCount());
		//pBufferDC->TextOut(8,8+20*8,szBuffer);
		//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("胜  率：%.2f%%"),m_pUserData->GetUserWinRate());
		//pBufferDC->TextOut(8,8+20*9,szBuffer);
		//_sntprintf(szBuffer,CountArray(szBuffer),TEXT("逃跑率：%.2f%%"),m_pUserData->GetUserFleeRate());
		//pBufferDC->TextOut(8,8+20*10,szBuffer);
	}

	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);

	return TRUE;
}


void CDlgUserInfo::ShowUserInfo(IClientUserItem*pUserData,LPCTSTR lpszStatus)
{
	ZeroMemory(m_szStatus,CountArray(m_szStatus));

	if(lpszStatus!=NULL) lstrcpy(m_szStatus,lpszStatus);

	m_pUserData=pUserData;
	if(m_pUserData==NULL)
		ShowWindow(SW_HIDE);
	else
	{
		ShowWindow(SW_SHOW);
	    RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE);
	}
}

//////////////////////////////////////////////////////////////////////////////////
